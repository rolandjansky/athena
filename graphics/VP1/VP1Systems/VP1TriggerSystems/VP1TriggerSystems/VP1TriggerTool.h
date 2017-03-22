/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRIGGERTOOL_H
#define VP1TRIGGERTOOL_H

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @class   : VP1TriggerTool
 *
 * @brief   : VP1 Trigger Tool (Header) - public access tool to retrieve trigger data
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/Logger.h"

//Other includes
#include "StoreGate/StoreGateSvc.h"
#include <QString>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace VP1Trig {
  class VP1TriggerProcessor;
  
  class VP1TriggerTool : public virtual Logger
  {
  public:
    VP1TriggerTool();
    virtual ~VP1TriggerTool();
    //
    VP1TriggerTool & operator=(const VP1TriggerTool &) = delete;
    //
    VP1TriggerTool(const VP1TriggerTool &) = delete;
    //
    //External Interface ---------------------------->
    void process(StoreGateSvc* m_storeGate);
    void clear();
    //-----------------------------------------------<
    
    //User interface methods ------------------------>
    //+ return daq status
    bool isReady();
    
    //+ set/get trigger level to process
    void setTrigLvlToProcess(QString triglvl);
    QString getTrigLvlToProcess();
    
    //+ set/get data container format
    void setDataContainerFormat(char ctnid);
    char getDataContainerFormat();
    
    //+ get number of muons in event
    int getMuonCount();
    
    //+ get all trigger data in QTree container format 
    QList<QTreeWidgetItem *> getTriggerData_QTree(QString);
    //-----------------------------------------------<
    
  private:
    VP1TriggerProcessor * m_triggerprocessor;
    bool m_status;
  };
}
#endif
