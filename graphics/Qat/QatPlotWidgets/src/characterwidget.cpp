/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "QatPlotWidgets/characterwidget.h"
#include <iostream>
CharacterWidget::CharacterWidget(QWidget *parent)
    : QWidget(parent)
{
    m_squareSize = 24;
    m_columns = 16;
    m_lastKey = -1;
    setMouseTracking(true);
}

void CharacterWidget::updateFont(const QFont &font)
{
    m_displayFont.setFamily(font.family());
    m_squareSize = qMax(24, QFontMetrics(m_displayFont).xHeight() * 3);
    adjustSize();
    update();
}


void CharacterWidget::updateFamily(const QString &family)
{
    m_displayFont.setFamily(family);
    m_squareSize = qMax(24, QFontMetrics(m_displayFont).xHeight() * 3);
    adjustSize();
    update();
}

void CharacterWidget::updateSize(const QString &fontSize)
{
    m_displayFont.setPointSize(fontSize.toInt());
    m_squareSize = qMax(24, QFontMetrics(m_displayFont).xHeight() * 3);
    adjustSize();
    update();
}

void CharacterWidget::updateStyle(const QString &fontStyle)
{
    QFontDatabase fontDatabase;
    const QFont::StyleStrategy oldStrategy = m_displayFont.styleStrategy();
    m_displayFont = fontDatabase.font(m_displayFont.family(), fontStyle, m_displayFont.pointSize());
    m_displayFont.setStyleStrategy(oldStrategy);
    m_squareSize = qMax(24, QFontMetrics(m_displayFont).xHeight() * 3);
    adjustSize();
    update();
}

void CharacterWidget::updateFontMerging(bool enable)
{
    if (enable)
        m_displayFont.setStyleStrategy(QFont::PreferDefault);
    else
        m_displayFont.setStyleStrategy(QFont::NoFontMerging);
    adjustSize();
    update();
}

QSize CharacterWidget::sizeHint() const
{
    return QSize(m_columns*m_squareSize, (65536/m_columns)*m_squareSize);
}

void CharacterWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint widgetPosition = mapFromGlobal(event->globalPos());
    uint key = (widgetPosition.y()/m_squareSize)*m_columns + widgetPosition.x()/m_squareSize;

    QString text = QString::fromLatin1("<p>Character: <span style=\"font-size: 24pt; font-family: %1\">").arg(m_displayFont.family())
                  + QChar(key)
                  + QString::fromLatin1("</span><p>Value: 0x")
      + QString::number(key, 16) + QString ("Type " ) + QString::number(QChar(key).category()); 
    QToolTip::showText(event->globalPos(), text, this);
}

void CharacterWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastKey = (event->y()/m_squareSize)*m_columns + event->x()/m_squareSize;
        if (QChar(m_lastKey).category() != QChar::NoCategory)
            emit characterSelected(QString(QChar(m_lastKey)));
        update();
    }
    else
        QWidget::mousePressEvent(event);
}

void CharacterWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QBrush(Qt::white));
    painter.setFont(m_displayFont);

    QRect redrawRect = event->rect();
    int beginRow = redrawRect.top()/m_squareSize;
    int endRow = redrawRect.bottom()/m_squareSize;
    int beginColumn = redrawRect.left()/m_squareSize;
    int endColumn = redrawRect.right()/m_squareSize;

    painter.setPen(QPen(Qt::gray));
    for (int row = beginRow; row <= endRow; ++row) {
        for (int column = beginColumn; column <= endColumn; ++column) {
            painter.drawRect(column*m_squareSize, row*m_squareSize, m_squareSize, m_squareSize);
        }
    }

    QFontMetrics fontMetrics(m_displayFont);
    painter.setPen(QPen(Qt::black));
    for (int row = beginRow; row <= endRow; ++row) {

        for (int column = beginColumn; column <= endColumn; ++column) {

            int key = row*m_columns + column;
            painter.setClipRect(column*m_squareSize, row*m_squareSize, m_squareSize, m_squareSize);

            if (key == m_lastKey)
	    painter.fillRect(column*m_squareSize + 1, row*m_squareSize + 1, m_squareSize, m_squareSize, QBrush(Qt::red));
	    QChar Char(key);
	    //      QChar::Category cat =Char.category();
	    //	    if (QChar(key).isPrint()) 
            painter.drawText(column*m_squareSize + (m_squareSize / 2) - fontMetrics.width(QChar(key))/2,
                             row*m_squareSize + 4 + fontMetrics.ascent(),
                             QString(Char));

      }
    }
}
