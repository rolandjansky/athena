/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauEnergyTool.cxx
// PACKAGE:  Trigger/TrigAlgoritms/TrigT2Tau
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
 
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigParticle/TrigTau.h"

#include "TrigT2Tau/T2TauEnergyTool.h"

T2TauEnergyTool::T2TauEnergyTool(const std::string & type, const std::string & name, 
                   const IInterface* parent): AlgTool(type, name, parent){
  declareInterface<IAlgToolTau>(this);
}
 
T2TauEnergyTool::~T2TauEnergyTool(){
}
 
StatusCode T2TauEnergyTool::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in initialize()" << endreq;
  return StatusCode::SUCCESS;
}
 
StatusCode T2TauEnergyTool::finalize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode T2TauEnergyTool::execute(const TrigTauCluster *pTrigTauCluster,
				    const TrigTauTracksInfo* /*pTracksi*/,
				    TrigTau& pTrigTau){
  MsgStream log(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel(name());
  if (outputLevel <= MSG::DEBUG)
      log << MSG::DEBUG << name() << "in execute()" << endreq;
       
  if( pTrigTauCluster != 0 )
    pTrigTau.setEtCalibCluster(pTrigTauCluster->eCalib() / cosh(pTrigTauCluster->eta()) );
  else
    pTrigTau.setEtCalibCluster(0);
  
  
  return true;
}

