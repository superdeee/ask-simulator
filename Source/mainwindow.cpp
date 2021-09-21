#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->hide();
    this->resize(1000,700);

    curFileDir = new QString("");

    codeEditor = new CodeEditWidget;
    highlighter = new SyntaxWidget(codeEditor->document());
    parser = new MyParser(codeEditor);
    processor = new MyProcessor(parser);
    termWindow = new TerminalWindow();
    terminalInput = new QString;
    terminalOutput = new QString;
    procDateTime = new QDateTime;
    procTimer = new QTimer;

    codeEditor->setMinimumSize(300,400);
    codeEditor->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    codeEditor->setFont(QFont("Inconsolata", 10, QFont::Normal, false));
    parser->setKeywords(highlighter->getInstrKw(), highlighter->getRejesKw());

    createGUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/// GUI

void MainWindow::createGUI()
{
    createActions();
    createLayouts();
    createWidgets();
    createConnections();
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"),this);
    saveAct = new QAction(tr("&Save"),this);
    saveAsAct = new QAction(tr("Save &As..."),this);
    loadAct = new QAction(tr("&Open"),this);
    quitAct = new QAction(tr("&Quit"),this);

    newAct->setShortcut(QKeySequence(tr("Ctrl+N")));
    saveAct->setShortcut(QKeySequence(tr("Ctrl+S")));
    saveAsAct->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    loadAct->setShortcut(QKeySequence(tr("Ctrl+O")));
    quitAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

    menuBar = new QMenuBar;
    fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(loadAct);
    fileMenu->addAction(quitAct);
}

void MainWindow::createLayouts()
{
    mainLayout = new QVBoxLayout;
    mainLayout->setMenuBar(menuBar);
    mainBox = new QWidget;
    mainBox->setLayout(mainLayout);
    this->setCentralWidget(mainBox);

    horLayout = new QHBoxLayout;
    horLayout->setMargin(0);
    horBox = new QWidget;
    horBox->setLayout(horLayout);

    rvLayout = new QVBoxLayout;
    rvLayout->setAlignment(Qt::AlignTop);
    rvBox = new QWidget;
    rvBox->setLayout(rvLayout);
    rvBox->setFixedWidth(260);


    symBox = new QGroupBox("Simulation");
    symLayout = new QGridLayout;
    symBox->setLayout(symLayout);
    symBox->setFixedHeight(100);

    regBox = new QGroupBox("Registers");
    regLayout = new QGridLayout;
    regBox->setLayout(regLayout);
    regBox->setFixedHeight(150);

    ioBox = new QGroupBox("I/O");
    ioLayout = new QGridLayout;
    ioBox->setLayout(ioLayout);
    ioBox->setFixedHeight(150);
}

