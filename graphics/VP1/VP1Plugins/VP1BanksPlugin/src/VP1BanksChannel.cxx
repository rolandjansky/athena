/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1BanksChannel                //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: May 2007                              //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1BanksPlugin/VP1BanksChannel.h"
#include "ui_bankschannelwidgetform.h"
#include "VP1BanksSystems/VP1BanksSystem.h"
#include <QLabel>

//_________________________________________________________
VP1BanksChannel::VP1BanksChannel()
  : IVP1ChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1BanksChannel,"Banks"),
              "This channel simply shows the banks list, provided by the VP1BanksSystem.",
              "Thomas.Kittelmann@cern.ch"),
    m_tableWidget(0)
{
}

//_________________________________________________________
void VP1BanksChannel::init()
{
  m_bankssystem = new VP1BanksSystem();
  registerSystem(m_bankssystem);

  connect(m_bankssystem,SIGNAL(entriesChanged(const QStringList&,const QStringList&,const QStringList&)),
      this,       SLOT(  entriesChanged(const QStringList&,const QStringList&,const QStringList&)) );

}

//_________________________________________________________
void VP1BanksChannel::create() {

  //Setup this widget
  Ui::BanksChannelWidgetForm ui;
  ui.setupUi(this);
  m_tableWidget = ui.tableWidget;
  m_tableWidget->setColumnCount(2);
  m_tableWidget->setHorizontalHeaderLabels((QStringList()<<"Type"<<"Key"));
  m_tableWidget->setAlternatingRowColors ( true );
  m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  registerController(m_bankssystem->controllerWidget());
}

//_________________________________________________________
void VP1BanksChannel::entriesChanged( const QStringList& entry_key,const QStringList& entry_type,const QStringList& entry_id )
{

  if (entry_key.count()!=entry_type.count()||entry_key.count()!=entry_id.count()) {
    message("Error: Received data has wrong format!");
    m_tableWidget->setRowCount(0);
    return;
  }

  m_tableWidget->setUpdatesEnabled(false);

  //Turn sorting off while inserting (otherwise the correct row index to use will likely change in the middle of insertions):
  m_tableWidget->setSortingEnabled(false);

  m_tableWidget->setRowCount(entry_key.count());
  for (int irow = 0; irow<entry_key.count();++irow) {
    QTableWidgetItem * item0 = new QTableWidgetItem(entry_type.at(irow));
    QTableWidgetItem * item1 = new QTableWidgetItem(entry_key.at(irow));
    item0->setToolTip("Class ID "+entry_id.at(irow));
    m_tableWidget->setItem(irow, 0, item0);
    m_tableWidget->setItem(irow, 1, item1);
  }

  m_tableWidget->setSortingEnabled(true);
  m_tableWidget->resizeColumnsToContents();
  m_tableWidget->setUpdatesEnabled(true);
}
