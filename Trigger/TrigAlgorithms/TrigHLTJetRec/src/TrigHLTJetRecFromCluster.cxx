/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetRec_Cluster: Provide the xAOD::CaloCluster specific method
// needed by TruggerHLTJetRec_Base to for m jets from clusters.

#include "TrigHLTJetRec/TrigHLTJetRecFromCluster.h"
#include "TrigHLTJetRec/ClusterToPseudoJetConverter.h"

#include <assert.h>
#include <algorithm>


namespace fastjet{
  class PseudoJet;
}

using fastjet::PseudoJet;


TrigHLTJetRecFromCluster::TrigHLTJetRecFromCluster(const std::string& name, 
                                             ISvcLocator* pSvcLocator):
  TrigHLTJetRecBase<xAOD::CaloClusterContainer>(name, pSvcLocator) {}

TrigHLTJetRecFromCluster::~TrigHLTJetRecFromCluster(){}


HLT::ErrorCode
TrigHLTJetRecFromCluster::getPseudoJets(const xAOD::CaloClusterContainer* ic,
                                        LabelIndex* indexMap,
                                        PseudoJetVector& pjv){

  // override of base class function - due to CaloClusters
  // having calibration states. CaloCluster will always be run
  // so as to produce calibrated clusters. Some jet chains use
  // uncalibrated clusters.
  //
  // convert elements of DataVector<CaloCluster> to pseudojets
  // after switching the state of the CaloCluster objects to
  // the calibration determined by clusterCalib, which is
  // set when the Algorithm is configured.

  auto clusterCalib = this->getClusterCalib();  // config parameter
  auto uncalibrated = clusterCalib == "EM" ? true : false;

  indexMap->addLabel(clusterCalib + "Topo");
  AnyToPseudoJet<InputContainer::const_value_type> apj(indexMap);
  
  ClusterToPseudoJetConverter converter(apj, uncalibrated);

  // create the pseudojets
  std::transform(ic -> cbegin(),
                 ic -> cend(),
                 std::back_inserter(pjv),
                 converter);

  ATH_MSG_DEBUG("No of pseudojets: " << pjv.size());
  for(auto ps : pjv) {ATH_MSG_VERBOSE("PseudoJetDump " << ps.Et() << " "
                                      << std::boolalpha
                                      << uncalibrated << " "
                                      << clusterCalib);}

  return HLT::OK;

}