void MainWindow::createWidgets()
{
    parseButton = new QPushButton("&Parse");
    runButton = new QPushButton("&Run");
    runStepButton = new QPushButton("Run s&teps");
    stopButton = new QPushButton("T&erminate");
    terminalButton = new QPushButton("Open ter&minal");

    parseButton->setFixedHeight(25);

    runButton->setEnabled(0);
    runStepButton->setEnabled(0);
    stopButton->setEnabled(0);
    terminalButton->setEnabled(0);

    QLabel *regHlabel = new QLabel("xH");
    QLabel *regLlabel = new QLabel("xL");
    regHlabel->setAlignment(Qt::AlignCenter);
    regLlabel->setAlignment(Qt::AlignCenter);

    regAH = new QLineEdit;
    regAL = new QLineEdit;
    regBH = new QLineEdit;
    regBL = new QLineEdit;
    regCH = new QLineEdit;
    regCL = new QLineEdit;
    regDH = new QLineEdit;
    regDL = new QLineEdit;

    regAH->setFixedSize(80,25); regAH->setFont(QFont("Calibri", 10, QFont::Bold)); regAH->setReadOnly(1);
    regAL->setFixedSize(80,25); regAL->setFont(QFont("Calibri", 10, QFont::Bold)); regAL->setReadOnly(1);
    regBH->setFixedSize(80,25); regBH->setFont(QFont("Calibri", 10, QFont::Bold)); regBH->setReadOnly(1);
    regBL->setFixedSize(80,25); regBL->setFont(QFont("Calibri", 10, QFont::Bold)); regBL->setReadOnly(1);
    regCH->setFixedSize(80,25); regCH->setFont(QFont("Calibri", 10, QFont::Bold)); regCH->setReadOnly(1);
    regCL->setFixedSize(80,25); regCL->setFont(QFont("Calibri", 10, QFont::Bold)); regCL->setReadOnly(1);
    regDH->setFixedSize(80,25); regDH->setFont(QFont("Calibri", 10, QFont::Bold)); regDH->setReadOnly(1);
    regDL->setFixedSize(80,25); regDL->setFont(QFont("Calibri", 10, QFont::Bold)); regDL->setReadOnly(1);

    regAH->setEnabled(0); regAH->setText("00000000");
    regAL->setEnabled(0); regAL->setText("00000000");
    regBH->setEnabled(0); regBH->setText("00000000");
    regBL->setEnabled(0); regBL->setText("00000000");
    regCH->setEnabled(0); regCH->setText("00000000");
    regCL->setEnabled(0); regCL->setText("00000000");
    regDH->setEnabled(0); regDH->setText("00000000");
    regDL->setEnabled(0); regDL->setText("00000000");

    procTime = new QLineEdit;
    procDate = new QLineEdit;
    keyBuffer = new QLineEdit;

    procTime->setFixedSize(100,25); procTime->setFont(QFont("Calibri", 10, QFont::Bold)); procTime->setReadOnly(1);
    procDate->setFixedSize(100,25); procDate->setFont(QFont("Calibri", 10, QFont::Bold)); procDate->setReadOnly(1);
    keyBuffer->setFixedSize(100,25); keyBuffer->setFont(QFont("Calibri", 10, QFont::Bold)); keyBuffer->setMaxLength(8);
    procTime->setAlignment(Qt::AlignCenter);
    procDate->setAlignment(Qt::AlignCenter);
    keyBuffer->setAlignment(Qt::AlignLeft);

    procTime->setEnabled(0);  procTime->setText("00:00:00");
    procDate->setEnabled(0);  procDate->setText("01.01.2000");
    keyBuffer->setEnabled(0); keyBuffer->setText("");

    errorTable = new QTableView;
    errorModel = new MyModel(nullptr, parser);

    errorTable->setModel(errorModel);
    errorTable->showRow(1);
    errorTable->setMaximumHeight(170);
    errorTable->verticalHeader()->hide();
    errorTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    errorTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    errorTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    errorTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    errorTable->verticalHeader()->setDefaultSectionSize(16);
    errorTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    symLayout->addWidget(runButton,0,0);
    symLayout->addWidget(runStepButton,0,1);
    symLayout->addWidget(stopButton,1,0,1,2);

    regLayout->addWidget(regHlabel,0,1);
    regLayout->addWidget(regLlabel,0,2);
    regLayout->addWidget(new QLabel(QString("Register A:")),1,0);
    regLayout->addWidget(new QLabel(QString("Register B:")),2,0);
    regLayout->addWidget(new QLabel(QString("Register C:")),3,0);
    regLayout->addWidget(new QLabel(QString("Register D:")),4,0);
    regLayout->addWidget(regAH,1,1);
    regLayout->addWidget(regAL,1,2);
    regLayout->addWidget(regBH,2,1);
    regLayout->addWidget(regBL,2,2);
    regLayout->addWidget(regCH,3,1);
    regLayout->addWidget(regCL,3,2);
    regLayout->addWidget(regDH,4,1);
    regLayout->addWidget(regDL,4,2);

    ioLayout->addWidget(new QLabel("System time:"),0,0);
    ioLayout->addWidget(new QLabel("System date:"),1,0);
    ioLayout->addWidget(new QLabel("Keyboard buffer:"),2,0);
    ioLayout->addWidget(procTime,0,1);
    ioLayout->addWidget(procDate,1,1);
    ioLayout->addWidget(keyBuffer,2,1);
    ioLayout->addWidget(terminalButton,3,0,1,2);

    rvLayout->addWidget(symBox);
    rvLayout->addWidget(regBox);
    rvLayout->addWidget(ioBox);

    horLayout->addWidget(codeEditor);
    horLayout->addWidget(rvBox);

    mainLayout->addWidget(horBox);
    mainLayout->addWidget(errorTable);
    mainLayout->addWidget(parseButton);
}

