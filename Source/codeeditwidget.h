#ifndef CODEEDITWIDGET_H
#define CODEEDITWIDGET_H

#include <QObject>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextStream>
#include <QFile>
#include <QPainter>

class CodeEditWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    int lineNumberAreaWidth;
    int cursorPos;

    CodeEditWidget(QWidget *parent = nullptr);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void moveCurToLine(int lineNum);
    void setLineHighlightColor(QColor);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget *lineNumberArea;
    QColor lineColor;

private slots:
    void updateLineNumberArea(const QRect &, int);
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
};

class LineNumberWidget : public QWidget
{
public:
    LineNumberWidget(CodeEditWidget *editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditWidget *codeEdit;
};

class SyntaxWidget : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxWidget(QTextDocument *parent = nullptr);
    QStringList getInstrKw();
    QStringList getRejesKw();

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QStringList kwInstr;
    QStringList kwRejes;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // CODEEDITWIDGET_H
