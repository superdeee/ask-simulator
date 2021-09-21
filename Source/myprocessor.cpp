#include "myprocessor.h"

MyProcessor::MyProcessor(MyParser *parserParent)
{
    parser = parserParent;
    ipStack = new QStack<int>;
    regStack = new QStack<quint16>;
    procEngine = new QScriptEngine;
}

void MyProcessor::loadProgram(QVector<MyParser::ExecInstruction> instrVec)
{
    runVec = instrVec;
    runVec.append(MyParser::ExecInstruction{100000, QString("end"), QString(""), QString("")});
    ipStack->clear();
    regStack->clear();
    wipeRegsMem();
    instructionPointer = 0;
    endOfProg = false;
}

void MyProcessor::stepProgram()
{
    if(!endOfProg)
    {
        execute();
        instructionPointer++;
        if(!endOfProg)emit stepDone(runVec.at(instructionPointer).lineNum);
        else emit progEnd();
    }
}

MyProcessor::RegSet MyProcessor::getRegs()
{
    return regs;
}

void MyProcessor::wipeRegsMem()
{
    procEngine->globalObject().setProperty("AX", 0);
    procEngine->globalObject().setProperty("BX", 0);
    procEngine->globalObject().setProperty("CX", 0);
    procEngine->globalObject().setProperty("DX", 0);
    procEngine->globalObject().setProperty("AH", 0);
    procEngine->globalObject().setProperty("AL", 0);
    procEngine->globalObject().setProperty("BH", 0);
    procEngine->globalObject().setProperty("BL", 0);
    procEngine->globalObject().setProperty("CH", 0);
    procEngine->globalObject().setProperty("CL", 0);
    procEngine->globalObject().setProperty("DH", 0);
    procEngine->globalObject().setProperty("DL", 0);
    for(int i=0; i<256; i++) procEngine->globalObject().setProperty(QString("M%1").arg(i), 0);

    updateRegs();
    updateMem();
}

void MyProcessor::execute()
{
    MyParser::ExecInstruction tempInstr;
    tempInstr = runVec.at(instructionPointer);
    QString scriptLine = tempInstr.script;

    if(scriptLine.contains("M$a") && tempInstr.argOne.toUInt() > 255) tempInstr.argOne = QString("null");
    if(scriptLine.contains("M$b") && tempInstr.argTwo.toUInt() > 255) tempInstr.argTwo = QString::number(0);

    scriptLine.replace("$a",tempInstr.argOne);
    scriptLine.replace("$b",tempInstr.argTwo);

    handleSpecInstructions(&scriptLine);

    qDebug() << scriptLine;
    procEngine->evaluate(scriptLine);

    if(tempInstr.argOne.contains("X"))
    {
        QString tempR = tempInstr.argOne.split("X").at(0);
        procEngine->evaluate(QString(tempR+"H=Math.floor("+tempR+"X/256)"));
        procEngine->evaluate(QString(tempR+"L="+tempR+"X%256"));
    }

    normalizeRegs();
    eval16bregs();

    updateRegs();
    if(tempInstr.script.contains("M$a")) updateMem();

    //qDebug() << "AH: " << regs.AH << "AL: " << regs.AL << "BH: " << regs.BH << "BL: " << regs.BL << "CH: " << regs.CH << "CL: " << regs.CL << "DH: " << regs.DH << "DL: " << regs.DL;

}

void MyProcessor::updateRegs()
{
    regs.AH = procEngine->evaluate("AH").toString().toUInt();
    regs.AL = procEngine->evaluate("AL").toString().toUInt();
    regs.BH = procEngine->evaluate("BH").toString().toUInt();
    regs.BL = procEngine->evaluate("BL").toString().toUInt();
    regs.CH = procEngine->evaluate("CH").toString().toUInt();
    regs.CL = procEngine->evaluate("CL").toString().toUInt();
    regs.DH = procEngine->evaluate("DH").toString().toUInt();
    regs.DL = procEngine->evaluate("DL").toString().toUInt();
}

void MyProcessor::updateMem()
{
    for(int i=0; i<256; i++)
    {
        memory[i] = procEngine->evaluate(QString("M%1").arg(i)).toString().toUInt();
    }
}

void MyProcessor::handleSpecInstructions(QString *scriptLine)
{
    if(scriptLine->contains("#sip<#ip"))
    {
        ipStack->push(instructionPointer);
        scriptLine->replace("#sip<#ip","");
    }

    if(scriptLine->contains("#sip>#ip"))
    {
        if(!ipStack->isEmpty()) instructionPointer = ipStack->pop();
        else                    instructionPointer = 0;
        scriptLine->replace("#sip>#ip","");
    }

    if(scriptLine->contains("#rip<"))
    {
        scriptLine->replace("#rip<","");
        regStack->push(procEngine->evaluate(*scriptLine).toUInt16());
    }

    if(scriptLine->contains("#rip>"))
    {
        scriptLine->replace("#rip>","");
        if(!regStack->isEmpty()) procEngine->evaluate(QString(scriptLine+QString("=%1").arg(regStack->pop())));
    }

    if(scriptLine->contains(";"))
    {
        QString scriptLine2 = scriptLine->split(";").at(1);
        if(scriptLine2.contains("#ip=")) instructionPointer = scriptLine2.split("=").at(1).toInt()-1;
        scriptLine->clear();
    }

    if(scriptLine->contains("!"))
    {
        updateRegs();
        emit intCall(scriptLine->split("!").at(1).toInt());
    }

    if(scriptLine->contains("end")) endOfProg = true;

}

void MyProcessor::normalizeRegs()
{
    procEngine->evaluate("AH=AH%256");
    procEngine->evaluate("AL=AL%256");
    procEngine->evaluate("BH=BH%256");
    procEngine->evaluate("BL=BL%256");
    procEngine->evaluate("CH=CH%256");
    procEngine->evaluate("CL=CL%256");
    procEngine->evaluate("DH=DH%256");
    procEngine->evaluate("DL=DL%256");

}

void MyProcessor::eval16bregs()
{
    procEngine->evaluate("AX=256*AH+AL");
    procEngine->evaluate("BX=256*BH+BL");
    procEngine->evaluate("CX=256*CH+CL");
    procEngine->evaluate("DX=256*DH+DL");
}

void MyProcessor::setRegs(RegSet newRegs)
{
    regs = newRegs;
    procEngine->globalObject().setProperty("AH", regs.AH);
    procEngine->globalObject().setProperty("AL", regs.AL);
    procEngine->globalObject().setProperty("BH", regs.BH);
    procEngine->globalObject().setProperty("BL", regs.BL);
    procEngine->globalObject().setProperty("CH", regs.CH);
    procEngine->globalObject().setProperty("CL", regs.CL);
    procEngine->globalObject().setProperty("DH", regs.DH);
    procEngine->globalObject().setProperty("DL", regs.DL);
    eval16bregs();
}
