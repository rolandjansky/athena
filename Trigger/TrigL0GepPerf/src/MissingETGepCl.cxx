/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigL0GepPerf/MissingETGepCl.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"
#include "StoreGate/ReadHandle.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "TrigT1Interfaces/L1METvalue.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventShape/EventShape.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "TrigEFMissingET/PufitGrid.h"
#include "TrigEFMissingET/PufitUtils.h"

MissingETGepCl::MissingETGepCl( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  declareProperty("inputClusters",m_inCluster="CaloCalTopoClusters");
  declareProperty("outputMET",   m_outMET="cluster420"); 

  //declareProperty( "Property", m_nProperty = 0, "My Example Integer Property" ); //example property declaration

}


MissingETGepCl::~MissingETGepCl() {}


StatusCode MissingETGepCl::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode MissingETGepCl::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //

  return StatusCode::SUCCESS;
}

StatusCode MissingETGepCl::execute() {
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed  
  
  ATH_CHECK(ComputeClusterMET(m_inCluster,m_outMET) ); 
  setFilterPassed(true); //if got here, assume that means algorithm passed 
  return StatusCode::SUCCESS; 
}

StatusCode MissingETGepCl::ComputeClusterMET(std::string clusterContainerName, std::string metName ) {

  ATH_MSG_DEBUG("ABOUT TO RETRIEVE TOPOCLUSTERS " << clusterContainerName);
  const xAOD::CaloClusterContainer * topoClusters = 0;
  CHECK(evtStore()->retrieve(topoClusters,clusterContainerName));

  float Ex = 0.;
  float Ey = 0.;
  float totalEt =0.; 

  for ( const auto* cluster : *topoClusters ) {
    //access m, eta by cluster->m() cluster->eta() 
    float et = cluster->et();
    float phi = cluster->phi();

    Ex -= et * TMath::Cos(phi);
    Ey -= et * TMath::Sin(phi);
    totalEt += et; 
  }

  ATH_MSG_DEBUG( "Calculated MissingET Ex,Ey: " << Ex << "," << Ey); 

  xAOD::EnergySumRoIAuxInfo* METContAux = new xAOD::EnergySumRoIAuxInfo();
  xAOD::EnergySumRoI* METCont = new xAOD::EnergySumRoI();
  METCont->setStore(METContAux);
  
  METCont->setEnergyX(Ex);
  METCont->setEnergyY(Ey);
  METCont->setEnergyT(totalEt);

  ATH_CHECK( evtStore()->record(METCont,metName));
  ATH_CHECK( evtStore()->record(METContAux,metName+"Aux."));
 
  return StatusCode::SUCCESS;
}