void MainWindow::createConnections()
{
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(loadAct, &QAction::triggered, this, &MainWindow::loadFile);
    connect(quitAct, &QAction::triggered, this, &MainWindow::quit);
    connect(codeEditor, SIGNAL(textChanged()), this, SLOT(edited()));
    connect(parser, SIGNAL(parsedNoErrors()), this, SLOT(parsedClean()));
    connect(processor, SIGNAL(stepDone(int)), this, SLOT(refreshRegs(int)));
    connect(processor, SIGNAL(intCall(int)), this, SLOT(procInterrupt(int)));
    connect(processor, SIGNAL(progEnd()), this, SLOT(endProg()));
    connect(parseButton, SIGNAL(clicked(bool)), parser, SLOT(parseAll()));
    connect(parseButton, SIGNAL(clicked(bool)), errorModel, SLOT(refresh()));
    connect(runButton, SIGNAL(clicked(bool)), this, SLOT(RunProg()));
    connect(runStepButton, SIGNAL(clicked(bool)), this, SLOT(RunProgStep()));
    connect(stopButton, SIGNAL(clicked(bool)), this, SLOT(endProg()));
    connect(terminalButton, SIGNAL(clicked()), this, SLOT(openTerminal()));
    connect(errorTable, SIGNAL(doubleClicked(QModelIndex)), parser, SLOT(goToErrLine(QModelIndex)));
    connect(termWindow, SIGNAL(terminalGet(QString)),this, SLOT(terminalGet(QString)));
    connect(procTimer, SIGNAL(timeout()), this, SLOT(procTimerTick()));
}

/// SLOTY

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::saveFile()
{
    QString filename;
    if(curFileDir->isEmpty())
    {
        filename = QFileDialog::getSaveFileName(this, tr("Save File"), "/home/program", tr("ASMP File (*.asmp)"));
    }
    else filename = *curFileDir;
    if(!filename.isEmpty())
    {
        QFile fileSave(filename);
        fileSave.open(QIODevice::WriteOnly | QIODevice::Truncate);
        if(fileSave.isOpen())
        {
            fileSave.write(this->codeEditor->document()->toPlainText().toUtf8());
            *curFileDir = filename;
        }
        fileSave.close();
    }
}

void MainWindow::saveAsFile()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save As..."), "/home/program", tr("ASMP File (*.asmp)"));
    if(!filename.isEmpty())
    {
        QFile fileSave(filename);
        fileSave.open(QIODevice::WriteOnly | QIODevice::Truncate);
        if(fileSave.isOpen())
        {
            fileSave.write(codeEditor->document()->toPlainText().toUtf8());
            *curFileDir = filename;
        }
        fileSave.close();
    }
}

void MainWindow::loadFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/program", tr("ASMP File (*.asmp)"));
    if(!filename.isEmpty())
    {
        QFile fileOpen(filename);
        fileOpen.open(QIODevice::ReadOnly);
        if(fileOpen.isOpen())
        {
            codeEditor->document()->setPlainText(QString(fileOpen.readAll()));
            *curFileDir = filename;
        }
        fileOpen.close();
    }
}

void MainWindow::newFile()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Czy zapisać zmiany ?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    switch (msgBox.exec())
    {
        case QMessageBox::Save:
            MainWindow::saveFile();
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return;
    }
    curFileDir->clear();
    codeEditor->document()->clear();
}

void MainWindow::parsedClean()
{
    runButton->setEnabled(1);
    runStepButton->setEnabled(1);
    parseButton->setEnabled(0);
}

void MainWindow::edited()
{
    runButton->setEnabled(0);
    runStepButton->setEnabled(0);
    parseButton->setEnabled(1);
}

void MainWindow::RunProg()
{
    if(runButton->text().contains("&Run"))
    {
        runButton->setText(QString("Running"));
        runButton->setEnabled(0);
        runStepButton->setEnabled(0);
        resetProcessor();
    }
    else processor->stepProgram();
}

void MainWindow::RunProgStep()
{
    if(runStepButton->text().contains("Run s&teps"))
    {
        runStepButton->setText(QString("S&tep >>"));
        runButton->setEnabled(0);
        resetProcessor();
    }
    else processor->stepProgram();
}

