/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1RoINtComponent.cxx
*/
#include "TriggerMenuAnalysis/L1RoINtComponent.h"

using namespace std;

template<class T>
void del_zero(T* x) {
  if (x) delete x;
  x = 0;
}

L1RoINtComponent::L1RoINtComponent(NtupleAlgorithm* mainAlg, 
				   const NtComponent::NtComponentParameters& params) : NtComponent::NtupleComponent(mainAlg, params) {
  m_L1MuonRoI_RoIWord = 0;
  m_L1MuonRoI_pt = 0;
  m_L1MuonRoI_eta = 0;
  m_L1MuonRoI_phi = 0;

  m_L1EmTauRoI_RoIWord = 0;
  m_L1EmTauRoI_pt = 0;
  m_L1EmTauRoI_eta = 0;
  m_L1EmTauRoI_phi = 0;
  m_L1EmTauRoI_Core = 0;
  m_L1EmTauRoI_EmClus = 0;
  m_L1EmTauRoI_TauClus = 0;
  m_L1EmTauRoI_EmIsol = 0;
  m_L1EmTauRoI_HadIsol = 0;
  m_L1EmTauRoI_HadCore = 0;

  m_L1JetRoI_RoIWord = 0;
  m_L1JetRoI_pt = 0;
  m_L1JetRoI_eta = 0;
  m_L1JetRoI_phi = 0;
  m_L1JetRoI_Et4x4 = 0;
  m_L1JetRoI_Et6x6 = 0;
  m_L1JetRoI_Et8x8 = 0;
}

L1RoINtComponent::~L1RoINtComponent() {
  del_zero(m_L1MuonRoI_RoIWord);
  del_zero(m_L1MuonRoI_pt);
  del_zero(m_L1MuonRoI_eta);
  del_zero(m_L1MuonRoI_phi);

  del_zero(m_L1EmTauRoI_RoIWord);
  del_zero(m_L1EmTauRoI_pt);
  del_zero(m_L1EmTauRoI_eta);
  del_zero(m_L1EmTauRoI_phi);
  del_zero(m_L1EmTauRoI_Core);
  del_zero(m_L1EmTauRoI_EmClus);
  del_zero(m_L1EmTauRoI_TauClus);
  del_zero(m_L1EmTauRoI_EmIsol);
  del_zero(m_L1EmTauRoI_HadIsol);
  del_zero(m_L1EmTauRoI_HadCore);

  del_zero(m_L1JetRoI_RoIWord);
  del_zero(m_L1JetRoI_pt);
  del_zero(m_L1JetRoI_eta);
  del_zero(m_L1JetRoI_phi);
  del_zero(m_L1JetRoI_Et4x4);
  del_zero(m_L1JetRoI_Et6x6);
  del_zero(m_L1JetRoI_Et8x8);
}

StatusCode L1RoINtComponent::book() {
  m_L1MuonRoI_RoIWord = new vector<unsigned int>();
  m_L1MuonRoI_pt = new vector<float>();
  m_L1MuonRoI_eta = new vector<float>();
  m_L1MuonRoI_phi = new vector<float>();
  m_tree->Branch("L1MuonRoI_RoIWord", "std::vector<unsigned int>", 
		 &m_L1MuonRoI_RoIWord);
  m_tree->Branch("L1MuonRoI_pt", "std::vector<float>", &m_L1MuonRoI_pt);
  m_tree->Branch("L1MuonRoI_eta", "std::vector<float>", &m_L1MuonRoI_eta);
  m_tree->Branch("L1MuonRoI_phi", "std::vector<float>", &m_L1MuonRoI_phi);

  m_L1EmTauRoI_RoIWord = new vector<unsigned int>();
  m_L1EmTauRoI_pt = new vector<float>();
  m_L1EmTauRoI_eta = new vector<float>();
  m_L1EmTauRoI_phi = new vector<float>();
  m_L1EmTauRoI_Core = new vector<float>();
  m_L1EmTauRoI_EmClus = new vector<float>();
  m_L1EmTauRoI_TauClus = new vector<float>();
  m_L1EmTauRoI_EmIsol = new vector<float>();
  m_L1EmTauRoI_HadIsol = new vector<float>();
  m_L1EmTauRoI_HadCore = new vector<float>();
  m_tree->Branch("L1EmTauRoI_RoIWord", "std::vector<unsigned int>", 
		 &m_L1EmTauRoI_RoIWord);
  m_tree->Branch("L1EmTauRoI_pt", "std::vector<float>", &m_L1EmTauRoI_pt);
  m_tree->Branch("L1EmTauRoI_eta", "std::vector<float>", &m_L1EmTauRoI_eta);
  m_tree->Branch("L1EmTauRoI_phi", "std::vector<float>", &m_L1EmTauRoI_phi);
  m_tree->Branch("L1EmTauRoI_Core", "std::vector<float>", &m_L1EmTauRoI_Core);
  m_tree->Branch("L1EmTauRoI_EmClus", "std::vector<float>", 
		 &m_L1EmTauRoI_EmClus);
  m_tree->Branch("L1EmTauRoI_TauClus", "std::vector<float>", 
		 &m_L1EmTauRoI_TauClus);
  m_tree->Branch("L1EmTauRoI_EmIsol", "std::vector<float>", 
		 &m_L1EmTauRoI_EmIsol);
  m_tree->Branch("L1EmTauRoI_HadIsol", "std::vector<float>", 
		 &m_L1EmTauRoI_HadIsol);
  m_tree->Branch("L1EmTauRoI_EmCore", "std::vector<float>", 
		 &m_L1EmTauRoI_HadCore);

  m_L1JetRoI_RoIWord = new vector<unsigned int>();
  m_L1JetRoI_pt = new vector<float>();
  m_L1JetRoI_eta = new vector<float>();
  m_L1JetRoI_phi = new vector<float>();
  m_L1JetRoI_Et4x4 = new vector<float>();
  m_L1JetRoI_Et6x6 = new vector<float>();
  m_L1JetRoI_Et8x8 = new vector<float>();
  m_tree->Branch("L1JetRoI_RoIWord", "std::vector<unsigned int>", 
		 &m_L1JetRoI_RoIWord);
  m_tree->Branch("L1JetRoI_pt", "std::vector<float>", &m_L1JetRoI_pt);
  m_tree->Branch("L1JetRoI_eta", "std::vector<float>", &m_L1JetRoI_eta);
  m_tree->Branch("L1JetRoI_phi", "std::vector<float>", &m_L1JetRoI_phi);
  m_tree->Branch("L1JetRoI_4x4", "std::vector<float>", &m_L1JetRoI_Et4x4);
  m_tree->Branch("L1JetRoI_6x6", "std::vector<float>", &m_L1JetRoI_Et6x6);
  m_tree->Branch("L1JetRoI_8x8", "std::vector<float>", &m_L1JetRoI_Et8x8);

  return StatusCode::SUCCESS;
}

