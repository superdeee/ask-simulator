#include "myparser.h"

MyParser::MyParser(CodeEditWidget *editorParent)
{
    editor = editorParent;

    QFile isFile(".../instr_set.is");
    isFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!isFile.isOpen())
    {
        qDebug("INSTRUCTION SET NIE WCZYTANY !!!");
    }
    QTextStream iSet(&isFile);


    QStringList iSetList = iSet.readAll().split(QRegularExpression("[\n]"));
    QMutableStringListIterator iter = iSetList;
    Instruction tempInstr;
    QString tempInstrString;

    isFile.close();
    instructionSet.clear();

    while(iter.hasNext())
    {
        tempInstrString = iter.next();
        tempInstr.mnemonic = tempInstrString.split(",").at(0);
        tempInstr.argNum = tempInstrString.split(",").at(1).toInt();
        tempInstr.argTypes = tempInstrString.split(",").at(2).toUInt();
        tempInstr.job = tempInstrString.split(",").at(3);
        instructionSet.append(tempInstr);
    }

}

void MyParser::setKeywords(QStringList instr, QStringList rejes)
{
    kwInstr = instr;
    kwRejes = rejes;
}

int MyParser::errorCount()
{
    return errors.length();
}

int MyParser::errorLineNum(int errNum)
{
    return errors.at(errNum).lineNum;
}

int MyParser::errorCode(int errNum)
{
    return errors.at(errNum).errorCode;
}

QString MyParser::errorDesc(int errNum)
{
    switch(errors.at(errNum).errorCode)
    {
        case 1:
//            return QString("Nieznany operand");
            return QString("Unknown operand");
        case 2:
//            return QString("Nieprawidłowa liczba argumentów");
            return QString("Invalid number of arguments");
        case 3:
//            return QString("Nieprawidłowy argument");
            return QString("Invalid argument");
        case 31:
//            return QString("Nieprawidłowy argument pierwszy");
            return QString("Invalid first argument");
        case 32:
//            return QString("Nieprawidłowy argument drugi");
            return QString("Invalid second argument");
        case 404:
//            return QString("Nieprawidłowa składnia linii");
            return QString("Invalid syntax");
        case 5:
//            return QString("Nieprawidłowa nazwa etykiety");
            return QString("Invalid label");
        case 6:
//            return QString("Niepoprawna definicja zmiennej");
            return QString("Invalid variable definition");
    }
    return QString("Error %1 description!!!").arg(errNum);
}

QVector<MyParser::ExecInstruction> MyParser::program()
{
    return runVec;
}

void MyParser::readCode()
{
    QString fullCode = editor->document()->toPlainText();
    codeLines = fullCode.split("\n");

    QMutableStringListIterator iter = codeLines;
    QString tempVal;
    variables.clear();
    kwVar.clear();
    while(iter.hasNext())
    {
        tempVal = iter.next();
        tempVal.replace(QRegularExpression(";[^\n]*"),"");
        tempVal = tempVal.simplified();
        tempVal.replace(QRegularExpression(" , | ,|, "),",");
        if(tempVal.contains(QRegularExpression("^#DEFINE[\\s]*(\\b((0x[A-F0-9]{2})|(0b[0-1]{8})|(0d([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])))\\b)[\\s]*[A-Za-z][A-Za-z0-9]{2,9}[\\.]")))
        {
            UserVar tempVar;
            tempVal.replace("#DEFINE ", "");
            tempVal.replace(".", " ");
            tempVar.varDef = tempVal.split(" ").at(0);
            tempVar.varName =  tempVal.split(" ").at(1);
            variables.append(tempVar);
            kwVar.append(tempVar.varName);
            tempVal.replace(QRegularExpression("(\\b((0x[A-F0-9]{2})|(0b[0-1]{8})|(0d([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])))\\b)[\\s]*[A-Za-z][A-Za-z0-9]{2,9} "),"");
        }

        iter.setValue(tempVal);
    }
}

