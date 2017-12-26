/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1ColorSelectButton           //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1Base/VP1Msg.h"

#include <QColor>
#include <QColorDialog>
#include <QTimer>
#include <QShortcut>

//____________________________________________________________________
class VP1ColorSelectButton::Imp {
public:
  Imp(const QColor& col, VP1ColorSelectButton* but,int the_dim) : presentcolor(col),button(but),dim(the_dim) {}
  QColor presentcolor;
  VP1ColorSelectButton* button;
  int dim;
};

//____________________________________________________________________
VP1ColorSelectButton::VP1ColorSelectButton(QWidget*parent,const QColor& initialcolor,int dim)
  : QPushButton(parent), m_d(new Imp(initialcolor,this,dim))
{
  connect (this,SIGNAL(clicked()),this,SLOT(launchColorChooser()));
  QTimer::singleShot(0, this, SLOT(updateButton()));
}

//____________________________________________________________________
void VP1ColorSelectButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("VP1ColorSelectButton");
  setColButtonProperties(this,m_d->presentcolor,m_d->dim);
}

//____________________________________________________________________
void VP1ColorSelectButton::setColButtonProperties(QPushButton* pb,const QColor& col,int dim) {

//  if (VP1Msg::verbose())
//    VP1Msg::messageVerbose("setColButtonProperties: button="+VP1Msg::str(pb)+", color="+VP1Msg::str(col));

  if (pb->objectName().isEmpty()) {
    VP1Msg::messageDebug("VP1ColorSelectButton::setColButtonProperties() WARNING: Empty objectName()!!");
    pb->setObjectName("VP1ColorSelectButton_setColButtonProperties");
  }

  if (!pb->text().isEmpty()) {
    VP1Msg::messageDebug("VP1ColorSelectButton::setColButtonProperties() WARNING: Noticed non-empty text field. Clearing!");
    pb->QPushButton::setText("");
  }

  pb->setStyleSheet("QPushButton#"+pb->objectName()+" { background-color: "
		+col.name()+"; border-color: "+col.name()+"; }");
  pb->setFocusPolicy(Qt::NoFocus);
  if (dim>0) {
    QSize sz = QSize(dim,dim);
    QSizePolicy sp(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sp.setHorizontalStretch(0);
    sp.setVerticalStretch(0);
    sp.setHeightForWidth(pb->sizePolicy().hasHeightForWidth());
    pb->setSizePolicy(sp);
    pb->setMinimumSize(sz);
    pb->setMaximumSize(sz);
  }
}

//____________________________________________________________________
VP1ColorSelectButton::~VP1ColorSelectButton()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
void VP1ColorSelectButton::launchColorChooser()
{
  VP1Msg::messageVerbose("VP1ColorSelectButton emitting aboutToShowColorDialog()");
  emit aboutToShowColorDialog();
  QColor color = QColorDialog::getColor(m_d->presentcolor, this);
  setColor(color);
}

//____________________________________________________________________
void VP1ColorSelectButton::setColor(const QColor&col)
{
  if (!col.isValid())
    return;
  if (m_d->presentcolor==col)
    return;
  m_d->presentcolor=col;
  updateButton();
  VP1Msg::messageVerbose("VP1ColorSelectButton emitting colorChanged("+col.name()+")");
  emit colorChanged(col);
}

//____________________________________________________________________
QColor VP1ColorSelectButton::color() const
{
  return m_d->presentcolor;
}

//____________________________________________________________________
void VP1ColorSelectButton::setText( const QString & s )
{
  if (!s.isEmpty())
    VP1Msg::messageDebug("VP1ColorSelectButton::setText() called (with '"+s+"'), but not allowed!");
}

//____________________________________________________________________
void VP1ColorSelectButton::setDimension(int dim)
{
  if (m_d->dim == dim)
    return;
  m_d->dim = dim;
  updateButton();
}
