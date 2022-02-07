/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1JFexSRJetMonitorAlgorithm.h"
#include "AsgDataHandles/ReadDecorHandle.h"


TrigL1JFexSRJetMonitorAlgorithm::TrigL1JFexSRJetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty("L1JetContainer", m_l1jetContainerkey = "LVL1JetRoIs");
}

TrigL1JFexSRJetMonitorAlgorithm::~TrigL1JFexSRJetMonitorAlgorithm() {}



StatusCode TrigL1JFexSRJetMonitorAlgorithm::initialize() {

  ATH_CHECK(m_l1jetContainerkey.initialize());
  if (m_isMatched) {
    m_offmatchedKey = m_l1jetContainerkey.key() + ".matched_" + m_matchedOfflineJetContainer;
    m_offptdiffKey = m_l1jetContainerkey.key() + ".ptdiff_" + m_matchedOfflineJetContainer;
    m_offenergydiffKey = m_l1jetContainerkey.key() + ".energydiff_" + m_matchedOfflineJetContainer;
    m_offmassdiffKey = m_l1jetContainerkey.key() + ".massdiff_" + m_matchedOfflineJetContainer;
    m_offptrespKey = m_l1jetContainerkey.key() + ".ptresp_" + m_matchedOfflineJetContainer;
    m_offenergyrespKey = m_l1jetContainerkey.key() + ".energyresp_" + m_matchedOfflineJetContainer;
    m_offmassrespKey = m_l1jetContainerkey.key() + ".massresp_" + m_matchedOfflineJetContainer;
    m_offptrefKey = m_l1jetContainerkey.key() + ".ptRef_" + m_matchedOfflineJetContainer;
    m_offetarefKey = m_l1jetContainerkey.key() + ".etaRef_" + m_matchedOfflineJetContainer;
    ATH_CHECK( m_offmatchedKey.initialize() );
    ATH_CHECK( m_offptdiffKey.initialize() );
    ATH_CHECK( m_offenergydiffKey.initialize() );
    ATH_CHECK( m_offmassdiffKey.initialize() );
    ATH_CHECK( m_offptrespKey.initialize() );
    ATH_CHECK( m_offenergyrespKey.initialize() );
    ATH_CHECK( m_offmassrespKey.initialize() );
    ATH_CHECK( m_offptrefKey.initialize() );
    ATH_CHECK( m_offetarefKey.initialize() );

    m_hltmatchedKey = m_l1jetContainerkey.key() + ".matched_" + m_matchedHLTJetContainer;
    m_hltptdiffKey = m_l1jetContainerkey.key() + ".ptdiff_" + m_matchedHLTJetContainer;
    m_hltenergydiffKey = m_l1jetContainerkey.key() + ".energydiff_" + m_matchedHLTJetContainer;
    m_hltmassdiffKey = m_l1jetContainerkey.key() + ".massdiff_" + m_matchedHLTJetContainer;
    m_hltptrespKey = m_l1jetContainerkey.key() + ".ptresp_" + m_matchedHLTJetContainer;
    m_hltenergyrespKey = m_l1jetContainerkey.key() + ".energyresp_" + m_matchedHLTJetContainer;
    m_hltmassrespKey = m_l1jetContainerkey.key() + ".massresp_" + m_matchedHLTJetContainer;
    m_hltptrefKey = m_l1jetContainerkey.key() + ".ptRef_" + m_matchedHLTJetContainer;
    m_hltetarefKey = m_l1jetContainerkey.key() + ".etaRef_" + m_matchedHLTJetContainer;
    ATH_CHECK( m_hltmatchedKey.initialize() );
    ATH_CHECK( m_hltptdiffKey.initialize() );
    ATH_CHECK( m_hltenergydiffKey.initialize() );
    ATH_CHECK( m_hltmassdiffKey.initialize() );
    ATH_CHECK( m_hltptrespKey.initialize() );
    ATH_CHECK( m_hltenergyrespKey.initialize() );
    ATH_CHECK( m_hltmassrespKey.initialize() );
    ATH_CHECK( m_hltptrefKey.initialize() );
    ATH_CHECK( m_hltetarefKey.initialize() );
  }
  
  return   AthMonitorAlgorithm::initialize();
}

