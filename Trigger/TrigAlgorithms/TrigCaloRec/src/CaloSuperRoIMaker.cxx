/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CaloSuperRoIMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   Ademar Delgado, tavares@cern.ch
//
// Description: Receive all TE of the event and store them in a superRoi.
//              Important: To be used in any Partial Scan before the 
//           TrigCaloCellMaker
// Updates:
// 
// ********************************************************************

#include "TrigCaloRec/CaloSuperRoIMaker.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

//--------------------------------------------------------------
CaloSuperRoIMaker::CaloSuperRoIMaker(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_doFS(false),
    m_RoISize(0.5),
    m_superRoIOutputKey(""),
    m_superRoIUniqueKey(""),
    m_log(0),
    m_superRoi(0),
    m_fillSuperRoI_timer(0),
    m_settingOutput_timer(0),
    m_fillSuperRoITime(0.),
    m_settingOutputTime(0.),
    m_TEnumb(0),
    m_eta(0.),
    m_phi(0.)
{
  // unpacking tool
  // configurables
  declareProperty("superRoIOutputKey",   m_superRoIOutputKey = "CaloSuperRoI");
  declareProperty("doFullScan",    m_doFS = "false");
  declareProperty("RoISize",       m_RoISize = 0.5);
   
  // monitoring  
  declareMonitoredVariable("FillSuperRoITime",   m_fillSuperRoITime);  
  declareMonitoredVariable("SettingOutputTime",  m_settingOutputTime);  
  declareMonitoredVariable("TE_Event",           m_TEnumb);
  
  /// use msgsvc
  //  std::printf("#######################   CaloSuperRoIMaker - tmpAlg - v-0.0.0   #######################\n");
}

//--------------------------------------------------------------
CaloSuperRoIMaker::~CaloSuperRoIMaker(){ }

//--------------------------------------------------------------
HLT::ErrorCode CaloSuperRoIMaker::hltInitialize(){
  m_log = new MsgStream(msgSvc(), name());
  MsgStream &logStream = *m_log;
  
  if(logStream.level() <= MSG::INFO)
    logStream << MSG::INFO << " Initalizing FEX algorithm: " << name() << endreq;
  
  initializeTimers();  

  //  std::printf("T2AllRoiCellUnpacking::hltInitialize - RoISize - %f \n", m_RoISize);
  //  std::printf("#######################   T2AllRoiCellUnpacking::hltInitialize   #######################\n");
  return HLT::OK;
}

//--------------------------------------------------------------
HLT::ErrorCode CaloSuperRoIMaker::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out){
  MsgStream &logStream = *m_log;
  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  beforeExecMonitors().ignore();
  // setup what we need (variables and tools)
  resetTimers();
  
  //--------------------- begin TE check and prepare superRoi ---------------------
  if(m_settingOutput_timer) m_settingOutput_timer->start();  
  
  if(!inputIsValid(tes_in)) return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
  // if the input is valid, take _the_ last TEVec and process it
  const HLT::TEVec &tev = tes_in.back();
  HLT::TriggerElement* outputTE(setupOutputTe(type_out));
  
  m_superRoi = new TrigSuperRoi(m_doFS); //if m_doFS true, do SuperRoi fullscan
  m_superRoi->clear();
  
  if ((getUniqueKey( m_superRoi, m_superRoIUniqueKey, m_superRoIOutputKey )) != HLT::OK) {
    (*m_log) << MSG::DEBUG << "Unable to retrieve the superRoI unique key" << endreq;
  }
  if (evtStore()->record(m_superRoi, m_superRoIUniqueKey).isFailure()) {
    (*m_log) << MSG::DEBUG << "Unable to record the superRoI with the key - " << m_superRoIUniqueKey << endreq;
  }
  
  if(m_settingOutput_timer) m_settingOutput_timer->pause();  
  //--------------------- finish TE check and prepare superRoi ---------------------
  

  //--------------------- begin superRoI fill ---------------------
  if(m_fillSuperRoI_timer) m_fillSuperRoI_timer->start();  
  
  for(unsigned int iTe=0; iTe<tev.size(); iTe++){
    const HLT::TriggerElement* inputTe = tev[iTe];
    
    if(!inputTe){
      logStream<<MSG::ERROR<<"Invalid TriggerElement ("<<inputTe<<") pointer"<<endreq;
      return HLT::ERROR;
    } // end if(!inputTe)
    
    // get the RoiDescriptor
    const TrigRoiDescriptor* roiDescriptor = 0;
    HLT::ErrorCode hltStatus = getFeature(inputTe, roiDescriptor);
    if(HLT::OK != hltStatus){
      logStream<<MSG::ERROR<<" Failed to find RoiDescriptor "<<endreq;
      return hltStatus;
    } // end if(not OK)*/
    
    m_eta = roiDescriptor->eta();
    m_phi = roiDescriptor->phi();
    TrigRoiDescriptor* tmpRoiDescriptor = new TrigRoiDescriptor( m_eta, m_eta-m_RoISize, m_eta+m_RoISize, m_phi, HLT::wrapPhi(m_phi-m_RoISize), HLT::wrapPhi(m_phi+m_RoISize) ); 
    m_superRoi->push_back(tmpRoiDescriptor);
  } // end for(iTe)
  
  if(m_fillSuperRoI_timer) m_fillSuperRoI_timer->stop();
  //--------------------- finish superRoI fill ---------------------
  
  m_TEnumb=tev.size();
  
  //--------------------- begin TE finalize the output ---------------------
  if(m_settingOutput_timer) m_settingOutput_timer->resume();  
  // finalize the output and the monitoring variables
  HLT::ErrorCode stat(finalizeOutput(outputTE));
  if(stat!= HLT::OK) return stat;
  finalizeAccessories();
  if(m_settingOutput_timer) m_settingOutput_timer->stop();  
  //--------------------- finish TE finalize the output ---------------------
  
  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();
  logStream<<MSG::DEBUG<<"Processed "<<tev.size()<<" TriggerElements."<<endreq;
  
  //std::printf("Test - m_doPFS=%d tev.size()= %u m_superRoi.size()=%d  m_cellCont.size()=%lu \n",m_doPFS,tev.size(),m_superRoi.size(),m_cellCont->size());
  //std::printf("#######################   CaloSuperRoIMaker::hltExecute  #######################\n");
  
  return HLT::OK;
}

