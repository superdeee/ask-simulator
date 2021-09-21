#ifndef MYPROCESSOR_H
#define MYPROCESSOR_H

#include "myparser.h"
#include <QObject>
#include <QStack>
#include <QScriptEngine>

class MyProcessor : public QObject
{
    Q_OBJECT

public:

    struct RegSet
    {
        quint8 AH,AL,BH,BL,CH,CL,DH,DL;
    };

    MyProcessor(MyParser *parserParent = nullptr);
    void loadProgram(QVector<MyParser::ExecInstruction> instrVec);
    void stepProgram();
    RegSet getRegs();
    void setRegs(RegSet);

private:


    MyParser *parser;

    QVector<MyParser::ExecInstruction> runVec;
    RegSet regs;
    int instructionPointer;
    QStack<int> *ipStack;
    QStack<quint16> *regStack;
    quint16 memory[256];
    bool endOfProg;

    QScriptEngine *procEngine;

    void wipeRegsMem();
    void execute();
    void updateRegs();
    void updateMem();
    void handleSpecInstructions(QString *scriptLine);
    void normalizeRegs();
    void eval16bregs();

signals:
    void stepDone(int);
    void intCall(int);
    void progEnd();

public slots:
};

#endif // MYPROCESSOR_H