void TrigL1JFexSRJetMonitorAlgorithm::fillMatchedHistograms(const xAOD::jFexSRJetRoIContainer & jets, const EventContext& ctx) const {

  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, char>  offmatchedHandle(m_offmatchedKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> offptdiffHandle(m_offptdiffKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> offenergydiffHandle(m_offenergydiffKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> offmassdiffHandle(m_offmassdiffKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> offptrespHandle(m_offptrespKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> offenergyrespHandle(m_offenergyrespKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> offmassrespHandle(m_offmassrespKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> offptrefHandle(m_offptrefKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> offetarefHandle(m_offetarefKey, ctx);

  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, char>  hltmatchedHandle(m_hltmatchedKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> hltptdiffHandle(m_hltptdiffKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> hltenergydiffHandle(m_hltenergydiffKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> hltmassdiffHandle(m_hltmassdiffKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> hltptrespHandle(m_hltptrespKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> hltenergyrespHandle(m_hltenergyrespKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> hltmassrespHandle(m_hltmassrespKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> hltptrefHandle(m_hltptrefKey, ctx);
  SG::ReadDecorHandle<xAOD::jFexSRJetRoIContainer, double> hltetarefHandle(m_hltetarefKey, ctx);

  auto offmatched = Monitored::Collection("offmatched", jets, [offmatchedHandle](const xAOD::jFexSRJetRoI * jet) {return offmatchedHandle(*jet);}); 
  auto offdPt = Monitored::Scalar("offptdiff",0.0);
  auto offdEnergy = Monitored::Scalar("offenergydiff",0.0);
  auto offdMass = Monitored::Scalar("offmassdiff",0.0);
  auto offrPt = Monitored::Scalar("offptresp",0.0);
  auto offrEnergy = Monitored::Scalar("offenergyresp",0.0);
  auto offrMass = Monitored::Scalar("offmassresp",0.0);
  auto offptRef = Monitored::Scalar("offptref",0.0);
  auto offetaRef = Monitored::Scalar("offetaref",0.0);

  auto hltmatched = Monitored::Collection("hltmatched", jets, [hltmatchedHandle](const xAOD::jFexSRJetRoI * jet) {return hltmatchedHandle(*jet);}); 
  auto hltdPt = Monitored::Scalar("hltptdiff",0.0);
  auto hltdEnergy = Monitored::Scalar("hltenergydiff",0.0);
  auto hltdMass = Monitored::Scalar("hltmassdiff",0.0);
  auto hltrPt = Monitored::Scalar("hltptresp",0.0);
  auto hltrEnergy = Monitored::Scalar("hltenergyresp",0.0);
  auto hltrMass = Monitored::Scalar("hltmassresp",0.0);
  auto hltptRef = Monitored::Scalar("hltptref",0.0);
  auto hltetaRef = Monitored::Scalar("hltetaref",0.0);

  // Loop over L1 jets and fill pt, energy, mass differences and responses between matched jets, plus reference pT and eta
  auto tool = getGroup("TrigL1JFexSRJetMonitor");
  for(const xAOD::jFexSRJetRoI* jet : jets){
    bool offmatched = offmatchedHandle(*jet);
    bool hltmatched = hltmatchedHandle(*jet);
    if(offmatched){
      offdPt = offptdiffHandle(*jet);
      offdEnergy = offenergydiffHandle(*jet);
      offdMass = offmassdiffHandle(*jet);
      offrPt = offptrespHandle(*jet);
      offrEnergy = offenergyrespHandle(*jet);
      offrMass = offmassrespHandle(*jet);
      offptRef = offptrefHandle(*jet);
      offetaRef = offetarefHandle(*jet);
      fill(tool, offdPt, offdEnergy, offdMass, offrPt, offrEnergy, offrMass, offptRef, offetaRef);
    } 
    if(hltmatched){
      hltdPt = hltptdiffHandle(*jet);
      hltdEnergy = hltenergydiffHandle(*jet);
      hltdMass = hltmassdiffHandle(*jet);
      hltrPt = hltptrespHandle(*jet);
      hltrEnergy = hltenergyrespHandle(*jet);
      hltrMass = hltmassrespHandle(*jet);
      hltptRef = hltptrefHandle(*jet);
      hltetaRef = hltetarefHandle(*jet);
      fill(tool, hltdPt, hltdEnergy, hltdMass, hltrPt, hltrEnergy, hltrMass, hltptRef, hltetaRef);
    } 
  }
  fill(tool,offmatched);
  fill(tool,hltmatched);

  return;
}

StatusCode TrigL1JFexSRJetMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  std::stringstream ss;

  using namespace Monitored;

  // Retrieve the L1 jet container
  SG::ReadHandle<xAOD::jFexSRJetRoIContainer> jets(m_l1jetContainerkey, ctx);
  if( !jets.isValid() ){
    ATH_MSG_WARNING("evtStore() does not contain the L1 jet collection with name " << m_l1jetContainerkey);
    return StatusCode::FAILURE;
  }

  // Declare the quantities which should be monitored
  auto et = Monitored::Scalar<float>("et");
  auto eta   = Monitored::Scalar<float>("eta");
  auto phi   = Monitored::Scalar<float>("phi");

  // Loop over jets
  for(const auto j : *jets){
    // Set the values of the monitored variables for the event
    et = j->et()*0.001;
    eta   = j->eta();
    phi   = j->phi();
    // Fill. First argument is the tool (GMT) name as defined in TrigJetMonitorAlgorithm.py, 
    // all others are the variables to be saved.
    auto tool = getGroup("TrigL1JFexSRJetMonitor");
    fill(tool,et,eta,phi);
  }
  // Adding histograms for L1 jets matched to offline/online jets
  if (m_isMatched) {
    // turn SG::ReadHandle<xAOD::jFexSRJetRoIContainer> into xAOD::jFexSRJetRoIContainer to be able to use Monitored::Collection on the L1 jet collection
    ConstDataVector< xAOD::jFexSRJetRoIContainer > tmpCont(SG::VIEW_ELEMENTS);
    for(const xAOD::jFexSRJetRoI* l1jet : *jets ) tmpCont.push_back(l1jet);
    const xAOD::jFexSRJetRoIContainer *l1JetContainer = tmpCont.asDataVector();
    fillMatchedHistograms(*l1JetContainer, ctx);
  }

  return StatusCode::SUCCESS;
}