//--------------------------------------------------------------
HLT::ErrorCode CaloSuperRoIMaker::hltFinalize(){
  MsgStream &logStream = *m_log;
  if ( logStream.level() <= MSG::DEBUG )
    logStream << MSG::DEBUG << "Finalizing CaloSuperRoIMaker FEX " << name() << endreq;
  
  delete m_log;	
  //std::printf("#######################   CaloSuperRoIMaker::hltFinalize   #######################\n");
  return HLT::OK;	
}

//!____________________ core functions _____________________


//!__________________ auxiliary functions __________________
//----------------------------------------------------------
bool CaloSuperRoIMaker::initializeTimers(){
  MsgStream &logStream = *m_log;
  ITrigTimerSvc *pTimerService=0;
  if( service( "TrigTimerSvc", pTimerService).isFailure() || !pTimerService){
    logStream<<MSG::ERROR <<name()<<": Unable to locate TrigTimer Service" <<endreq;
    return false;
  }
  logStream << MSG::DEBUG << " Adding timers" << endreq;
  //Add timers
  m_fillSuperRoI_timer   = addTimer("fillSuperRoI_timer");
  m_settingOutput_timer  = addTimer("settingOutput_timer");
  
  //std::printf("#######################   CaloSuperRoIMaker::initializeTimers   #######################\n");
  return true;
}

//--------------------------------------------------------------
void CaloSuperRoIMaker::resetTimers(){
  if(m_fillSuperRoI_timer)  m_fillSuperRoI_timer->reset();
  if(m_settingOutput_timer) m_settingOutput_timer->reset();
  //std::printf("#######################   CaloSuperRoIMaker::resetTimers   #######################\n");
}

