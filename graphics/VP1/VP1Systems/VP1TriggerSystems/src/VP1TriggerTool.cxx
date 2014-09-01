/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : VP1TriggerTool
 *
 * @brief   : VP1 Trigger Tool - public access tool to retrieve trigger data
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/VP1TriggerProcessor.h"
#include "VP1TriggerSystems/VP1TriggerTool.h"

//Other includes
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>


//Trigger System Constructor
//_____________________________________________________________________________________________
VP1Trig::VP1TriggerTool::VP1TriggerTool()
  : Logger("VP1TriggerTool"),
    m_status(false)
{
  //Initialize VP1TriggerProcessor
  log_info("Initializing VP1Trig::VP1TriggerProcessor");
  m_triggerprocessor = new VP1Trig::VP1TriggerProcessor();
  m_triggerprocessor->initialize();
}


//Trigger System Destructor
//_____________________________________________________________________________________________
VP1Trig::VP1TriggerTool::~VP1TriggerTool()
{
  log_verbose("destructor");
  delete m_triggerprocessor;
}


//Load Trigger Data from StoreGate (after new event loaded)
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerTool::process(StoreGateSvc* m_storeGate)
{
  if(m_status)
    clear();
  
  log_info("Load trigger data from StoreGate");
  if(m_triggerprocessor->getInitStatus()) {
    m_triggerprocessor->loadData(m_storeGate);
    m_status=true;
    log_info("Successfully retrieved the VP1TriggerTool!");
  }
  else {
    m_status=false;
    log_error("VP1Trig::VP1TriggerProcessor not initialized!");
  }
}


//Clear Trigger System (before new event loaded)
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerTool::clear()
{
  log_verbose("Clear system-wide allocated memory");
  m_status=false;
  m_triggerprocessor->clearSystem();
}


//#############################################################################################
// USER INTERFACE TO TRIGGER DATA
//#############################################################################################


//Return System Status
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerTool::isReady()
{
  log_verbose("Requesting DAQ status from VP1Trig::VP1TriggerProcessor");
  if(m_status && m_triggerprocessor->getDaqStatus())
    return true;
  return false;
}


//Set trigger level flag to process its data
//--- Note:    has to be called before new event loaded, which means before system ---
//---          refresh(), e.g. at the particular channel init() or create().       ---
//--- Default: [ALL] trigger levels; set by constructor.                           ---
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerTool::setTrigLvlToProcess(QString triglvl)
{
  log_verbose("Presetting trigger level flag in VP1Trig::VP1TriggerProcessor");
  m_triggerprocessor->setTrigLvl(triglvl);
}


//Get trigger level flag to process its data
//_____________________________________________________________________________________________
QString VP1Trig::VP1TriggerTool::getTrigLvlToProcess()
{
  log_verbose("Requesting preset trigger level flag from VP1Trig::VP1TriggerProcessor");
  return m_triggerprocessor->getTrigLvl();
}


//Set data container format to store trigger data
//--- Note:    has to be called before new event loaded, which means before system ---
//---          refresh(), e.g. at the particular channel init() or create().       ---
//--- Default: [v]ector format; set by constructor.                                ---
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerTool::setDataContainerFormat(char ctnid)
{
  //Container IDs: [v]ector, [q]tree, [a]ll available
  log_verbose("Presetting data format in VP1Trig::VP1TriggerProcessor");
  m_triggerprocessor->setDataFormat(ctnid);
}


//Get data container format to store trigger data
//_____________________________________________________________________________________________
char VP1Trig::VP1TriggerTool::getDataContainerFormat()
{
  log_verbose("Requesting preset data format from VP1Trig::VP1TriggerProcessor");
  return m_triggerprocessor->getDataFormat();
}


//Get number of muons in event
//_____________________________________________________________________________________________
int VP1Trig::VP1TriggerTool::getMuonCount()
{
  log_verbose("Requesting number of muons from VP1Trig::VP1TriggerProcessor");
  return m_triggerprocessor->getMuonsInEvt();
}


//Get trigger data in QTree container format
//_____________________________________________________________________________________________
QList<QTreeWidgetItem *> VP1Trig::VP1TriggerTool::getTriggerData_QTree(QString triglvl)
{
  log_verbose("Requesting data in QTree format from VP1Trig::VP1TriggerProcessor");
  return m_triggerprocessor->getQTrigData(triglvl);
}
