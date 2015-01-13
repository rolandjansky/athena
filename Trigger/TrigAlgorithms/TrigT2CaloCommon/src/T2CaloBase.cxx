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

#include "TrigT2CaloCommon/T2CaloBase.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"


class ISvcLocator;


T2CaloBase::T2CaloBase(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator),  m_emAlgTools(this),
  m_data("TrigDataAccess/TrigDataAccess"),
  m_timersvc("TrigTimerSvc/TrigTimerSvc","T2CaloBase"){
  declareProperty("ParamSetFile", m_paramSet);
  declareProperty("EtaWidth", m_etaWidth = 0.1,"Eta Width of the Region of Interest");
  declareProperty("PhiWidth", m_phiWidth = 0.1,"Phi Width of the Region of Interest");  
  declareProperty("EtaWidthForID", m_etaWidthForID = 0.1,"Eta Width of the Region of Interest for ID");
  declareProperty("PhiWidthForID", m_phiWidthForID = 0.1,"Phi Width of the Region of Interest for ID");  
  // declareProperty("T2CaloTools",m_emToolName);
  declareProperty("TimerNtuple",m_timerString);
  declareProperty("TriggerOn",m_triggerType);
  declareProperty("IAlgToolList", m_emAlgTools,"list of AlgToolCalo. These tools perform FEX.");
  declareProperty("TrigTimerSvc", m_timersvc,"TrigTimerSvc Handle.");
  declareProperty("TrigDataAccess",m_data,"Data Access for LVL2 Calo Algorithms");
  declareProperty("roiEtaLimit",m_roiEtaLimit=2.5);  // See TriggerTowerMap
  declareProperty("FCalEtaMin",m_fcalEtaMin = 3);
  declareProperty("FCalEtaMax",m_fcalEtaMax = 5);
  declareProperty("USeCompositeRoi",m_useCompositeRoi = false);

  m_fcalEtaMin = 3.0;
  m_fcalEtaMax = 5.0;
  m_vec_robs.reserve(200);
}


T2CaloBase::~T2CaloBase(){
}


StatusCode T2CaloBase::initialize(){

  MsgStream log(msgSvc(), name());

  //  if ( msgLvl() <= MSG::INFO ) 
  log << MSG::INFO << "on initialize()" << endreq;
  
  StatusCode stat = HLT::FexAlgo::initialize();
  if (stat.isFailure()) {
    log << MSG::ERROR << "base class finalize failed!" << endreq;
    return stat;
  }

    if( m_emAlgTools.retrieve().isFailure() ) {
      log << MSG::FATAL << "Unable to create " << 
          "some AlgTool" << endreq;
      return StatusCode::FAILURE;
    } ;

    if ((m_data.retrieve()).isFailure()) {
      log << MSG::ERROR << "Could not get m_data" << endreq;
      return StatusCode::FAILURE;
    }


  // Initialize timing service
  
  if( (m_timersvc.retrieve()).isFailure() ) {
    log << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" << endreq;
  } 

  if ( !m_timersvc.empty()) {
    
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


StatusCode T2CaloBase::finalize(){

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


HLT::ErrorCode
T2CaloBase::prepareRobRequests(const HLT::TriggerElement* inputTE)
{

  // Calculate ROBs needed

  const IRoiDescriptor* roi = 0;
  HLT::ErrorCode hltStatus;

  const TrigRoiDescriptor* roiDescriptor = 0;
  hltStatus = getFeature(inputTE, roiDescriptor);
  roi = roiDescriptor;
 

  if ( hltStatus != HLT::OK ) {
    std::cout <<  MSG::WARNING << " Failed to find RoiDescriptor " << std::endl;
    return hltStatus;
  }
    

#if 0
  // setup ROI bounds
  double etamin = std::max(-1.0 * m_roiEtaLimit, roiDescriptor->eta() - m_etaWidth);
  double etamax = std::min( 1.0 * m_roiEtaLimit, roiDescriptor->eta() + m_etaWidth);
  double phimin = roiDescriptor->phi() - m_phiWidth;
  double phimax = roiDescriptor->phi() + m_phiWidth;
  while (phimin < -M_PI) phimin += 2. * M_PI;
  while (phimax >  M_PI) phimax -= 2. * M_PI;

  //Look at RoIs in FCal
  /// MS: all this stuff checking RoI eta and phi ranges should 
  ///     be in the RegionSelector
  if(roiDescriptor->eta()<-3.2){
    etamin = -m_fcalEtaMax;
    etamax = -m_fcalEtaMin;
  }
  if(roiDescriptor->eta()>3.2){
    etamin = m_fcalEtaMin;
    etamax = m_fcalEtaMax;
  }
#endif


  //  m_data->ROBList(etamin,etamax,phimin,phimax,m_vec_robs);
  m_data->ROBList( *roi, m_vec_robs);

  config()->robRequestInfo()->addRequestScheduledRobIDs(m_vec_robs);

  return HLT::OK;
}

