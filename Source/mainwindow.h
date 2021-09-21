#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "myprocessor.h"
#include "terminalwindow.h"
#include "windows.h"
#include <QDebug>
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QLayout>
#include <QTableView>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextBlockFormat>
#include <QProcess>
#include <QDateTime>
#include <QTimer>


namespace Ui {
class MainWindow;
}

/// KLASA MODELU ERRORA do qtableview

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(QObject *parent, MyParser *parservar);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private:
    MyParser *parser;
public slots:
    void refresh();

};

///GLOWNA KLASA OKNA

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    int symStepMs = 100;
    Ui::MainWindow *ui;
    TerminalWindow *termWindow;

    QString *curFileDir;

    CodeEditWidget *codeEditor;
    SyntaxWidget *highlighter;
    MyParser *parser;
    MyProcessor *processor;

    QAction *newAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *loadAct;
    QAction *quitAct;

    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *symMenu;

    QWidget *mainBox;
    QWidget *horBox;
    QWidget *rvBox;
    QVBoxLayout *mainLayout;
    QHBoxLayout *horLayout;
    QVBoxLayout *rvLayout;

    QTableView *errorTable;
    MyModel *errorModel;

    QGroupBox *symBox;
    QGroupBox *regBox;
    QGroupBox *ioBox;
    QGridLayout *symLayout;
    QGridLayout *regLayout;
    QGridLayout *ioLayout;

    QPushButton *parseButton;
    QPushButton *runButton;
    QPushButton *runStepButton;
    QPushButton *stopButton;
    QPushButton *terminalButton;

    MyProcessor::RegSet regs;
    QLineEdit *regAH;
    QLineEdit *regAL;
    QLineEdit *regBH;
    QLineEdit *regBL;
    QLineEdit *regCH;
    QLineEdit *regCL;
    QLineEdit *regDH;
    QLineEdit *regDL;
    QLineEdit *procTime;
    QLineEdit *procDate;
    QLineEdit *keyBuffer;
    QString *terminalInput;
    QString *terminalOutput;
    QDateTime *procDateTime;
    QTimer *procTimer;

    void createGUI();
    void createActions();
    void createLayouts();
    void createWidgets();
    void createConnections();
    void resetProcessor();

private slots:
    void quit();
    void saveFile();
    void saveAsFile();
    void loadFile();
    void newFile();
    void parsedClean();
    void edited();
    void RunProg();
    void RunProgStep();
    void refreshRegs(int);
    void endProg();
    void procInterrupt(int);
    void openTerminal();
    void terminalGet(QString);
    void procTimerTick();
};

#endif // MAINWINDOW_H