//--------------------------------------------------------------
void CaloSuperRoIMaker::storeTimers(){
  if(m_fillSuperRoI_timer)  m_fillSuperRoITime  = m_fillSuperRoI_timer->elapsed();
  if(m_settingOutput_timer) m_settingOutputTime = m_settingOutput_timer->elapsed();
  //std::printf("#######################   CaloSuperRoIMaker::storeTimers   #######################\n AllRoi m_RoITime=%f \n AllRoi m_UnpckTime=%f \n AllRoi m_cellUnpckTime=%f \n", m_RoITime, m_UnpckTime, m_cellUnpckTime);
}

//--------------------------------------------------------------
bool CaloSuperRoIMaker::inputIsValid(const std::vector<std::vector<HLT::TriggerElement*> >& input) const {
  MsgStream &logStream = *m_log;
  if(logStream.level() <= MSG::DEBUG)
    logStream<< MSG::DEBUG << "CaloSuperRoIMaker "<<name()<<"\n"
	     <<input.size()<<" input trigger elements for CaloSuperRoIMaker"<<endreq;
  // sanity check on the input
  if(input.size()==0){
    logStream<<MSG::ERROR<<"CaloSuperRoIMaker "<<name()<<"\n"
	     <<"Error! expected at least one vector<TriggerElement> as input,\n"
	     <<" that should contain the L1 jets. (got 0 vectors instead)\n"
	     <<"Exiting hltExecute."<<endreq;
    //std::printf("#######################   CaloSuperRoIMaker::inputIsValid - error  #######################\n");
    return false;
  } // end if(1!=input.size())
  
  //std::printf("#######################   CaloSuperRoIMaker::inputIsValid   #######################\n");
  return true;
}

//--------------------------------------------------------------
void CaloSuperRoIMaker::finalizeAccessories(){
  storeTimers();
}

//--------------------------------------------------------------
HLT::TriggerElement* CaloSuperRoIMaker::setupOutputTe(unsigned int type_out) {
  (*m_log) <<MSG::ERROR<<"CaloSuperRoIMaker::setupOutputTE  "<<name() <<" this class is obsolete now\n";
  //  HLT::TriggerElement* outputTE = addRoI(type_out,0.,0.); // note: addRoI is defined in AllTEAlgo base class
  //  outputTE->setActiveState(true);
  //std::printf("#######################   CaloSuperRoIMaker::setupOutputTe   #######################\n",type_out);
  HLT::TriggerElement* outputTE = 0;
  return outputTE;
}

//--------------------------------------------------------------
HLT::ErrorCode CaloSuperRoIMaker::finalizeOutput(HLT::TriggerElement* outputTE){
  MsgStream &logStream = *m_log;
  
  std::string m_superRoIUniqueKey="";
  HLT::ErrorCode stat = reAttachFeature(outputTE, m_superRoi, m_superRoIUniqueKey, m_superRoIOutputKey);
  //  HLT::ErrorCode stat = recordAndAttachFeature(outputTE, m_cellCont, tmp_cellCollUniqueKey, m_cellContOutputKey);
  //  HLT::ErrorCode stat = attachFeature(outputTE, m_cellCont, tmp_cellCollUniqueKey, m_cellContOutputKey,HLT::Navigation::ObjectToStoreGate);
  if (stat != HLT::OK){
    logStream<<MSG::ERROR<<" recording of SuperRoI into StoreGate failed"<<endreq;
    return stat;
  }
  
  if(logStream.level() <= MSG::DEBUG)
    logStream<<MSG::DEBUG<<" SuperRoIMaker done. "<<name()<<endreq;
  
  /// need to use msgsvc
  //  std::printf(" m_superRoIUniqueKey = %s \n m_superRoIOutputKey = %s \n",m_superRoIUniqueKey.c_str(), m_superRoIOutputKey.c_str());
  //  std::printf(" outputTE->getId() = %u \n inputTE->getActiveState() = %d //1 true; 0 false \n",outputTE->getId(), outputTE->getActiveState());
  //  std::printf("#######################   CaloSuperRoIMaker TESTE -m_superRoi.size()=%d \n",m_superRoi->size());
  //  std::printf("#######################   CaloSuperRoIMaker::finalizeOutput   #######################\n");
  
  return stat;
}

