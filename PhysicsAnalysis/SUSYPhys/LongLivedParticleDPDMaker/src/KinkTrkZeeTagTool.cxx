/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinkTrkZeeTagTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Shimpei Yamamoto (shimpei.yamamoto@cern.ch)

#include "LongLivedParticleDPDMaker/KinkTrkZeeTagTool.h"
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
  m_electronIDKeys(std::vector<std::string>()),
  m_electronPtCut(0),
  m_electronEtaMax(9999),
  m_clusterEtCut(0),
  m_clusterEtaMax(2.8),
  m_diEleMassLow(50.),
  m_diEleMassHigh(-1),
  m_dPhiMax(10)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("TriggerDecisionTool", m_trigDecisionTool);
  declareProperty("TriggerMatchTool", m_trigMatchTool);
  declareProperty("Triggers", m_trigNames);
  declareProperty("TriggerMatchDeltaR", m_trigMatchDeltaR);
  declareProperty("RequireTriggerMatch", m_doTrigMatch);
  declareProperty("ElectronIDKeys", m_electronIDKeys),
  declareProperty("ElectronPtMin", m_electronPtCut);
  declareProperty("ElectronEtaMax", m_electronEtaMax);
  declareProperty("ClusterEtMin", m_clusterEtCut);
  declareProperty("ClusterEtaMax", m_clusterEtaMax);
  declareProperty("DiEleMassLow", m_diEleMassLow);
  declareProperty("DiEleMassHigh", m_diEleMassHigh);
  declareProperty("DeltaPhiMax", m_dPhiMax);
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

  ATH_CHECK(m_electronSGKey.initialize());
  ATH_CHECK(m_clusterSGKey.initialize());
  ATH_CHECK(m_KinkTrkDiEleMassKey.initialize());
  ATH_CHECK(m_KinkTrkProbeEleEtKey.initialize());

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
  SG::WriteHandle< std::vector<float> > diEleMass(m_KinkTrkDiEleMassKey);
  ATH_CHECK(diEleMass.record(std::make_unique< std::vector<float> >()));

  SG::WriteHandle< std::vector<float> > probeEleEt(m_KinkTrkProbeEleEtKey);
  ATH_CHECK(probeEleEt.record(std::make_unique< std::vector<float> >()));

  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronSGKey);
  SG::ReadHandle<xAOD::CaloClusterContainer> clusters(m_clusterSGKey);
	
  for (const auto ele: *electrons) {
    if (!checkTagElectron(ele)) continue;
    for (const auto clu: *clusters) {
      if (!checkCluster(clu)) continue;
      if (!checkEleClusPair(ele, clu)) continue;
      diEleMass->push_back((ele->p4()+clu->p4()).M());
      probeEleEt->push_back(clu->et());
    }
  }

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
  if (std::abs(ele->p4().DeltaPhi(clu->p4())) > m_dPhiMax) return false;
  float mass = (ele->p4()+clu->p4()).M();
  if (mass < m_diEleMassLow) return false;
  if (mass > m_diEleMassHigh) return false;
  return true;
}


bool DerivationFramework::KinkTrkZeeTagTool::passElectronQuality(const xAOD::Electron *ele) const
{
  if (ele->pt() < m_electronPtCut) return false;
  if (std::abs(ele->eta()) > m_electronEtaMax) return false;
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
  if (std::abs(clu->eta()) > m_clusterEtaMax) return false;
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

