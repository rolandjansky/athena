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
  mL1MuonRoI_RoIWord = 0;
  mL1MuonRoI_pt = 0;
  mL1MuonRoI_eta = 0;
  mL1MuonRoI_phi = 0;

  mL1EmTauRoI_RoIWord = 0;
  mL1EmTauRoI_pt = 0;
  mL1EmTauRoI_eta = 0;
  mL1EmTauRoI_phi = 0;
  mL1EmTauRoI_Core = 0;
  mL1EmTauRoI_EmClus = 0;
  mL1EmTauRoI_TauClus = 0;
  mL1EmTauRoI_EmIsol = 0;
  mL1EmTauRoI_HadIsol = 0;
  mL1EmTauRoI_HadCore = 0;

  mL1JetRoI_RoIWord = 0;
  mL1JetRoI_pt = 0;
  mL1JetRoI_eta = 0;
  mL1JetRoI_phi = 0;
  mL1JetRoI_Et4x4 = 0;
  mL1JetRoI_Et6x6 = 0;
  mL1JetRoI_Et8x8 = 0;
}

L1RoINtComponent::~L1RoINtComponent() {
  del_zero(mL1MuonRoI_RoIWord);
  del_zero(mL1MuonRoI_pt);
  del_zero(mL1MuonRoI_eta);
  del_zero(mL1MuonRoI_phi);

  del_zero(mL1EmTauRoI_RoIWord);
  del_zero(mL1EmTauRoI_pt);
  del_zero(mL1EmTauRoI_eta);
  del_zero(mL1EmTauRoI_phi);
  del_zero(mL1EmTauRoI_Core);
  del_zero(mL1EmTauRoI_EmClus);
  del_zero(mL1EmTauRoI_TauClus);
  del_zero(mL1EmTauRoI_EmIsol);
  del_zero(mL1EmTauRoI_HadIsol);
  del_zero(mL1EmTauRoI_HadCore);

  del_zero(mL1JetRoI_RoIWord);
  del_zero(mL1JetRoI_pt);
  del_zero(mL1JetRoI_eta);
  del_zero(mL1JetRoI_phi);
  del_zero(mL1JetRoI_Et4x4);
  del_zero(mL1JetRoI_Et6x6);
  del_zero(mL1JetRoI_Et8x8);
}