StatusCode L1RoINtComponent::fill() {
  const LVL1_ROI* rois;

  clear();

  if ( (*m_evtStore)->retrieve(rois).isFailure()) {
    (*m_msg) << MSG::WARNING << "Cannot find LVL1_ROI" << endreq;
  } else {
    const LVL1_ROI::muons_type& muon_rois = rois->getMuonROIs();
    LVL1_ROI::muons_type::const_iterator p_muon;
    for (p_muon=muon_rois.begin(); p_muon!=muon_rois.end(); ++p_muon) {
      m_L1MuonRoI_RoIWord->push_back(p_muon->getROIWord());
      m_L1MuonRoI_pt->push_back(p_muon->getThrValue());
      m_L1MuonRoI_eta->push_back(p_muon->eta());
      m_L1MuonRoI_phi->push_back(p_muon->phi());
    }

    const LVL1_ROI::emtaus_type& emtau_rois = rois->getEmTauROIs();
    LVL1_ROI::emtaus_type::const_iterator p_emtau;
    for (p_emtau=emtau_rois.begin(); p_emtau!=emtau_rois.end(); ++p_emtau) {
      m_L1EmTauRoI_RoIWord->push_back(p_emtau->getROIWord());
      m_L1EmTauRoI_pt->push_back(p_emtau->pt());
      m_L1EmTauRoI_eta->push_back(p_emtau->eta());
      m_L1EmTauRoI_phi->push_back(p_emtau->phi());
      m_L1EmTauRoI_Core->push_back(p_emtau->getCore());
      m_L1EmTauRoI_EmClus->push_back(p_emtau->getEMClus());
      m_L1EmTauRoI_TauClus->push_back(p_emtau->getTauClus());
      m_L1EmTauRoI_EmIsol->push_back(p_emtau->getEMIsol());
      m_L1EmTauRoI_HadIsol->push_back(p_emtau->getHadIsol());
      m_L1EmTauRoI_HadCore->push_back(p_emtau->getHadCore());
    }

    const LVL1_ROI::jets_type& jet_rois = rois->getJetROIs();
    LVL1_ROI::jets_type::const_iterator p_jet;
    for (p_jet=jet_rois.begin(); p_jet!=jet_rois.end(); ++p_jet) {
      m_L1JetRoI_RoIWord->push_back(p_jet->getROIWord());
      m_L1JetRoI_pt->push_back(p_jet->pt());
      m_L1JetRoI_eta->push_back(p_jet->eta());
      m_L1JetRoI_phi->push_back(p_jet->phi());
      m_L1JetRoI_Et4x4->push_back(p_jet->getET4x4());
      m_L1JetRoI_Et6x6->push_back(p_jet->getET6x6());
      m_L1JetRoI_Et8x8->push_back(p_jet->getET8x8());
    }

  }
  return StatusCode::SUCCESS;
}

void L1RoINtComponent::clear() {
  m_L1MuonRoI_RoIWord->clear();
  m_L1MuonRoI_pt->clear();
  m_L1MuonRoI_eta->clear();
  m_L1MuonRoI_phi->clear();

  m_L1EmTauRoI_RoIWord->clear();
  m_L1EmTauRoI_pt->clear();
  m_L1EmTauRoI_eta->clear();
  m_L1EmTauRoI_phi->clear();
  m_L1EmTauRoI_Core->clear();
  m_L1EmTauRoI_EmClus->clear();
  m_L1EmTauRoI_TauClus->clear();
  m_L1EmTauRoI_EmIsol->clear();
  m_L1EmTauRoI_HadIsol->clear();
  m_L1EmTauRoI_HadCore->clear();

  m_L1JetRoI_RoIWord->clear();
  m_L1JetRoI_pt->clear();
  m_L1JetRoI_eta->clear();
  m_L1JetRoI_phi->clear();
  m_L1JetRoI_Et4x4->clear();
  m_L1JetRoI_Et6x6->clear();
  m_L1JetRoI_Et8x8->clear();
}
