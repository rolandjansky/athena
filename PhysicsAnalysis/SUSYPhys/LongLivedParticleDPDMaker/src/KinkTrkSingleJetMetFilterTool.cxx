/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LongLivedParticleDPDMaker/KinkTrkSingleJetMetFilterTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include <vector>

/// Service Constructor
DerivationFramework::KinkTrkSingleJetMetFilterTool::KinkTrkSingleJetMetFilterTool( const std::string& t,
                                                           const std::string& n,
                                                           const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_passAll(false),
  m_LeptonVeto(false),
  m_jetSGKey("AntiKt4LCTopoJets"),
  m_metSGKey("MET_RefFinal"),
  m_metTerm("Final"),
  m_muonSGKey("Muons"),
  m_muonIDKey("Medium"),
  m_electronSGKey("ElectronCollection"),
  m_electronIDKey("Tight"),
  m_metCut(-1),
  m_jetPtCut(0),
  m_jetEtaMax(3.2),
  m_leptonPtCut(30.0),
  m_leptonEtaMax(2.5)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("passAll", m_passAll); 
    declareProperty("LeptonVeto", m_LeptonVeto);
    declareProperty("JetContainerKey", m_jetSGKey);
    declareProperty("MetContainerKey", m_metSGKey);
    declareProperty("MetTerm", m_metTerm);
    declareProperty("MuonContainerKey", m_muonSGKey);
    declareProperty("MuonIDKey", m_muonIDKey);
    declareProperty("ElectronContainerKey", m_electronSGKey);
    declareProperty("ElectronIDKey", m_electronIDKey);
    declareProperty("MetCut", m_metCut);
    declareProperty("JetPtCut", m_jetPtCut);
    declareProperty("JetEtaMax", m_jetEtaMax);
    declareProperty("LeptonPtCut", m_leptonPtCut);
    declareProperty("LeptonEtaMax", m_leptonEtaMax);
  }
 

/// Destructor
DerivationFramework::KinkTrkSingleJetMetFilterTool::~KinkTrkSingleJetMetFilterTool() {}


// Athena initialize and finalize
StatusCode DerivationFramework::KinkTrkSingleJetMetFilterTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::KinkTrkSingleJetMetFilterTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_ntot << " events, " << m_npass << " events passed filter ");
  ATH_MSG_INFO("Efficiency = "<< 100.0*(float)m_npass/(float)m_ntot <<" % ");
  return StatusCode::SUCCESS;
}


// The filter itself
bool DerivationFramework::KinkTrkSingleJetMetFilterTool::eventPassesFilter() const
{
  ++m_ntot;
  bool acceptEvent(false);

  // If passAll is selected, accept all events
  if(m_passAll){
      ATH_MSG_DEBUG("Event passed because passAll is true.");
      return acceptEvent; 
  }


  // Retrieve MET container 
  if (m_metCut > 0) {
    const xAOD::MissingETContainer* metContainer(0);
    const xAOD::MissingET* met;

    ATH_CHECK( evtStore()->retrieve(metContainer, m_metSGKey) );	
    met = (*metContainer)[m_metTerm];
    if (!met) {
      ATH_MSG_ERROR("Cannot retrieve MissingET term " << m_metTerm << " in " << m_metSGKey);
      return acceptEvent; 
    }
    //ATH_MSG_DEBUG("MET and phi are " << met->met() << " " << met->phi());
  
    if (met->met() < m_metCut) return acceptEvent; 
  }

  // Loop over jets
  std::vector<int> goodJetIndices;
  const DataVector<xAOD::Jet>* jetContainer(0);
  ATH_CHECK( evtStore()->retrieve(jetContainer, m_jetSGKey) );
  ATH_MSG_DEBUG("retrieved jet collection size "<< jetContainer->size());
  
  DataVector<xAOD::Jet>::const_iterator jetItr;
  int i = 0;
  for (jetItr=jetContainer->begin(); jetItr!=jetContainer->end(); ++jetItr) {
      if ( (*jetItr)->pt() > m_jetPtCut && fabs((*jetItr)->eta()) < m_jetEtaMax ) goodJetIndices.push_back(i);
      i++;
  }
  if (goodJetIndices.size() == 0) {
      return acceptEvent; 
  } 
  
  if (m_LeptonVeto) {
    // Retrieve muon container	
    const xAOD::MuonContainer* muons(0);
    ATH_CHECK( evtStore()->retrieve(muons, m_muonSGKey) );	
    for (auto muon: *muons) {
      bool passID(false);
      int qflag(0);
      if (m_muonIDKey == "Loose") {
        qflag = xAOD::Muon::Loose;
      } else if (m_muonIDKey == "Medium") {
        qflag = xAOD::Muon::Medium;
      } else if (m_muonIDKey == "Tight") {
        qflag = xAOD::Muon::Tight;
      } else {
        ATH_MSG_WARNING("Cannot find the muon quality flag " << m_muonIDKey << ". Use Tight instead.");
        qflag = xAOD::Muon::Tight;
      }

      if (muon->quality() == xAOD::Muon::Tight) {
        if ( qflag == xAOD::Muon::Tight )  passID = true;
      } else if (muon->quality() == xAOD::Muon::Medium) {
        if ( qflag == xAOD::Muon::Medium || qflag == xAOD::Muon::Tight ) passID = true;
      } else if (muon->quality() == xAOD::Muon::Loose) {
        if ( qflag == xAOD::Muon::Loose || qflag == xAOD::Muon::Medium || qflag == xAOD::Muon::Tight ) passID = true;
      }

      if (muon->pt() > m_leptonPtCut && fabs(muon->eta()) < m_leptonEtaMax && passID) {
        return acceptEvent; 
      }
    }

    // Retrieve electron container	
    const xAOD::ElectronContainer* electrons(0);
    ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));	
    for (auto ele: *electrons) {
      bool passID(false);
      if (!ele->passSelection(passID, m_electronIDKey)) {
        ATH_MSG_WARNING("Cannot find the electron quality flag " << m_muonIDKey);
      }
      if (ele->pt() > m_leptonPtCut && fabs(ele->eta()) < m_leptonEtaMax && passID) {
        return acceptEvent; 
      }
    }
  }

  acceptEvent = true;
  ++m_npass;
  return acceptEvent; 

}