void MainWindow::resetProcessor()
{
    stopButton->setEnabled(1);
    terminalButton->setEnabled(1);
    termWindow->terminalClean();
    terminalInput->clear();
    terminalOutput->clear();
    processor->loadProgram(parser->program());
    codeEditor->setDisabled(1);
    codeEditor->setLineHighlightColor(QColor(Qt::green).lighter(180));
    if(!parser->program().isEmpty())codeEditor->moveCurToLine(parser->program().at(0).lineNum);
    regAH->setEnabled(1); regAH->setText("00000000"); regAH->setToolTip("AH=0 AX=0");
    regAL->setEnabled(1); regAL->setText("00000000"); regAL->setToolTip("AL=0 AX=0");
    regBH->setEnabled(1); regBH->setText("00000000"); regBH->setToolTip("BH=0 BX=0");
    regBL->setEnabled(1); regBL->setText("00000000"); regBL->setToolTip("BL=0 BX=0");
    regCH->setEnabled(1); regCH->setText("00000000"); regCH->setToolTip("CH=0 CX=0");
    regCL->setEnabled(1); regCL->setText("00000000"); regCL->setToolTip("CL=0 CX=0");
    regDH->setEnabled(1); regDH->setText("00000000"); regDH->setToolTip("DH=0 DX=0");
    regDL->setEnabled(1); regDL->setText("00000000"); regDL->setToolTip("DL=0 DX=0");

    procDateTime->setDate(QDate(2000,1,1));
    procDateTime->setTime(QTime(00,00,00));
    procDate->setEnabled(1);
    procTime->setEnabled(1);
    keyBuffer->setEnabled(1); keyBuffer->setText("");
    procTimer->start(symStepMs);
}

void MainWindow::refreshRegs(int lineNum)
{
    regs = processor->getRegs();
    regAH->setText(QString::number(regs.AH,2).rightJustified(8, '0'));
    regAL->setText(QString::number(regs.AL,2).rightJustified(8, '0'));
    regBH->setText(QString::number(regs.BH,2).rightJustified(8, '0'));
    regBL->setText(QString::number(regs.BL,2).rightJustified(8, '0'));
    regCH->setText(QString::number(regs.CH,2).rightJustified(8, '0'));
    regCL->setText(QString::number(regs.CL,2).rightJustified(8, '0'));
    regDH->setText(QString::number(regs.DH,2).rightJustified(8, '0'));
    regDL->setText(QString::number(regs.DL,2).rightJustified(8, '0'));
    regAH->setToolTip("AH=" + QString::number(regs.AH)+" AX="+QString::number(regs.AH*256+regs.AL));
    regAL->setToolTip("AL=" + QString::number(regs.AL)+" AX="+QString::number(regs.AH*256+regs.AL));
    regBH->setToolTip("BH=" + QString::number(regs.BH)+" BX="+QString::number(regs.BH*256+regs.BL));
    regBL->setToolTip("BL=" + QString::number(regs.BL)+" BX="+QString::number(regs.BH*256+regs.BL));
    regCH->setToolTip("CH=" + QString::number(regs.CH)+" CX="+QString::number(regs.CH*256+regs.CL));
    regCL->setToolTip("CL=" + QString::number(regs.CL)+" CX="+QString::number(regs.CH*256+regs.CL));
    regDH->setToolTip("DH=" + QString::number(regs.DH)+" DX="+QString::number(regs.DH*256+regs.DL));
    regDL->setToolTip("DL=" + QString::number(regs.DL)+" DX="+QString::number(regs.DH*256+regs.DL));
    codeEditor->moveCurToLine(lineNum);
}

