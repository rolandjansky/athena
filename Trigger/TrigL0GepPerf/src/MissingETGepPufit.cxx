/*
*   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigL0GepPerf/MissingETGepPufit.h"
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

MissingETGepPufit::MissingETGepPufit( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  declareProperty("inputClusters",m_inCluster="CaloCalTopoClusters");
  declareProperty("outputMET",   m_outMET="cluster420");

}


MissingETGepPufit::~MissingETGepPufit() {}


StatusCode MissingETGepPufit::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode MissingETGepPufit::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode MissingETGepPufit::execute() {
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed
  ATH_CHECK(PufitMET(m_inCluster,m_outMET, PUfitVar::nSigma)); 
  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode MissingETGepPufit::PufitMET(std::string clusterContainerName, std::string metName, float inputSigma){

  using namespace HLT::MET;
  PufitGrid::GridParameters params{
    PUfitVar::maxEta, PUfitVar::nEtaBins, PUfitVar::nPhiBins};
  PufitGrid grid(params);

  const xAOD::CaloClusterContainer * topoClusters = 0;
  CHECK(evtStore()->retrieve(topoClusters,clusterContainerName));

  // Start by filling the grid with the towers
  for ( const auto* cluster : *topoClusters ) {
    grid += SignedKinematics::fromEtEtaPhi(cluster->et(), cluster->eta(), cluster->phi() );
  }
  // Then calculate mean and variance
  std::pair<double, double> mav = PufitUtils::trimmedMeanAndVariance(grid, PUfitVar::trimFactor);
  // Calculate the threshold
  double threshold = mav.first + inputSigma*sqrt(mav.second);

  // Apply the masks, store the masked towers and calculate the pileup
  // quantities
  PufitUtils::CovarianceSum pileupSum;
  std::vector<SignedKinematics> masked;
  for (PufitGrid::Tower& tower : grid) {
    if (tower.sumEt() > threshold) {
      tower.mask(true);
      masked.push_back(tower);
    }
    else {
      double sigma =
        PUfitVar::caloResFloor*PUfitVar::caloResFloor +
        tower.kinematics().absPt()*PUfitVar::caloResSqrtTerm*PUfitVar::caloResSqrtTerm;
      pileupSum.add(tower, sigma);
    }
  }

  // Now derive the corrections
  std::vector<SignedKinematics> corrections = PufitUtils::pufit(
                                pileupSum.sum,
                                pileupSum.covariance,
                                mav.first,
                                mav.second,
                                masked,
                                PUfitVar::constraintWeight);

  // Sum over the masked towers
  METComponent sum = grid.sum(PufitGrid::SumStrategy::Masked);
  // Now add the corrections - the function above returned them with the right
  // sign for this to work
  for (const SignedKinematics& kin : corrections)
    sum += kin;

  xAOD::EnergySumRoIAuxInfo* METContAux = new xAOD::EnergySumRoIAuxInfo();
  xAOD::EnergySumRoI* METCont = new xAOD::EnergySumRoI();
  METCont->setStore(METContAux);

  METCont->setEnergyX(sum.mpx);
  METCont->setEnergyY(sum.mpy);
  METCont->setEnergyT(sum.met());

  ATH_CHECK( evtStore()->record(METCont,metName));
  ATH_CHECK( evtStore()->record(METContAux,metName+"Aux."));

  return StatusCode::SUCCESS;

}
