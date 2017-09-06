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
    tableWidget(0)
{
}

//_________________________________________________________
void VP1BanksChannel::init()
{
  bankssystem = new VP1BanksSystem();
  registerSystem(bankssystem);

  connect(bankssystem,SIGNAL(entriesChanged(const QStringList&,const QStringList&,const QStringList&)),
	  this,       SLOT(  entriesChanged(const QStringList&,const QStringList&,const QStringList&)) );

}

//_________________________________________________________
void VP1BanksChannel::create() {

  //Setup this widget
  Ui::BanksChannelWidgetForm ui;
  ui.setupUi(this);
  tableWidget = ui.tableWidget;
  tableWidget->setColumnCount(2);
  tableWidget->setHorizontalHeaderLabels((QStringList()<<"Type"<<"Key"));
  tableWidget->setAlternatingRowColors ( true );
  tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  registerController(bankssystem->controllerWidget());
}

//_________________________________________________________
void VP1BanksChannel::entriesChanged( const QStringList& entry_key,const QStringList& entry_type,const QStringList& entry_id )
{

  if (entry_key.count()!=entry_type.count()||entry_key.count()!=entry_id.count()) {
    message("Error: Received data has wrong format!");
    tableWidget->setRowCount(0);
    return;
  }

  tableWidget->setUpdatesEnabled(false);

  //Turn sorting off while inserting (otherwise the correct row index to use will likely change in the middle of insertions):
  tableWidget->setSortingEnabled(false);

  tableWidget->setRowCount(entry_key.count());
  for (int irow = 0; irow<entry_key.count();++irow) {
    QTableWidgetItem * item0 = new QTableWidgetItem(entry_type.at(irow));
    QTableWidgetItem * item1 = new QTableWidgetItem(entry_key.at(irow));
    item0->setToolTip("Class ID "+entry_id.at(irow));
    tableWidget->setItem(irow, 0, item0);
    tableWidget->setItem(irow, 1, item1);
  }

  tableWidget->setSortingEnabled(true);
  tableWidget->resizeColumnsToContents();
  tableWidget->setUpdatesEnabled(true);
}