StatusCode L1RoINtComponent::book() {
  mL1MuonRoI_RoIWord = new vector<unsigned int>();
  mL1MuonRoI_pt = new vector<float>();
  mL1MuonRoI_eta = new vector<float>();
  mL1MuonRoI_phi = new vector<float>();
  m_tree->Branch("L1MuonRoI_RoIWord", "std::vector<unsigned int>", 
		 &mL1MuonRoI_RoIWord);
  m_tree->Branch("L1MuonRoI_pt", "std::vector<float>", &mL1MuonRoI_pt);
  m_tree->Branch("L1MuonRoI_eta", "std::vector<float>", &mL1MuonRoI_eta);
  m_tree->Branch("L1MuonRoI_phi", "std::vector<float>", &mL1MuonRoI_phi);

  mL1EmTauRoI_RoIWord = new vector<unsigned int>();
  mL1EmTauRoI_pt = new vector<float>();
  mL1EmTauRoI_eta = new vector<float>();
  mL1EmTauRoI_phi = new vector<float>();
  mL1EmTauRoI_Core = new vector<float>();
  mL1EmTauRoI_EmClus = new vector<float>();
  mL1EmTauRoI_TauClus = new vector<float>();
  mL1EmTauRoI_EmIsol = new vector<float>();
  mL1EmTauRoI_HadIsol = new vector<float>();
  mL1EmTauRoI_HadCore = new vector<float>();
  m_tree->Branch("L1EmTauRoI_RoIWord", "std::vector<unsigned int>", 
		 &mL1EmTauRoI_RoIWord);
  m_tree->Branch("L1EmTauRoI_pt", "std::vector<float>", &mL1EmTauRoI_pt);
  m_tree->Branch("L1EmTauRoI_eta", "std::vector<float>", &mL1EmTauRoI_eta);
  m_tree->Branch("L1EmTauRoI_phi", "std::vector<float>", &mL1EmTauRoI_phi);
  m_tree->Branch("L1EmTauRoI_Core", "std::vector<float>", &mL1EmTauRoI_Core);
  m_tree->Branch("L1EmTauRoI_EmClus", "std::vector<float>", 
		 &mL1EmTauRoI_EmClus);
  m_tree->Branch("L1EmTauRoI_TauClus", "std::vector<float>", 
		 &mL1EmTauRoI_TauClus);
  m_tree->Branch("L1EmTauRoI_EmIsol", "std::vector<float>", 
		 &mL1EmTauRoI_EmIsol);
  m_tree->Branch("L1EmTauRoI_HadIsol", "std::vector<float>", 
		 &mL1EmTauRoI_HadIsol);
  m_tree->Branch("L1EmTauRoI_EmCore", "std::vector<float>", 
		 &mL1EmTauRoI_HadCore);

  mL1JetRoI_RoIWord = new vector<unsigned int>();
  mL1JetRoI_pt = new vector<float>();
  mL1JetRoI_eta = new vector<float>();
  mL1JetRoI_phi = new vector<float>();
  mL1JetRoI_Et4x4 = new vector<float>();
  mL1JetRoI_Et6x6 = new vector<float>();
  mL1JetRoI_Et8x8 = new vector<float>();
  m_tree->Branch("L1JetRoI_RoIWord", "std::vector<unsigned int>", 
		 &mL1JetRoI_RoIWord);
  m_tree->Branch("L1JetRoI_pt", "std::vector<float>", &mL1JetRoI_pt);
  m_tree->Branch("L1JetRoI_eta", "std::vector<float>", &mL1JetRoI_eta);
  m_tree->Branch("L1JetRoI_phi", "std::vector<float>", &mL1JetRoI_phi);
  m_tree->Branch("L1JetRoI_4x4", "std::vector<float>", &mL1JetRoI_Et4x4);
  m_tree->Branch("L1JetRoI_6x6", "std::vector<float>", &mL1JetRoI_Et6x6);
  m_tree->Branch("L1JetRoI_8x8", "std::vector<float>", &mL1JetRoI_Et8x8);

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
      mL1MuonRoI_RoIWord->push_back(p_muon->getROIWord());
      mL1MuonRoI_pt->push_back(p_muon->getThrValue());
      mL1MuonRoI_eta->push_back(p_muon->eta());
      mL1MuonRoI_phi->push_back(p_muon->phi());
    }

    const LVL1_ROI::emtaus_type& emtau_rois = rois->getEmTauROIs();
    LVL1_ROI::emtaus_type::const_iterator p_emtau;
    for (p_emtau=emtau_rois.begin(); p_emtau!=emtau_rois.end(); ++p_emtau) {
      mL1EmTauRoI_RoIWord->push_back(p_emtau->getROIWord());
      mL1EmTauRoI_pt->push_back(p_emtau->pt());
      mL1EmTauRoI_eta->push_back(p_emtau->eta());
      mL1EmTauRoI_phi->push_back(p_emtau->phi());
      mL1EmTauRoI_Core->push_back(p_emtau->getCore());
      mL1EmTauRoI_EmClus->push_back(p_emtau->getEMClus());
      mL1EmTauRoI_TauClus->push_back(p_emtau->getTauClus());
      mL1EmTauRoI_EmIsol->push_back(p_emtau->getEMIsol());
      mL1EmTauRoI_HadIsol->push_back(p_emtau->getHadIsol());
      mL1EmTauRoI_HadCore->push_back(p_emtau->getHadCore());
    }

    const LVL1_ROI::jets_type& jet_rois = rois->getJetROIs();
    LVL1_ROI::jets_type::const_iterator p_jet;
    for (p_jet=jet_rois.begin(); p_jet!=jet_rois.end(); ++p_jet) {
      mL1JetRoI_RoIWord->push_back(p_jet->getROIWord());
      mL1JetRoI_pt->push_back(p_jet->pt());
      mL1JetRoI_eta->push_back(p_jet->eta());
      mL1JetRoI_phi->push_back(p_jet->phi());
      mL1JetRoI_Et4x4->push_back(p_jet->getET4x4());
      mL1JetRoI_Et6x6->push_back(p_jet->getET6x6());
      mL1JetRoI_Et8x8->push_back(p_jet->getET8x8());
    }

  }
  return StatusCode::SUCCESS;
}

void L1RoINtComponent::clear() {
  mL1MuonRoI_RoIWord->clear();
  mL1MuonRoI_pt->clear();
  mL1MuonRoI_eta->clear();
  mL1MuonRoI_phi->clear();

  mL1EmTauRoI_RoIWord->clear();
  mL1EmTauRoI_pt->clear();
  mL1EmTauRoI_eta->clear();
  mL1EmTauRoI_phi->clear();
  mL1EmTauRoI_Core->clear();
  mL1EmTauRoI_EmClus->clear();
  mL1EmTauRoI_TauClus->clear();
  mL1EmTauRoI_EmIsol->clear();
  mL1EmTauRoI_HadIsol->clear();
  mL1EmTauRoI_HadCore->clear();

  mL1JetRoI_RoIWord->clear();
  mL1JetRoI_pt->clear();
  mL1JetRoI_eta->clear();
  mL1JetRoI_phi->clear();
  mL1JetRoI_Et4x4->clear();
  mL1JetRoI_Et6x6->clear();
  mL1JetRoI_Et8x8->clear();
}
