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

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigParticle/TrigTau.h"

#include "TrigT2Tau/T2TauEnergyTool.h"

T2TauEnergyTool::T2TauEnergyTool(const std::string & type, const std::string & name, 
                   const IInterface* parent): AthAlgTool(type, name, parent){
  declareInterface<IAlgToolTau>(this);
}
 
T2TauEnergyTool::~T2TauEnergyTool(){
}
 
StatusCode T2TauEnergyTool::initialize(){
  ATH_MSG_DEBUG( "in initialize()" );
  return StatusCode::SUCCESS;
}
 
StatusCode T2TauEnergyTool::finalize(){
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

StatusCode T2TauEnergyTool::execute(const TrigTauCluster *pTrigTauCluster,
				    const TrigTauTracksInfo* /*pTracksi*/,
				    TrigTau& pTrigTau){
  ATH_MSG_DEBUG( name() << "in execute()" );
       
  if( pTrigTauCluster != 0 )
    pTrigTau.setEtCalibCluster(pTrigTauCluster->eCalib() / cosh(pTrigTauCluster->eta()) );
  else
    pTrigTau.setEtCalibCluster(0);
  
  
  return StatusCode::SUCCESS;
}

