/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2Calo.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
// 
// AUTHOR:   M.P. Casado
//           S.R. Armstrong
// 
// - Add variables for job option controlled region limits, set defaults
//   to most likely values.
// - Add function EtaPhiRange to return the maximum and minimum eta or phi
//   values to use when calculating energy sums over a region  - R. Soluk
// ********************************************************************
 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"                               

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

#include "TrigT2CaloCommon/T2CaloNewBase.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"

class ISvcLocator;


T2CaloNewBase::T2CaloNewBase(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator){
  declareProperty("ParamSetFile", m_paramSet);
  declareProperty("EtaWidth", m_etaWidth = 0.1);
  declareProperty("PhiWidth", m_phiWidth = 0.1);  
  declareProperty("T2CaloTools",m_emToolName);
  declareProperty("TimerNtuple",m_timerString);
  declareProperty("TriggerOn",m_triggerType);
}


T2CaloNewBase::~T2CaloNewBase(){
}


StatusCode T2CaloNewBase::initialize(){

  MsgStream log(msgSvc(), name());

  //  if ( msgLvl() <= MSG::INFO ) 
  log << MSG::INFO << "on initialize()" << endreq;
  
  StatusCode stat = HLT::FexAlgo::initialize();
  if (stat.isFailure()) {
    log << MSG::ERROR << "base class finalize failed!" << endreq;
    return stat;
  }

  m_emAlgTools.reserve(MaxNumberTools);
  log << MSG::DEBUG << "Initialization set of tools" << endreq;
     
  std::vector<std::string>::iterator it = m_emToolName.begin();
 
  IToolSvc* toolSvc;
  service("ToolSvc",toolSvc);

  for (int iC = 0; it != m_emToolName.end(); ++iC, ++it) {
    if (iC >=  MaxNumberTools) {
      log << MSG::FATAL << "More T2Calo Tools than expected" << endreq;
      return StatusCode::FAILURE;
    }

    std::string toolName(*it);
    IAlgTool* bTool;
    IAlgToolCalo* pTool;

    if(toolSvc->retrieveTool(toolName, bTool, this).isFailure() ) {
      log << MSG::FATAL << "Unable to create " << *it
          << "AlgTool" << endreq;
      return StatusCode::FAILURE;
    } else {
      pTool = dynamic_cast<IAlgToolCalo*> (bTool);

#ifndef NDEBUG
	log << MSG::DEBUG << "Created " << *it << " AlgTool" << endreq;
#endif
    }

    // Save all AlgTool pointers in m_emAlgTools.
    
    m_emAlgTools.push_back(pTool);
  }

  // Initialize timing service
  
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    log << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" << endreq;
  } 

  if (m_timersvc) {
    
    m_timer[0] = m_timersvc->addItem(m_timerString);
    m_timer[1] = m_timersvc->addItem("onlyExecs");
    
  }
 
#ifndef NDEBUG
  log << MSG::INFO 
      << "Initialization completed successfully" 
      << endreq;
#endif
  
  return StatusCode::SUCCESS;
}


StatusCode T2CaloNewBase::finalize(){

  MsgStream log(msgSvc(), name());

#ifndef NDEBUG
    log << MSG::INFO << "in finalize()" << endreq;
#endif  

    StatusCode stat = HLT::FexAlgo::finalize();
    if (stat.isFailure()) {
      log << MSG::ERROR << "base class finalize failed!" << endreq;
      return stat;
    }

    return StatusCode::SUCCESS;
}
