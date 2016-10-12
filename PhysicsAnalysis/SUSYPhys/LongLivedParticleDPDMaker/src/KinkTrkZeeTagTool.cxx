/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinkTrkZeeTagTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Shimpei Yamamoto (shimpei.yamamoto@cern.ch)

#include "LongLivedParticleDPDMaker/KinkTrkZeeTagTool.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODMissingET/MissingETContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::KinkTrkZeeTagTool::KinkTrkZeeTagTool(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_trigMatchTool("TrigMatchTool/TrigMatchTool"),
  m_trigNames(std::vector<std::string>()),
  m_trigMatchDeltaR(0.1),
  m_doTrigMatch(false),
  m_electronSGKey("ElectronCollection"),
  m_electronIDKeys(std::vector<std::string>()),
  m_electronPtCut(0),
  m_electronEtaMax(9999),
  m_clusterSGKey("egammaClusters"),
  m_clusterEtCut(0),
  m_clusterEtaMax(2.8),
  m_diEleMassLow(50.),
  m_diEleMassHigh(-1),
  m_dPhiMax(10),
  m_sgKeyPrefix("KinkTrk")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("TriggerDecisionTool", m_trigDecisionTool);
  declareProperty("TriggerMatchTool", m_trigMatchTool);
  declareProperty("Triggers", m_trigNames);
  declareProperty("TriggerMatchDeltaR", m_trigMatchDeltaR);
  declareProperty("RequireTriggerMatch", m_doTrigMatch);
  declareProperty("ElectronContainerKey", m_electronSGKey);
  declareProperty("ElectronIDKeys", m_electronIDKeys),
  declareProperty("ElectronPtMin", m_electronPtCut);
  declareProperty("ElectronEtaMax", m_electronEtaMax);
  declareProperty("ClusterContainerKey", m_clusterSGKey);
  declareProperty("ClusterEtMin", m_clusterEtCut);
  declareProperty("ClusterEtaMax", m_clusterEtaMax);
  declareProperty("DiEleMassLow", m_diEleMassLow);
  declareProperty("DiEleMassHigh", m_diEleMassHigh);
  declareProperty("DeltaPhiMax", m_dPhiMax); 
  declareProperty("StoreGateKeyPrefix", m_sgKeyPrefix);
}
  

// Destructor
DerivationFramework::KinkTrkZeeTagTool::~KinkTrkZeeTagTool() {
}  


// Athena initialize and finalize
StatusCode DerivationFramework::KinkTrkZeeTagTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  // trigger decision tool
  if (m_trigNames.size()>0) {
    if (m_trigDecisionTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("TriggerDecisionTool: " << m_trigDecisionTool);

    if (m_trigMatchTool.empty()) {
      ATH_MSG_FATAL("TrigMatchTool is not specified.");
      return StatusCode::FAILURE;
    }
    CHECK(m_trigMatchTool.retrieve());
    ATH_MSG_INFO("TrgMatchTool retrived successfully");
  }

  return StatusCode::SUCCESS;
}


StatusCode DerivationFramework::KinkTrkZeeTagTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}



// Augmentation
StatusCode DerivationFramework::KinkTrkZeeTagTool::addBranches() const
{
  std::vector<float> *diEleMass = new std::vector<float>();
  std::vector<float> *probeEleEt = new std::vector<float>();

  const xAOD::ElectronContainer* electrons(0);
  ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));	

  const xAOD::CaloClusterContainer* clusters(0);
  ATH_CHECK(evtStore()->retrieve(clusters, m_clusterSGKey));

  for (const auto ele: *electrons) {
    if (!checkTagElectron(ele)) continue;
    for (const auto clu: *clusters) {
      if (!checkCluster(clu)) continue;
      if (!checkEleClusPair(ele, clu)) continue;
      diEleMass->push_back((ele->p4()+clu->p4()).M());
      probeEleEt->push_back(clu->et());
    }
  }

  // Writing to SG
  std::string sgKey1(m_sgKeyPrefix+"DiEleMass");
  if (evtStore()->contains< float >(sgKey1)) {
    ATH_MSG_ERROR("StoreGate key " << sgKey1 << "already exists.");
    // avoid mem leak
    delete probeEleEt;
    delete diEleMass;
    return StatusCode::FAILURE;
  }
  CHECK(evtStore()->record(diEleMass, sgKey1));

  std::string sgKey2(m_sgKeyPrefix+"ProbeEleEt");
  if (evtStore()->contains< float >(sgKey2)) {
    ATH_MSG_ERROR("StoreGate key " << sgKey2 << "already exists.");
    delete probeEleEt; // avoid mem leak
    return StatusCode::FAILURE;
  }
  CHECK(evtStore()->record(probeEleEt, sgKey2));

  return StatusCode::SUCCESS;
}


bool DerivationFramework::KinkTrkZeeTagTool::passTrigger(const std::vector<std::string>& triggers) const
{
  for (unsigned int i=0; i<triggers.size(); i++) {
    if (m_trigDecisionTool->isPassed(triggers[i])) return true;
  }
  return false;
}


bool DerivationFramework::KinkTrkZeeTagTool::checkTagElectron(const xAOD::Electron *ele) const
{
  if (!passElectronQuality(ele)) return false;
  if (m_doTrigMatch) {
    if (!passElectronTrigMatch(ele, m_trigNames)) return false;
  }
  return true;
}


bool DerivationFramework::KinkTrkZeeTagTool::checkCluster(const xAOD::CaloCluster *clu) const
{
  if (!passClusterQuality(clu)) return false;
  return true;
}


bool DerivationFramework::KinkTrkZeeTagTool::checkEleClusPair(const xAOD::Electron *ele, const xAOD::CaloCluster *clu) const
{
  if (fabs(ele->p4().DeltaPhi(clu->p4())) > m_dPhiMax) return false;
  float mass = (ele->p4()+clu->p4()).M();
  if (mass < m_diEleMassLow) return false;
  if (mass > m_diEleMassHigh) return false;
  return true;
}


bool DerivationFramework::KinkTrkZeeTagTool::passElectronQuality(const xAOD::Electron *ele) const
{
  if (ele->pt() < m_electronPtCut) return false;
  if (fabs(ele->eta()) > m_electronEtaMax) return false;
  bool passID(false);
  for (unsigned int i=0; i<m_electronIDKeys.size(); i++) {
    if (ele->passSelection(passID, m_electronIDKeys[i])) {
      if (passID) break;
    } else {
      ATH_MSG_WARNING("Cannot find the electron quality flag " << m_electronIDKeys[i]);
    }
  }
  if (!passID) return false;
  return true;
}


bool DerivationFramework::KinkTrkZeeTagTool::passClusterQuality(const xAOD::CaloCluster *clu) const
{
  if (clu->et() < m_clusterEtCut) return false;
  if (fabs(clu->eta()) > m_clusterEtaMax) return false;
  return true;
}


// checking the muon trigger matching
bool DerivationFramework::KinkTrkZeeTagTool::passElectronTrigMatch(const xAOD::Electron *ele, const std::vector<std::string>& triggers) const
{
  for (unsigned int i=0; i<triggers.size(); i++) {
    if (m_trigMatchTool->chainPassedByObject<TrigMatch::TrigElectronEF, xAOD::Electron>(ele, triggers[i], m_trigMatchDeltaR)) return true;
  }
  return false;
}

