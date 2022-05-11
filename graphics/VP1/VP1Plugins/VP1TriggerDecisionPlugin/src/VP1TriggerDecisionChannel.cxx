/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1TriggerDecisionChannel      //
//  Weiyi Zhang  <wyizhang@cern.ch>                        //
//  June 18 2007                                           //
//
//  Major Updates:
//  - 2022/03, Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>
//                                                         
/////////////////////////////////////////////////////////////

#include "VP1TriggerDecisionPlugin/VP1TriggerDecisionChannel.h"
#include "ui_trigdecchannelwidgetform.h"
#include "VP1TriggerDecisionSystems/VP1TriggerDecisionSystem.h"
#include <QLabel>

//_________________________________________________________
VP1TriggerDecisionChannel::VP1TriggerDecisionChannel()
    : IVP1ChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1TriggerDecisionChannel,"TriggerDecision"),
              "This channel simply shows the trigger decision, provided by the VP1TrigDecSystem.",
              "wyizhang@cern.ch, Riccardo.Maria.Bianchi@cern.ch"),
        m_tableWidget(0)
{
}

VP1TriggerDecisionChannel::~VP1TriggerDecisionChannel()
{
}

//_________________________________________________________
void VP1TriggerDecisionChannel::init()
{
  m_trigdecsystem.reset (new VP1TriggerDecisionSystem());
  registerSystem(m_trigdecsystem.get());

  connect(m_trigdecsystem.get(),SIGNAL(entriesChanged(const QStringList&,const QStringList&)),
	  this, SLOT(  entriesChanged(const QStringList&,const QStringList&)) );
}

//_________________________________________________________
void VP1TriggerDecisionChannel::create()
{

    //Setup this widget
    Ui::TrigDecChannelWidgetForm ui;
    ui.setupUi(this);
    m_tableWidget=ui.tableWidget;
    m_tableWidget->setColumnCount(2);
    m_tableWidget->setHorizontalHeaderLabels((QStringList()<<"Signature"<<"Passed?"));
    m_tableWidget->setAlternatingRowColors ( true );
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_tableWidget,SIGNAL(cellActivated(int,int)),this,SLOT(cellActivated(int,int)));
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

    m_tableWidget->setUpdatesEnabled(false);

    //Turn sorting off while inserting (otherwise the correct row index to use will likely change in the middle of insertions):
    m_tableWidget->setSortingEnabled(false);

    for (int irow = 0; irow<entry_key.count();++irow)
    {
            m_tableWidget->insertRow ( irow );
            m_tableWidget->setItem(irow, 0, new QTableWidgetItem(entry_key.at(irow)));
            m_tableWidget->setItem(irow, 1, new QTableWidgetItem(entry_type.at(irow)));
    }

    m_tableWidget->setSortingEnabled(true);
    m_tableWidget->resizeColumnsToContents();
    m_tableWidget->resizeRowsToContents();
    m_tableWidget->setUpdatesEnabled(true);
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
    m_tableWidget->setUpdatesEnabled(false);
    m_tableWidget->setSortingEnabled(false);
    m_tableWidget->clearContents();

    while (m_tableWidget->rowCount()>0)
            m_tableWidget->removeRow(m_tableWidget->rowCount()-1);
    m_tableWidget->setUpdatesEnabled(true);

}

//_________________________________________________________
void VP1TriggerDecisionChannel::cellActivated( int row, int /*column*/ )
{
    QString key = m_tableWidget->item(row,0)->text();
    QString type = m_tableWidget->item(row,1)->text();

    QStringList info = m_trigdecsystem->getInfoForEntry(key,type);

    message("===== "+key+" ("+(type.isEmpty()? 0 :type)+"):");

    foreach(QString line,info)
            message(line);

}