void MyParser::vectorize()
{
    QMutableStringListIterator iter = codeLines;
    QString tempVal;
    ParseLine line;

    parseLineVec.clear();

    while(iter.hasNext())
    {
        line.hasLabel = false;
        line.emptyLine = true;
        tempVal = iter.next();
        if(!tempVal.isEmpty())
        {
            line.emptyLine = false;
            line.hasLabel = false;
            if(tempVal.contains(":"))
            {
                line.hasLabel = true;
                line.label = tempVal.split(":").at(0);
                tempVal = tempVal.split(":").at(1);
                tempVal = tempVal.simplified();
                if(tempVal.isEmpty())
                {
                    line.emptyLine = true;
                    line.lineNum = parseLineVec.size()+1;
                    parseLineVec.append(line);
                    continue;
                }
            }
            line.mnemonic = tempVal.split(" ").at(0);
            if(tempVal.contains("#"))
            {
                line.argNum = 101;
                line.lineNum = parseLineVec.size()+1;
                parseLineVec.append(line);
                continue;
            }
            if(tempVal.split(" ").length() > 2)
            {
                line.argNum = 100;
                line.lineNum = parseLineVec.size()+1;
                parseLineVec.append(line);
                continue;
            }
            if(tempVal.contains(","))
            {
                line.argNum = 2;
                line.argOne = tempVal.split(" ").at(1).split(",").at(0);
                line.argTwo = tempVal.split(" ").at(1).split(",").at(1);
            }
            else if(tempVal.contains(" ")){
                line.argNum = 1;
                line.argOne = tempVal.split(" ").at(1);
            }
            else
            {
                line.argNum = 0;
            }
        }else
        {
            line.emptyLine = true;
        }
        line.lineNum = parseLineVec.size()+1;
        parseLineVec.append(line);
        //qDebug() << "Line: " << line.lineNum << " Pusta: " << line.emptyLine << " ArgNum: " << line.argNum << " Mnemo: " << line.mnemonic << " Arg1: " << line.argOne << " Arg2: " << line.argTwo;
    }
}

void MyParser::checkExec()
{
    QVectorIterator<ParseLine> iter = parseLineVec;
    ParseLine tempLine;

    executableVec.clear();
    errors.clear();
    labels.clear();
    kwLabel.clear();

    while(iter.hasNext())
    {
        tempLine = iter.next();
        if(!tempLine.emptyLine || tempLine.hasLabel)
        {
            executableVec.append(tempLine);
            //qDebug() << "Line: " << tempLine.lineNum << " Pusta: " << tempLine.emptyLine << " Et: " << tempLine.hasLabel << " ArgNum: " << tempLine.argNum << " Mnemo: " << tempLine.mnemonic << " Arg1: " << tempLine.argOne << " Arg2: " << tempLine.argTwo;
        }
    }

    QRegularExpression pattern;
    QVectorIterator<Instruction> iter2 = instructionSet;
    Instruction tempInstr;

    iter = executableVec;

    while(iter.hasNext())
    {
        tempLine = iter.next();
        if(tempLine.argNum == 100) continue;
        if(tempLine.hasLabel)
        {
            if(!tempLine.label.contains(" ") && tempLine.label.contains(QRegularExpression("\\b[A-Z]{2,4}([0-9]{0,2})\\b")))
            {
                LabeledLine tempLabel;
                tempLabel.lineNum = tempLine.lineNum;
                tempLabel.label = tempLine.label;
                labels.append(tempLabel);
                kwLabel.append(QString("\\b"+tempLine.label+"\\b"));
            }else
            {
                ParseError tempError;
                tempError.lineNum = tempLine.lineNum;
                tempError.errorCode = 5;
                errors.append(tempError);
            }
        }
    }

    iter = executableVec;
    while(iter.hasNext())
    {
        bool mnemOK = false;
        tempLine = iter.next();
        if(tempLine.argNum == 100)
        {
            ParseError tempError;
            tempError.lineNum = tempLine.lineNum;
            tempError.errorCode = 404;
            errors.append(tempError);
            continue;
        }
        if(tempLine.argNum == 101)
        {
            ParseError tempError;
            tempError.lineNum = tempLine.lineNum;
            tempError.errorCode = 6;
            errors.append(tempError);
            continue;
        }
        if(tempLine.hasLabel)
        {
            if(!tempLine.label.contains(" ") && tempLine.label.contains(QRegularExpression("\\b[A-Z]{2,4}([0-9]{0,2})\\b")))
            {
                LabeledLine tempLabel;
                tempLabel.lineNum = tempLine.lineNum;
                tempLabel.label = tempLine.label;
                labels.append(tempLabel);
            }
        }

        foreach (const QString &pattern, kwInstr)
        {
            if(tempLine.mnemonic.contains(QRegularExpression(pattern))) mnemOK = true;
        }
        if(mnemOK)
        {
            iter2.toFront();
            while(iter2.hasNext())
            {
                tempInstr = iter2.next();

                if(tempLine.mnemonic == tempInstr.mnemonic)
                {
                    if(tempLine.argNum != tempInstr.argNum)
                    {
                        ParseError tempError;
                        tempError.lineNum = tempLine.lineNum;
                        tempError.errorCode = 2;
                        errors.append(tempError);
                    }else
                    {
                        qint8 argument = argType(tempLine);
                        if(tempLine.argNum == 0) argument &= 0x00;
                        else if(tempLine.argNum == 1) argument &= 0xf0;
                        else if(tempLine.argNum == 2) argument &= 0xff;

                        argument |= tempInstr.argTypes;
                        if((argument & 0xf0) != (tempInstr.argTypes & 0xf0))
                        {
                            ParseError tempError;
                            tempError.lineNum = tempLine.lineNum;
                            if(tempLine.argNum == 2) tempError.errorCode = 31;
                            else tempError.errorCode = 3;
                            errors.append(tempError);
                        }
                        if((argument & 0x0f) != (tempInstr.argTypes & 0x0f))
                        {
                            ParseError tempError;
                            tempError.lineNum = tempLine.lineNum;
                            tempError.errorCode = 32;
                            errors.append(tempError);
                        }
                    }
                }
            }
        }else
        {
            ParseError tempError;
            tempError.lineNum = tempLine.lineNum;
            tempError.errorCode = 1;
            errors.append(tempError);
        }
    }
}

