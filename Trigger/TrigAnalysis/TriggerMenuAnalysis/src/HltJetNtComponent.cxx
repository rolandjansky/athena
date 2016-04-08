/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  HltJetNtComponent.cxx
*/
#include "TriggerMenuAnalysis/HltJetNtComponent.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "JetEvent/JetCollection.h"

using namespace std;

template<class T>
void del_zero(T* x) {
  if (x) delete x;
  x = 0;
}

HltJetNtComponent::HltJetNtComponent(NtupleAlgorithm* mainAlg, 
				   const NtComponent::NtComponentParameters& params) : NtComponent::NtupleComponent(mainAlg, params) {
  m_NL2Jets = 0;
  m_trigT2Jet_et = 0;
  m_trigT2Jet_ehad0 = 0;
  m_trigT2Jet_eem0 = 0;
  m_trigT2Jet_eta = 0;
  m_trigT2Jet_phi = 0;

  m_NEFJets = 0;
  m_jet_EF_et = 0;
  m_jet_EF_eta = 0;
  m_jet_EF_phi = 0;
}

HltJetNtComponent::~HltJetNtComponent() {
  del_zero(m_trigT2Jet_et);
  del_zero(m_trigT2Jet_ehad0);
  del_zero(m_trigT2Jet_eem0);
  del_zero(m_trigT2Jet_eta);
  del_zero(m_trigT2Jet_phi);
  del_zero(m_jet_EF_et);
  del_zero(m_jet_EF_eta);
  del_zero(m_jet_EF_phi);
}

StatusCode HltJetNtComponent::book() {
  m_trigT2Jet_et = new std::vector<float>();
  m_trigT2Jet_ehad0 = new std::vector<float>();
  m_trigT2Jet_eem0 = new std::vector<float>();
  m_trigT2Jet_eta = new std::vector<float>();
  m_trigT2Jet_phi = new std::vector<float>();
  m_tree->Branch("NL2Jets", &m_NL2Jets, "NL2Jets/I");
  m_tree->Branch("TrigT2Jet_et", "std::vector<float>", &m_trigT2Jet_et);
  m_tree->Branch("TrigT2Jet_ehad0", "std::vector<float>", &m_trigT2Jet_ehad0);
  m_tree->Branch("TrigT2Jet_eem0", "std::vector<float>", &m_trigT2Jet_eem0);
  m_tree->Branch("TrigT2Jet_eta", "std::vector<float>", &m_trigT2Jet_eta);
  m_tree->Branch("TrigT2Jet_phi", "std::vector<float>", &m_trigT2Jet_phi);

  m_jet_EF_et = new std::vector<float>();
  m_jet_EF_eta = new std::vector<float>();
  m_jet_EF_phi = new std::vector<float>();
  m_tree->Branch("NEFJets", &m_NEFJets, "NEFJets/I");
  m_tree->Branch("Jet_EF_et", "std::vector<float>", &m_jet_EF_et);
  m_tree->Branch("Jet_EF_eta", "std::vector<float>", &m_jet_EF_eta);
  m_tree->Branch("Jet_EF_phi", "std::vector<float>", &m_jet_EF_phi);

  return StatusCode::SUCCESS;
}

StatusCode HltJetNtComponent::fill() {
  const TrigT2JetContainer* l2jets=0;
  const JetCollection* efjets=0;

  clear();

  if ( (*m_evtStore)->retrieve(l2jets, "HLT_TrigT2CaloJet").isFailure()) {
    (*m_msg) << MSG::WARNING << "Cannot find TrigT2JetContainer" << endreq;
  } else {
    TrigT2JetContainer::const_iterator p;
    for (p=l2jets->begin(); p!=l2jets->end(); ++p) {
      //      float eta = (*p)->eta();
      //      m_trigT2Jet_et->push_back( (*p)->e()/cosh(eta));
      m_trigT2Jet_et->push_back( (*p)->et());
      m_trigT2Jet_ehad0->push_back( (*p)->ehad0());
      m_trigT2Jet_eem0->push_back( (*p)->eem0());
      m_trigT2Jet_eta->push_back( (*p)->eta());
      m_trigT2Jet_phi->push_back( (*p)->phi());
    }
  }
  if ( (*m_evtStore)->retrieve(efjets, "HLT_TrigJetRec").isFailure()) {
    (*m_msg) << MSG::WARNING << "Cannot find JetCollection" << endreq;
  } else {
    JetCollection::const_iterator p;
    for (p=efjets->begin(); p!=efjets->end(); ++p) {
      m_jet_EF_et->push_back( (*p)->et());
      m_jet_EF_eta->push_back( (*p)->eta());
      m_jet_EF_phi->push_back( (*p)->phi());
    }
  }
  return StatusCode::SUCCESS;
}

void HltJetNtComponent::clear() {
  m_NL2Jets = 0;
  m_trigT2Jet_et->clear();
  m_trigT2Jet_ehad0->clear();
  m_trigT2Jet_eem0->clear();
  m_trigT2Jet_eta->clear();
  m_trigT2Jet_phi->clear();

  m_NEFJets = 0;
  m_jet_EF_et->clear();
  m_jet_EF_eta->clear();
  m_jet_EF_phi->clear();
}
