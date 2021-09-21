#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QObject>
#include <QDialog>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QDebug>

class Console : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void getData(QString data);

public:
    explicit Console(QWidget *parent = nullptr);

    void putData(QString data);
    void setLocalEchoEnabled(bool set);
    void clearTerminal();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    bool m_localEchoEnabled = false;
    void deleteLine();
    void lineEntered();
};

class TerminalWindow : public QDialog
{
    Q_OBJECT

public:
    TerminalWindow();
    void terminalClean();
    void terminalPut(QString data);

private:
    Console *terminal;

private slots:
    void forwardData(QString);

signals:
    void terminalGet(QString);

};


#endif // TERMINALWINDOW_H