qint8 MyParser::argType(ParseLine line)
{
    qint8 args = 0x00;

    foreach (const QString &pattern, kwRejes)
    {
        if(line.argOne.contains(QRegularExpression(pattern))) args|=0x10;
        if(line.argTwo.contains(QRegularExpression(pattern))) args|=0x01;
    }

    foreach (const QString &pattern, kwLabel)
    {
        if(line.argOne.contains(QRegularExpression(pattern))) args|=0x40;
        if(line.argTwo.contains(QRegularExpression(pattern))) args|=0x04;
    }

    foreach (const QString &pattern, kwVar)
    {
        if(line.argOne.contains(QRegularExpression(pattern))) args|=0x80;
        if(line.argTwo.contains(QRegularExpression(pattern))) args|=0x08;
    }

    if(line.argOne.contains(QRegularExpression("\\b((0x([A-F0-9]{2}){1,2})|(0b([0-1]{8}){1,2})|(0d([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65[0-4][0-9][0-9]|655[0-2][0-9]|6553[0-5])))\\b")))
        args|=0x20;
    if(line.argTwo.contains(QRegularExpression("\\b((0x([A-F0-9]{2}){1,2})|(0b([0-1]{8}){1,2})|(0d([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65[0-4][0-9][0-9]|655[0-2][0-9]|6553[0-5])))\\b")))
        args|=0x02;

    if((args & 0xf0) == 0) args |= 0xf0;
    if((args & 0x0f) == 0) args |= 0x0f;


    return args;
}

void MyParser::printErrors()
{
    QVectorIterator<ParseError> iter = errors;
    ParseError tempError;
    while(iter.hasNext())
    {
        tempError = iter.next();
        qDebug() << "In line " << tempError.lineNum << " error code " << tempError.errorCode;
    }
}

