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
  mNL2Jets = 0;
  mTrigT2Jet_et = 0;
  mTrigT2Jet_ehad0 = 0;
  mTrigT2Jet_eem0 = 0;
  mTrigT2Jet_eta = 0;
  mTrigT2Jet_phi = 0;

  mNEFJets = 0;
  mJet_EF_et = 0;
  mJet_EF_eta = 0;
  mJet_EF_phi = 0;
}

HltJetNtComponent::~HltJetNtComponent() {
  del_zero(mTrigT2Jet_et);
  del_zero(mTrigT2Jet_ehad0);
  del_zero(mTrigT2Jet_eem0);
  del_zero(mTrigT2Jet_eta);
  del_zero(mTrigT2Jet_phi);
  del_zero(mJet_EF_et);
  del_zero(mJet_EF_eta);
  del_zero(mJet_EF_phi);
}

StatusCode HltJetNtComponent::book() {
  mTrigT2Jet_et = new std::vector<float>();
  mTrigT2Jet_ehad0 = new std::vector<float>();
  mTrigT2Jet_eem0 = new std::vector<float>();
  mTrigT2Jet_eta = new std::vector<float>();
  mTrigT2Jet_phi = new std::vector<float>();
  m_tree->Branch("NL2Jets", &mNL2Jets, "NL2Jets/I");
  m_tree->Branch("TrigT2Jet_et", "std::vector<float>", &mTrigT2Jet_et);
  m_tree->Branch("TrigT2Jet_ehad0", "std::vector<float>", &mTrigT2Jet_ehad0);
  m_tree->Branch("TrigT2Jet_eem0", "std::vector<float>", &mTrigT2Jet_eem0);
  m_tree->Branch("TrigT2Jet_eta", "std::vector<float>", &mTrigT2Jet_eta);
  m_tree->Branch("TrigT2Jet_phi", "std::vector<float>", &mTrigT2Jet_phi);

  mJet_EF_et = new std::vector<float>();
  mJet_EF_eta = new std::vector<float>();
  mJet_EF_phi = new std::vector<float>();
  m_tree->Branch("NEFJets", &mNEFJets, "NEFJets/I");
  m_tree->Branch("Jet_EF_et", "std::vector<float>", &mJet_EF_et);
  m_tree->Branch("Jet_EF_eta", "std::vector<float>", &mJet_EF_eta);
  m_tree->Branch("Jet_EF_phi", "std::vector<float>", &mJet_EF_phi);

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
      //      mTrigT2Jet_et->push_back( (*p)->e()/cosh(eta));
      mTrigT2Jet_et->push_back( (*p)->et());
      mTrigT2Jet_ehad0->push_back( (*p)->ehad0());
      mTrigT2Jet_eem0->push_back( (*p)->eem0());
      mTrigT2Jet_eta->push_back( (*p)->eta());
      mTrigT2Jet_phi->push_back( (*p)->phi());
    }
  }
  if ( (*m_evtStore)->retrieve(efjets, "HLT_TrigJetRec").isFailure()) {
    (*m_msg) << MSG::WARNING << "Cannot find JetCollection" << endreq;
  } else {
    JetCollection::const_iterator p;
    for (p=efjets->begin(); p!=efjets->end(); ++p) {
      mJet_EF_et->push_back( (*p)->et());
      mJet_EF_eta->push_back( (*p)->eta());
      mJet_EF_phi->push_back( (*p)->phi());
    }
  }
  return StatusCode::SUCCESS;
}

void HltJetNtComponent::clear() {
  mNL2Jets = 0;
  mTrigT2Jet_et->clear();
  mTrigT2Jet_ehad0->clear();
  mTrigT2Jet_eem0->clear();
  mTrigT2Jet_eta->clear();
  mTrigT2Jet_phi->clear();

  mNEFJets = 0;
  mJet_EF_et->clear();
  mJet_EF_eta->clear();
  mJet_EF_phi->clear();
}
