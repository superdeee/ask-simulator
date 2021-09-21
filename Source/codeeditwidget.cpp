#include "codeeditwidget.h"

/// WIDGET EDYTORA

CodeEditWidget::CodeEditWidget(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberWidget(this);
    lineColor = QColor(Qt::yellow).lighter(160);

    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(50);
    highlightCurrentLine();
}

void CodeEditWidget::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int)blockBoundingRect(block).height();
    while(block.isValid() && top <= event->rect().bottom())
    {
        if(block.isVisible() && bottom >= event->rect().top())
        {
            if(blockNumber == cursorPos) painter.setFont(QFont(painter.font().family(), painter.font().pointSize(), QFont::Bold, false));

            painter.setPen(Qt::black);

            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, lineNumberArea->width()-15, fontMetrics().height(), Qt::AlignRight, number);

            if(blockNumber == cursorPos) painter.setFont(QFont(painter.font().family(), painter.font().pointSize(), QFont::Normal, false));
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)blockBoundingRect(block).height();
        blockNumber++;
    }
}

void CodeEditWidget::moveCurToLine(int lineNum)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor, lineNum-1);
    setTextCursor(cursor);
    setFocus();
}

void CodeEditWidget::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth, cr.height()));
}

void CodeEditWidget::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
}

void CodeEditWidget::updateLineNumberAreaWidth(int width)
{
    lineNumberAreaWidth = width;
    setViewportMargins(lineNumberAreaWidth, 0, 0, 0);
}

void CodeEditWidget::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);

        cursorPos =  selection.cursor.blockNumber();
    }
    setExtraSelections(extraSelections);
}

void CodeEditWidget::setLineHighlightColor(QColor color)
{
    lineColor = color;
}

/// WIDGET NUMERACJI LINII

LineNumberWidget::LineNumberWidget(CodeEditWidget *editor) : QWidget(editor)
{
    codeEdit = editor;
}

QSize LineNumberWidget::sizeHint() const
{
    return QSize(codeEdit->lineNumberAreaWidth, 0);
}

void LineNumberWidget::paintEvent(QPaintEvent *event)
{
    codeEdit->lineNumberAreaPaintEvent(event);
}

/// WIDGET SYNTAX HIGHLIGHTERA

SyntaxWidget::SyntaxWidget(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    QFile kwFile(".../keywords.kw");
    kwFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!kwFile.isOpen())
    {
        qDebug("KEYWORDS NIE WCZYTANE !!!");
    }
    QTextStream keywords(&kwFile);

    kwInstr = keywords.readLine(0).split(" ");
    kwRejes = keywords.readLine(0).split(" ");

    kwFile.close();

    QMutableStringListIterator iter = kwInstr;
    while(iter.hasNext())
    {
        QString val = QString("\\b" + iter.next() + "\\b");
        iter.setValue(val);
    }

    iter = kwRejes;
    while(iter.hasNext())
    {
        QString val = QString("\\b" + iter.next() + "\\b");
        iter.setValue(val);
    }

    HighlightingRule rule;

    // syntax BLUE dla keywordow INSTRUKCJA
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    foreach (const QString &pattern, kwInstr)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // syntax RED dla keywordow REJESTR
    keywordFormat.setForeground(Qt::red);
    keywordFormat.setFontWeight(QFont::Bold);
    foreach (const QString &pattern, kwRejes)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    //syntax
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression("^#DEFINE[\\s]*(\\b((0x[A-F0-9]{2})|(0b[0-1]{8})|(0d([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])))\\b)[\\s]*[A-Za-z][A-Za-z0-9]{2,9}[\\.]");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // syntax MAGENTA dla patternow ADRES
    classFormat.setForeground(Qt::darkMagenta);
    classFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\\b((0x([A-F0-9]{2}){1,2})|(0b([0-1]{8}){1,2})|(0d([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65[0-4][0-9][0-9]|655[0-2][0-9]|6553[0-5])))\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    //syntax BOLD dla ETYKIET
    functionFormat.setFontWeight(QFont::Bold);
    //rule.pattern = QRegularExpression("\\b[0-9_]+\\b");
    //rule.pattern = QRegularExpression("\\s*(\\b^[A-Z]{2,4}([0-9]{0,2})\\b):\\s");
    rule.pattern = QRegularExpression("^\\s*(\\b[A-Z]{2,4}([0-9]{0,2})\\b):\\s");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // syntax DARK GREEN dla KOMENTARZY
    quotationFormat.setForeground(Qt::darkGreen);
    //rule.pattern = QRegularExpression("\".*\"");
    rule.pattern = QRegularExpression(";[^\n]*");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

}

void SyntaxWidget::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);
}

QStringList SyntaxWidget::getInstrKw()
{
    return kwInstr;
}

QStringList SyntaxWidget::getRejesKw()
{
    return kwRejes;
}