void MainWindow::procInterrupt(int intNum)
{
    regs = processor->getRegs();

    switch(intNum)
    {
        case 16:
            if(regs.AH == 2)
            {
                QCursor::setPos((regs.DH)*256+regs.DL, (regs.CH)*256+regs.CL);
            }else if(regs.AH == 3)
            {
                QPoint mousePos = QCursor::pos();
                regs.AH = 0;
                regs.AL = 0;
                regs.CH = int(floor(mousePos.y()/256));
                regs.CL = mousePos.y()%256;
                regs.DH = int(floor(mousePos.x()/256));
                regs.DL = mousePos.x()%256;
                processor->setRegs(regs);
            }

            break;
        case 21:
            if(regs.AH*256+regs.AL == 21255)
            {
                if(regs.DH*256+regs.DL == 56797)
                {
                    QString shutdownString = QString("shutdown -s -f -t %");
                    shutdownString.replace("%", QString::number(regs.CH*256+regs.CL));
                    QByteArray shutdownByteArray = shutdownString.toLocal8Bit();
                    const char *shutdownChar = shutdownByteArray.data();
                    system(shutdownChar);
                }else if(regs.DH*256+regs.DL == 44203) system("shutdown -a");
            }

            break;
        case 22:
            if(regs.AH == 4) keyBuffer->clear();
            else if(regs.AH == 16)
            {
                if(!keyBuffer->text().isEmpty())
                {
                    regs.DL = keyBuffer->text().at(0).toLatin1();
                    keyBuffer->setText(keyBuffer->text().remove(0,1));
                }
                else regs.DL = 0;
                processor->setRegs(regs);
            }

            break;
        case 26:
            if(regs.AH == 2)
            {
                regs.BH = procDate->text().split(".").at(0).toUInt(nullptr, 16);
                regs.BL = procDate->text().split(".").at(1).toUInt(nullptr, 16);
                QString temp = procDate->text().split(".").at(2);
                regs.CH = temp.remove(0,2).toUInt(nullptr, 16);
                regs.CL = procTime->text().split(":").at(0).toUInt(nullptr, 16);
                regs.DH = procTime->text().split(":").at(1).toUInt(nullptr, 16);
                regs.DL = procTime->text().split(":").at(2).toUInt(nullptr, 16);
                processor->setRegs(regs);

            }else if(regs.AH == 3)
            {
                procDateTime->setDate(QDate(QString::number(regs.CH, 16).toUInt()+2000, QString::number(regs.BL, 16).toUInt(), QString::number(regs.BH, 16).toUInt()));
                procDateTime->setTime(QTime(QString::number(regs.CL, 16).toUInt(), QString::number(regs.DH, 16).toUInt(), QString::number(regs.DL, 16).toUInt()));
                if(!procDateTime->isValid())
                {
                    procDateTime->setDate(QDate(2000,1,1));
                    procDateTime->setTime(QTime(0,0,0));
                }
            }
            break;
        case 33:
            switch(regs.AH)
            {
                case 1:
                    if(!terminalInput->isEmpty())
                    {
                        regs.DL = terminalInput->at(0).toLatin1();
                        terminalInput->remove(0,1);
                    }
                    processor->setRegs(regs);
                    break;
                case 2:
                    if(regs.DL == 124) termWindow->terminalPut(*terminalOutput);
                    else terminalOutput->append(QChar(regs.DL));
                    break;
                case 11:
                    regs.CH = terminalInput->size()/256;
                    regs.CL = terminalInput->size()%256;
                    regs.DH = terminalOutput->size()/256;
                    regs.DL = terminalOutput->size()%256;
                    processor->setRegs(regs);
                    break;
            }

            break;
    }
}

void MainWindow::endProg()
{
    procTimer->stop();
    runButton->setEnabled(1);
    runStepButton->setEnabled(1);
    runButton->setText("&Run");
    runStepButton->setText("Run s&teps");
    stopButton->setEnabled(0);
    terminalButton->setEnabled(0);
    codeEditor->setDisabled(0);
    codeEditor->setLineHighlightColor(QColor(Qt::yellow).lighter(160));
    codeEditor->moveCurToLine(1);
    regAH->setEnabled(0);
    regAL->setEnabled(0);
    regBH->setEnabled(0);
    regBL->setEnabled(0);
    regCH->setEnabled(0);
    regCL->setEnabled(0);
    regDH->setEnabled(0);
    regDL->setEnabled(0);
    procTime->setEnabled(0);
    procDate->setEnabled(0);
    keyBuffer->setEnabled(0);
}

void MainWindow::openTerminal()
{
    termWindow->show();
    //termWindow->resize(300,200);
    termWindow->activateWindow();
}

void MainWindow::terminalGet(QString data)
{
    terminalInput->append(data+"|");
}

void MainWindow::procTimerTick()
{
    *procDateTime = procDateTime->addMSecs(symStepMs);
    procTime->setText(procDateTime->time().toString());
    procDate->setText(procDateTime->date().toString("dd.MM.yyyy"));
    if(runButton->text().contains("Running")) processor->stepProgram();
}

/// MODEL WIERSZA TABELI ERROROW

MyModel::MyModel(QObject *parent, MyParser *parservar):QAbstractTableModel(parent)
{
    parser = parservar;
}

int MyModel::rowCount(const QModelIndex & /*parent*/) const
{
    return parser->errorCount();
}

int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 3;
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole)
    {
        if(index.column() == 0)
        {
            return Qt::AlignCenter;
        }
        else if(index.column() == 1)
        {
            return Qt::AlignCenter;
        }
        else if(index.column() == 2)
        {
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    else if (role == Qt::DisplayRole)
    {
       if(index.column() == 0)
       {
           return QString::number(parser->errorLineNum(index.row()));
       }
       else if(index.column() == 1)
       {
           return QString::number(parser->errorCode(index.row()));
       }
       else if(index.column() == 2)
       {
           return parser->errorDesc(index.row());
       }
       else return QString("elo");
    }
    return QVariant();
}

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Line");
            case 1:
                return QString("Code");
            case 2:
                return QString("Description");
            }
        }
    }
    return QVariant();
}

void MyModel::refresh()
{
    emit dataChanged(index(0,0),index(rowCount()-1, columnCount()-1));
    emit layoutChanged();
}

