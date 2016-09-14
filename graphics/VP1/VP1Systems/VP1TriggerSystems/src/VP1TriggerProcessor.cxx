/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : VP1TriggerProcessor
 *
 * @brief   : Trigger Processor - trigger data processing methods;
 *                              - VP1TriggerTool interface to VP1TriggerSystem
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/VP1TriggerProcessor.h"
#include "VP1TriggerSystems/VP1TriggerSystem.h"

//Other includes
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <QTreeWidget>
#include <QTreeWidgetItem>

//Global VARs ------------------------->
int muonsInEvt=-1;
//-------------------------------------<


//Trigger Processor Constructor
//_____________________________________________________________________________________________
VP1Trig::VP1TriggerProcessor::VP1TriggerProcessor()
  : Logger("VP1TriggerProcessor"),
    m_triggersystem(nullptr),
    m_initStatus(false), 
    m_daqStatus(false),
    m_trigLvl("all"),
    m_dataFormat('v'),
    L1_QData{}, L2_QData{}, EF_QData{} {
     log_verbose("constructor");
    }


//Trigger Processor Destructor
//_____________________________________________________________________________________________
VP1Trig::VP1TriggerProcessor::~VP1TriggerProcessor()
{
  log_verbose("destructor");
  delete m_triggersystem;
}


//Initialize Trigger System
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerProcessor::initialize()
{
  log_info("Initializing VP1Trig::VP1TriggerSystem");
  m_triggersystem = new VP1TriggerSystem();
  m_initStatus=true;
}


//Load Trigger Data
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerProcessor::loadData(StoreGateSvc* m_storeGate)
{
  QString method="loadData(): ";
  log_info("Loading trigger data via VP1TriggerSystem");
  
  //--- Note: Filling trigger handle (L1, L2, EF) objects via the VP1TriggerSystem. ---
  if(m_initStatus) {
    m_daqStatus = m_triggersystem->loadTriggerHandles(m_storeGate, m_trigLvl);
    if(m_daqStatus)
      processData();
    else
      log_fatal(method+"system initialized but retrieving data failed!");
  }
  else
    log_error(method+"trigger system not initialized!");
}


