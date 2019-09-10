/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  Source file for class VP1SelectEvent                                //
//                                                                      //
//  Description: Dialog for the selection of a specific event in a file //
//                                                                      //
//  Author: Sebastian Andreas Merkt (sebastian.andreas.merkt@cern.ch)   //
//  Initial version: August 2018                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1SelectEvent.h"

#include <QPushButton>
#include <QGridLayout>

#include <QtWidgets>
#include <QString>

VP1SelectEvent::VP1SelectEvent(int totEvtNr, int evtNr, QWidget *parent) : QDialog(parent)
{
  //Set default dialog size
  int nWidth = 400;
  int nHeight = 200;
  if (parent != NULL)
    setGeometry(parent->x() + parent->width()/2 - nWidth/2,
        parent->y() + parent->height()/2 - nHeight/2,
        nWidth, nHeight);
  else
    resize(nWidth, nHeight);

  //Buttonbox to set Open, Cancel buttons
  m_buttonBox = new QDialogButtonBox(this);
  m_openButton = m_buttonBox->addButton(tr("&Select"), QDialogButtonBox::AcceptRole);
  m_cancelButton = m_buttonBox->addButton(tr("&Cancel"),QDialogButtonBox::RejectRole);
  connect(m_openButton, &QPushButton::clicked, this, &VP1SelectEvent::passEvtNr);
  connect(m_cancelButton, &QPushButton::clicked, this, &VP1SelectEvent::reject);


  m_spinBox = new QSpinBox();
  m_spinBox->setRange(1, totEvtNr);
  m_spinBox->setSingleStep(1);
  m_spinBox->setValue(evtNr+1);

  const QString line2 = QString::number(totEvtNr) + " events in file.";

  //Create the main layout
  QGridLayout *mainLayout = new QGridLayout(this);
  mainLayout->addWidget(new QLabel(tr("Select event:")), 0, 0);
  mainLayout->addWidget(m_spinBox, 0, 1);
  mainLayout->addWidget(new QLabel(line2), 1, 0);
  mainLayout->addWidget(m_buttonBox, 2, 1);

}

void VP1SelectEvent::passEvtNr(){
  if( m_spinBox->value() != 0 ) {
    VP1SelectEvent::done(m_spinBox->value());
  }
}

void VP1SelectEvent::reject(){
  VP1SelectEvent::done(-1);
}
