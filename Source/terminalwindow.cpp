#include "terminalwindow.h"

TerminalWindow::TerminalWindow()
{
    terminal = new Console(this);
    terminal->setLocalEchoEnabled(1);
    setLayout(new QHBoxLayout);
    layout()->addWidget(terminal);
    terminalClean();
    connect(terminal, SIGNAL(getData(QString)), this, SLOT(forwardData(QString)));
}

void TerminalWindow::terminalClean()
{
    terminal->clearTerminal();
}

void TerminalWindow::forwardData(QString data)
{
    emit terminalGet(data);
}

void TerminalWindow::terminalPut(QString data)
{
    terminal->putData(data);
}

Console::Console(QWidget *parent) : QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
}

void Console::putData(QString data)
{
    deleteLine();
    insertPlainText(QString("proc>>" + data + "\n"));
    insertPlainText(QString("user>>"));

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    m_localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
        deleteLine();
        insertPlainText(QString("user>>"));
        break;
    case (Qt::Key_Enter):
        lineEntered();
        break;
    case (Qt::Key_Return):
        lineEntered();
        break;
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
    case Qt::Key_Home:
    case Qt::Key_End:
    case Qt::Key_Bar:
        break;
    default:
        if (m_localEchoEnabled) QPlainTextEdit::keyPressEvent(e);
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

void Console::deleteLine()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    setTextCursor(cursor);
}

void Console::lineEntered()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::LineUnderCursor);
    emit getData(cursor.selectedText().replace("user>>",""));
    qDebug() << QString(cursor.selectedText().replace("user>>","")+"\n");
    insertPlainText(QString("\nuser>>"));
}

void Console::clearTerminal()
{
    document()->clear();
    insertPlainText(QString("user>>"));
}