//Process Trigger Data
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerProcessor::processData()
{
  QString method="processData(): ";
  log_info("Processing trigger data: start");
  
  //Status flags
  bool l1status=false,l2status=false,efstatus=false;
  
  //L2 and EF triggers from L1 handle processor ------------->
  QTreeWidgetItem * L1_triggers, * L2_triggers, * EF_triggers;
  L1_triggers = new QTreeWidgetItem(0);
  L2_triggers = new QTreeWidgetItem(0);
  EF_triggers = new QTreeWidgetItem(0);
  L1_triggers->setText(0, QString("Triggers"));
  L2_triggers->setText(0, QString("Triggers"));
  EF_triggers->setText(0, QString("Triggers"));
  //---------------------------------------------------------<
  
  //--- Note: Handles for L1, L2, EF contain trigger data previously stored in each ---
  //---       object's containers by the VP1TriggerSystem via loadData() and are    ---
  //---       being modified in each "Fill" section below by calling each handles'  ---
  //---       processing methods, which fill m_trigData with processed data.        ---
  
  //L1 PROCESSING ------------------------------------------------------------------------------>
  if(m_trigLvl.compare("ALL", Qt::CaseInsensitive)==0 || m_trigLvl.compare("L1", Qt::CaseInsensitive)==0) {
    std::vector<VP1Trig::VP1TriggerHandleL1> handleL1 = m_triggersystem->getHandleL1();
    std::vector<VP1Trig::VP1TriggerHandleL1>::iterator l1it;
    
    if(handleL1.size()>0) {
      log_info(pfx1()+"processing L1 data: start");
      
      if(m_dataFormat=='q') {
	log_verbose("processing sector for QTree format");
	//************************* INIT *************************//
	L1_QData = new QTreeWidgetItem(0);
	L1_QData->setText(0, QString("L1 Data"));
	
	//************************* FILL *************************//
	for(l1it=handleL1.begin(); l1it!=handleL1.end(); ++l1it) {
	  if(l1it->processQTrigItem()) {
	    QString trigID = l1it->m_trigID;
	    if(trigID.contains("L2_"))
	      L2_triggers->addChild(l1it->m_qtrigitem);
	    else if(trigID.contains("EF_"))
              EF_triggers->addChild(l1it->m_qtrigitem);
	    else
              L1_triggers->addChild(l1it->m_qtrigitem);
	  }
	}
	L1_QData->addChild(L1_triggers);
	
	//************************* DIST *************************//
	m_trigLvlDataSet << L1_QData;
	
	//General processing status update
	if(int(L1_QData->childCount())>0)
	  l1status=true;
	else
	  log_warning("L1 QTree has no children");
	
      }//END: [q] processing
      else if(m_dataFormat=='v') {
	log_verbose("processing sector for vector format");
	//************************* INIT *************************//
	int p_success=0, p_failed=0;
	
	//************************* FILL *************************//
	for(l1it=handleL1.begin(); l1it!=handleL1.end(); ++l1it) {
	  if(l1it->processVTrigItem())
	    ++p_success;
	  else
	    ++p_failed;
	} 
	
	//General processing status update
        if(p_success>0)
          l1status=true;
	if(p_failed>0)
	  log_warning("L1 data item(s) failed processing: "+VP1String::str(p_failed));
      }//END: [v] processing
      else
	log_error(method+"data container format invalid!");
      log_info(pfx1()+"processing L1 data: complete");
    }//END: size>0
    else
      log_warning(method+"L1 data requested but not available");
  }//END: L1
  //--------------------------------------------------------------------------------------------<
  
  
  //L2 PROCESSING ------------------------------------------------------------------------------>
  if(m_trigLvl.compare("ALL", Qt::CaseInsensitive)==0 || m_trigLvl.compare("L2", Qt::CaseInsensitive)==0) {
    std::vector<VP1Trig::VP1TriggerHandleL2> handleL2 = m_triggersystem->getHandleL2();
    std::vector<VP1Trig::VP1TriggerHandleL2>::iterator l2it = handleL2.begin();
    
    if(handleL2.size()>0) {
      log_info(pfx1()+"processing L2 data: start");
      if(m_dataFormat=='q') {
	log_verbose("processing sector for QTree format");
	//************************* FILL *************************//	
	if(l2it->processQTrigItem())
	  L2_QData = l2it->m_qtrigData;
	else
	  L2_QData = new QTreeWidgetItem(0);
	L2_QData->setText(0, QString("L2 Data"));

	if(int(L2_triggers->childCount())>0)
          L2_QData->insertChild(0,L2_triggers); //from L1 processor
	
	//************************* DIST *************************//
	m_trigLvlDataSet << L2_QData;
	
	//General processing status update
        if(int(L2_QData->childCount())>0)
          l2status=true;
        else
          log_warning("L2 QTree has no children");
      }//END: [q] processing
      else if(m_dataFormat=='v') {
	log_verbose("processing sector for vector format");
	//************************* FILL *************************//
	l2it->processVTrigItem();
	
	//General processing status update
	l2status=true;
      }//END: [v] processing
      else
	log_error(method+"data container format invalid!");
      log_info(pfx1()+"processing L2 data: complete");
    }//END: size>0
    else
      log_warning(method+"L2 data requested but not available");
  }//END: L2
  //--------------------------------------------------------------------------------------------<
  
  
  //EF PROCESSING ------------------------------------------------------------------------------>
  if(m_trigLvl.compare("ALL", Qt::CaseInsensitive)==0 || m_trigLvl.compare("EF", Qt::CaseInsensitive)==0) {
    std::vector<VP1Trig::VP1TriggerHandleEF> handleEF = m_triggersystem->getHandleEF();
    std::vector<VP1Trig::VP1TriggerHandleEF>::iterator efit;
    
    //Muons in event
    muonsInEvt = int(handleEF.size());
    
    if(handleEF.size()>0) {
      log_info(pfx1()+"processing EF data: start");
      if(m_dataFormat=='q') {
	      log_verbose("processing sector for QTree format");
        //************************* INIT *************************//
        EF_QData = new QTreeWidgetItem(0);
        EF_QData->setText(0, QString("EF Data"));
	
        //************************* FILL *************************//
        if(int(EF_triggers->childCount())>0)
	  EF_QData->addChild(EF_triggers); //from L1 processor
	for(efit=handleEF.begin(); efit!=handleEF.end(); ++efit) {
	  if(efit->processQTrigItem())
	    EF_QData->addChild(efit->m_qtrigData);
	}
	
        //************************* DIST *************************//
        m_trigLvlDataSet << EF_QData;

	//General processing status update
        if(int(EF_QData->childCount())>0)
          efstatus=true;
        else
          log_warning("EF QTree has no children");
      }//END: [q] processing
      else if(m_dataFormat=='v') {
	log_verbose("processing sector for vector format");
	//************************* INIT *************************//
        int p_success=0, p_failed=0;
	
        //************************* FILL *************************//
        for(efit=handleEF.begin(); efit!=handleEF.end(); ++efit) {
          if(efit->processVTrigItem())
            ++p_success;
          else
            ++p_failed;
        }
	
	//General processing status update
        if(p_success>0)
          efstatus=true;
        if(p_failed>0)
          log_warning("EF data item(s) failed processing: "+VP1String::str(p_failed));
      }//END: [v] processing
      else
	log_error(method+"data container format invalid!");
      log_info(pfx1()+"processing EF data: complete");
    }//END: size>0
    else
      log_warning(method+"EF data requested but not available");
  }//END: EF
  //--------------------------------------------------------------------------------------------<
  
  //Update DAQ Status flag ~~~~~~~~~~~~~~~
  if(m_daqStatus) {
    if(l1status || l2status || efstatus)
      m_daqStatus=true;
    else
      m_daqStatus=false;
  }//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  log_info("Processing trigger data: complete");
}//END: processData


