/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1TriggerDecisionChannel      //
//  Weiyi Zhang  <wyizhang@cern.ch>  			   //
//  June 18 2007                             		   //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1TriggerDecisionPlugin/VP1TriggerDecisionChannel.h"
#include "ui_trigdecchannelwidgetform.h"
#include "VP1TriggerDecisionSystems/VP1TriggerDecisionSystem.h"
#include <QLabel>

//_________________________________________________________
VP1TriggerDecisionChannel::VP1TriggerDecisionChannel()
  	: IVP1ChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1TriggerDecisionChannel,"TriggerDecision"),
		      "This channel simply shows the trigger decision, provided by the VP1TrigDecSystem.",
		      "wyizhang@cern.ch"),
    	tableWidget(0)
{
}

//_________________________________________________________
void VP1TriggerDecisionChannel::init()
{
  trigdecsystem = new VP1TriggerDecisionSystem();
  registerSystem(trigdecsystem);

  connect(trigdecsystem,SIGNAL(entriesChanged(const QStringList&,const QStringList&)),
	  this, SLOT(  entriesChanged(const QStringList&,const QStringList&)) );
}

//_________________________________________________________
void VP1TriggerDecisionChannel::create()
{

  	//Setup this widget
  	Ui::TrigDecChannelWidgetForm ui;
  	ui.setupUi(this);
  	tableWidget=ui.tableWidget;
  	tableWidget->setColumnCount(2);
  	tableWidget->setHorizontalHeaderLabels((QStringList()<<"Signature"<<"Passed?"));
  	tableWidget->setAlternatingRowColors ( true );
  	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

  	connect(tableWidget,SIGNAL(cellActivated(int,int)),this,SLOT(cellActivated(int,int)));
}

//_________________________________________________________
void VP1TriggerDecisionChannel::entriesChanged( const QStringList& entry_key,const QStringList& entry_type )
{
  	clearAll();

  	if (entry_key.count()!=entry_type.count())
	{
    		message("Error: Received data has wrong format!");
    		return;
  	}

  	tableWidget->setUpdatesEnabled(false);

  	//Turn sorting off while inserting (otherwise the correct row index to use will likely change in the middle of insertions):
  	tableWidget->setSortingEnabled(false);

  	for (int irow = 0; irow<entry_key.count();++irow)
	{
    		tableWidget->insertRow ( irow );
    		tableWidget->setItem(irow, 0, new QTableWidgetItem(entry_key.at(irow)));
    		tableWidget->setItem(irow, 1, new QTableWidgetItem(entry_type.at(irow)));
  	}

  	tableWidget->setSortingEnabled(true);
  	tableWidget->resizeColumnsToContents();
  	tableWidget->resizeRowsToContents();
  	tableWidget->setUpdatesEnabled(true);
}

//_________________________________________________________
void VP1TriggerDecisionChannel::systemErased(IVP1System*)
{
  	clearAll();
}

//_________________________________________________________
void VP1TriggerDecisionChannel::systemRefreshed(IVP1System*)
{

}

//_________________________________________________________
void VP1TriggerDecisionChannel::clearAll()
{
  	tableWidget->setUpdatesEnabled(false);
  	tableWidget->setSortingEnabled(false);
  	tableWidget->clearContents();

  	while (tableWidget->rowCount()>0)
    		tableWidget->removeRow(tableWidget->rowCount()-1);
  	tableWidget->setUpdatesEnabled(true);

}

//_________________________________________________________
void VP1TriggerDecisionChannel::cellActivated( int row, int /*column*/ )
{
  	QString key = tableWidget->item(row,0)->text();
  	QString type = tableWidget->item(row,1)->text();

  	QStringList info = trigdecsystem->getInfoForEntry(key,type);

   	message("===== "+key+" ("+(type.isEmpty()? 0 :type)+"):");

	foreach(QString line,info)
     		message(line);

}