void MyParser::compile()
{
    ParseLine tempLine;
    Instruction tempInstr;
    UserVar tempVar;
    LabeledLine tempLabel;
    int tempLineNum;
    QMutableVectorIterator<ParseLine> iterLine = executableVec;
    QVectorIterator<Instruction> iterInstr = instructionSet;
    QVectorIterator<UserVar> iterVar = variables;
    QVectorIterator<LabeledLine> iterLabel = labels;
    QVector<int> lineNumVec;


    runVec.clear();

    while(iterLine.hasNext())
    {
        tempLine = iterLine.next();
        if(tempLine.emptyLine)iterLine.remove();
        else lineNumVec.append(tempLine.lineNum);
    }
    iterLine.toFront();

    QVectorIterator<int> iterLineNum = lineNumVec;

    while(iterLine.hasNext())
    {
        tempLine = iterLine.next();
        iterVar.toFront();
        while(iterVar.hasNext())
        {
            tempVar = iterVar.next();
            if(tempLine.argOne == tempVar.varName)tempLine.argOne = tempVar.varDef;
            if(tempLine.argTwo == tempVar.varName)tempLine.argTwo = tempVar.varDef;
        }
        iterLabel.toFront();
        while(iterLabel.hasNext())
        {
            tempLabel = iterLabel.next();
            if(tempLine.argOne == tempLabel.label) tempLine.argOne = QString::number(tempLabel.lineNum);
        }
        if(tempLine.argOne.contains(QRegularExpression("\\b((0x([A-F0-9]{2}){1,2})|(0b([0-1]{8}){1,2})|(0d([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65[0-4][0-9][0-9]|655[0-2][0-9]|6553[0-5])))\\b")))
        {
            if(tempLine.argOne.contains("0x"))
            {
                tempLine.argOne =  QString::number(tempLine.argOne.remove(0,2).toUInt(nullptr, 16));
            }else if(tempLine.argOne.contains("0d"))
            {
                tempLine.argOne = QString::number(tempLine.argOne.remove(0,2).toUInt(nullptr, 10));
            }else
            {
                tempLine.argOne = QString::number(tempLine.argOne.remove(0,2).toUInt(nullptr, 2));
            }
        }
        if(tempLine.argTwo.contains(QRegularExpression("\\b((0x([A-F0-9]{2}){1,2})|(0b([0-1]{8}){1,2})|(0d([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65[0-4][0-9][0-9]|655[0-2][0-9]|6553[0-5])))\\b")))
        {
            if(tempLine.argTwo.contains("0x"))
            {
                tempLine.argTwo = QString::number(tempLine.argTwo.remove(0,2).toUInt(nullptr, 16));
            }else if(tempLine.argTwo.contains("0d"))
            {
                tempLine.argTwo = QString::number(tempLine.argTwo.remove(0,2).toUInt(nullptr, 10));
            }else
            {
                tempLine.argTwo = QString::number(tempLine.argTwo.remove(0,2).toUInt(nullptr, 2));
            }
        }
        iterLine.setValue(tempLine);
        //qDebug() << "Line: " << tempLine.lineNum << " Pusta: " << tempLine.emptyLine << " Et: " << tempLine.hasLabel << " ArgNum: " << tempLine.argNum << " Mnemo: " << tempLine.mnemonic << " Arg1: " << tempLine.argOne << " Arg2: " << tempLine.argTwo;
    }

    iterLine.toFront();
    int i;
    bool notFound;
    while(iterLine.hasNext())
    {
        tempLine = iterLine.next();
        if(tempLine.mnemonic.contains("JMP"))
        {
            iterLineNum.toFront();
            i = 0;
            notFound = 1;
            while(iterLineNum.hasNext() && notFound)
            {
                tempLineNum = iterLineNum.next();
                if(tempLineNum >= tempLine.argOne.toInt())
                {
                    tempLine.argOne = QString::number(i);
                    notFound = 0;
                }
                i++;
            }
            iterLine.setValue(tempLine);
            qDebug() << "Line: " << tempLine.lineNum << " Pusta: " << tempLine.emptyLine << " Et: " << tempLine.hasLabel << " ArgNum: " << tempLine.argNum << " Mnemo: " << tempLine.mnemonic << " Arg1: " << tempLine.argOne << " Arg2: " << tempLine.argTwo;
        }
    }

    iterLine.toFront();
    ExecInstruction tempExecInstr;

    while(iterLine.hasNext())
    {
        tempLine = iterLine.next();
        iterInstr.toFront();
        while(iterInstr.hasNext())
        {
            tempInstr = iterInstr.next();
            if(tempLine.mnemonic == tempInstr.mnemonic)
            {
                tempLine.mnemonic = tempInstr.job;
            }
        }

        tempExecInstr.lineNum = tempLine.lineNum;
        tempExecInstr.script = tempLine.mnemonic;
        if(tempLine.argNum == 2)
        {
            tempExecInstr.argOne = tempLine.argOne;
            tempExecInstr.argTwo = tempLine.argTwo;
        }else if(tempLine.argNum == 1)
        {
            tempExecInstr.argOne = tempLine.argOne;
            tempExecInstr.argTwo.clear();
        }else
        {
            tempExecInstr.argOne.clear();
            tempExecInstr.argTwo.clear();
        }
        runVec.append(tempExecInstr);
        qDebug() << "Line: " << tempExecInstr.lineNum << " Script: " << tempExecInstr.script << " Arg1: " << tempExecInstr.argOne << " Arg2: " << tempExecInstr.argTwo;
    }
}

void MyParser::goToErrLine(QModelIndex index)
{
    editor->moveCurToLine(errors.at(index.row()).lineNum);
}

void MyParser::parseAll()
{
    readCode();
    vectorize();
    checkExec();
    printErrors();
    if(errors.isEmpty())
    {
        compile();
        emit parsedNoErrors();
    }
    else emit parsedErrors();
}