//Clear System (at erase)
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerProcessor::clearSystem()
{
  log_info("clearSystem()");
  
  m_triggersystem->clearTriggerHandles();
  
  m_trigLvlDataSet.clear();
  
  L1_QData->takeChildren();
  L2_QData->takeChildren();
  EF_QData->takeChildren();
}//END: clearSystem


//#############################################################################################
// PUBLIC USER ACCESS METHODS [GETTER]
//#############################################################################################


//Return Status of Initalization
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerProcessor::getInitStatus()
{
  log_verbose("User request: return status of initalization");
  return m_initStatus;
}


//Return DAQ Status
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerProcessor::getDaqStatus()
{
  log_verbose("User request: return daq status");
  return m_daqStatus;
}


//Return Data Container Export Format
//_____________________________________________________________________________________________
char VP1Trig::VP1TriggerProcessor::getDataFormat()
{
  log_verbose("User request: return data container export type");
  return m_dataFormat;
}


//Return Trigger Level flag to Process
//_____________________________________________________________________________________________
QString VP1Trig::VP1TriggerProcessor::getTrigLvl()
{
  log_verbose("User request: return trigger level flag to process");
  return m_trigLvl;
}


//Return Number of Muons in Event (requires EF trigLvl)
//_____________________________________________________________________________________________
int VP1Trig::VP1TriggerProcessor::getMuonsInEvt()
{
  log_verbose("User request: return number of muons in event");
  return muonsInEvt;
}


//Return collected Trigger Data in QTree container format
//_____________________________________________________________________________________________
QList<QTreeWidgetItem *> VP1Trig::VP1TriggerProcessor::getQTrigData(QString triglvl)
{
  log_verbose("User request: return collected trigger data in QTree container format");
  QList<QTreeWidgetItem *> customDataSet;
  
  if(m_daqStatus) {
    if(triglvl.compare("ALL", Qt::CaseInsensitive)==0 || triglvl.compare("L1", Qt::CaseInsensitive)==0 || triglvl.compare("L2", Qt::CaseInsensitive)==0 || triglvl.compare("EF", Qt::CaseInsensitive)==0) {
      if(m_trigLvl.compare(triglvl, Qt::CaseInsensitive)==0)
	return m_trigLvlDataSet;
      else if(m_trigLvl.compare("ALL", Qt::CaseInsensitive)==0) {
	if(triglvl.compare("ALL", Qt::CaseInsensitive)==0)
	  return m_trigLvlDataSet;
	else if(triglvl.compare("L1", Qt::CaseInsensitive)==0) {
	  customDataSet << L1_QData;
	  return customDataSet;
	}
	else if(triglvl.compare("L2", Qt::CaseInsensitive)==0) {
	  customDataSet << L2_QData;
	  return customDataSet;
	}
	else if(triglvl.compare("EF", Qt::CaseInsensitive)==0) {
	  customDataSet << EF_QData;
	  return customDataSet;
	}
      }//all available
    }//available trigger levels
  }//daq status ok
  else {
    log_fatal("User data request: DAQ not available!");
    return customDataSet;
  }
  
  log_error("User requested ["+triglvl+"] trigger data which is not available! Only ["+m_trigLvl+"] available.");
  return m_trigLvlDataSet;
}


//#############################################################################################
// PUBLIC USER ACCESS METHODS [SETTER]
//#############################################################################################


//Set Trigger Level flag to Process
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerProcessor::setTrigLvl(QString triglvl)
{
  if(triglvl.compare("L1", Qt::CaseInsensitive)==0 || triglvl.compare("L2", Qt::CaseInsensitive)==0 || triglvl.compare("EF", Qt::CaseInsensitive)==0 || triglvl.compare("ALL", Qt::CaseInsensitive)==0) {
    log_verbose("User request: set trigger level flag to "+triglvl);
    m_trigLvl = triglvl;
  }
  else
    log_error("setTrigLvl(): ["+triglvl+"] as trigger level flag not available!");
}


//Set Data Container Format (> dataFormat)
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerProcessor::setDataFormat(char ctnid)
{
  if(ctnid=='v' || ctnid=='q' || ctnid=='a') { //[v]ector, [q]tree, [a]ll available
    log_verbose("User request: set data container format to "+qstr(ctnid));
    m_dataFormat = ctnid;
  }
  else
    log_error("setDataFormat(): ["+qstr(ctnid)+"] as data container format not available!");
}
