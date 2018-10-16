/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRIGGERPROCESSOR_H
#define VP1TRIGGERPROCESSOR_H

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @class   : VP1TriggerProcessor
 *
 * @brief   : Trigger Processor (Header) - trigger data processing methods;
 *                                       - VP1TriggerTool interface to VP1TriggerSystem
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/Logger.h"

//Other includes
#include "StoreGate/StoreGateSvc.h"
#include <QString>
#include <QList>
#include <QTreeWidgetItem>


namespace VP1Trig {
  class VP1TriggerSystem;
  
  class VP1TriggerProcessor : public virtual Logger
  {
  public:
    VP1TriggerProcessor();
    virtual ~VP1TriggerProcessor();
    
    //Basic System Setup ------------------------------------->
    void initialize();
    
    void loadData(StoreGateSvc* m_storeGate);
    
    void clearSystem();
    //--------------------------------------------------------<
    
    //User Access Methods ------------------------------------>
    bool getInitStatus();             //ret: m_initStatus
    bool getDaqStatus();              //ret: m_daqStatus
    char getDataFormat();             //ret: m_dataFormat
    QString getTrigLvl();             //ret: m_trigLvl
    int getMuonsInEvt();              //ret: muonsInEvt
    
    void setDataFormat(char ctnid);   //ctnid -> m_dataFormat
    void setTrigLvl(QString triglvl); //triglvl -> m_trigLvl
    
    //***** Data Export *****
    QList<QTreeWidgetItem *> getQTrigData(QString);           //ret: m_trigLvlDataSet
    //--------------------------------------------------------<
    
  private:
    VP1TriggerSystem* m_triggersystem;
    
    //Basic System Setup Variables --------------------------->
    bool m_initStatus;
    bool m_daqStatus;
    QString m_trigLvl; //L1, L2, EF or ALL
    char m_dataFormat; //[v]ector, [q]tree, [a]ll available
    //--------------------------------------------------------<
    
    //Internal Processing ------------------------------------>
    void processData();
    
    //*************** TRIGGER DATA ***************
    QTreeWidgetItem * L1_QData, * L2_QData, * EF_QData;
    QList<QTreeWidgetItem *> m_trigLvlDataSet;
    //--------------------------------------------------------<
  };
}
#endif
