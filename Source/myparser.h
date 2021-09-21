#ifndef MYPARSER_H
#define MYPARSER_H

#include "codeeditwidget.h"
#include <QModelIndex>
#include <QVector>
#include <QDebug>

class MyParser : public QObject
{
    Q_OBJECT

public:

    struct ExecInstruction
    {
        int lineNum;
        QString script;
        QString argOne;
        QString argTwo;
    };

    MyParser(CodeEditWidget *editorParent = nullptr);
    void setKeywords(QStringList instr, QStringList rejes);
    int errorCount();
    int errorLineNum(int errNum);
    int errorCode(int errNum);
    QString errorDesc(int errNum);
    QVector<ExecInstruction> program();

private:

    struct ParseLine
    {
        int lineNum;
        bool emptyLine;
        bool hasLabel;
        qint8 argNum;
        QString label;
        QString mnemonic;
        QString argOne;
        QString argTwo;
    };

    struct Instruction
    {
        QString mnemonic;
        int argNum;
        qint8 argTypes;
        QString job;
    };

    struct ParseError
    {
        int lineNum;
        int errorCode;
    };

    struct LabeledLine
    {
        int lineNum;
        QString label;
    };

    struct UserVar
    {
        QString varName;
        QString varDef;
    };

    QStringList codeLines;
    QVector<ParseLine> parseLineVec;
    QVector<ParseLine> executableVec;
    QVector<ParseError> errors;
    QVector<LabeledLine> labels;
    QVector<UserVar> variables;
    QVector<ExecInstruction> runVec;

    CodeEditWidget *editor;

    QStringList kwInstr;
    QStringList kwRejes;
    QStringList kwLabel;
    QStringList kwVar;
    QVector<Instruction> instructionSet;

    void readCode();
    void vectorize();
    void checkExec();
    qint8 argType(ParseLine);
    void printErrors();
    void compile();

public slots:
    void goToErrLine(QModelIndex);

private slots:
    void parseAll();

signals:
    void parsedErrors();
    void parsedNoErrors();
};

#endif // MYPARSER_H
