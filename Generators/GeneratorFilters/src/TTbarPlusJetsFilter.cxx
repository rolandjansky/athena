/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarPlusJetsFilter.h"
#include "xAODJet/JetContainer.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "TLorentzVector.h"

TTbarPlusJetsFilter::TTbarPlusJetsFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator),
    m_nbEventProcessed(0), m_nbEventKept(0),
    m_nbJetDiscarded(0), m_nbEventWithJetDiscarded(0)
{
  declareProperty("InputJetContainer", m_jetContainerLoc = "AntiKt4TruthJets");
  declareProperty("PtMinJet",m_ptMinJet = 14000.0);
  declareProperty("EtaMaxJet",m_etaMaxJet = 5.2);
  declareProperty("NbJetMin",m_nbJet = 6);
  declareProperty("PtMinJetB",m_ptMinJetB = 14000.0);
  declareProperty("EtaMaxJetB",m_etaMaxJetB = 2.7);
  declareProperty("NbJetBMin",m_nbJetB = 4);
  declareProperty("PtMinElec",m_ptMinElec = 14000.0);
  declareProperty("EtaMaxElec",m_etaMaxElec = 2.7);
  declareProperty("PtMinMuon",m_ptMinMuon = 14000.0);
  declareProperty("EtaMaxMuon",m_etaMaxMuon = 2.7);
  declareProperty("NbLeptonMin",m_nbLepton = 1);
  declareProperty("SelectLepHadEvents",m_selectLepHad = true); // select only lepton(e/mu)+jet W configurations
  declareProperty("NbEventMax",m_nbEventMax = 0); // 0 means no limit
}


StatusCode TTbarPlusJetsFilter::filterInitialize() {
  m_nbEventProcessed = 0;
  m_nbEventKept = 0;
  m_nbJetDiscarded = 0;
  m_nbEventWithJetDiscarded = 0;
  return StatusCode::SUCCESS;
}


StatusCode TTbarPlusJetsFilter::filterFinalize() {
  ATH_MSG_INFO(" Nb events processed= " << m_nbEventProcessed << " kept= " << m_nbEventKept);
  ATH_MSG_INFO(" Nb jets discarded (electron faking jets)= " << m_nbJetDiscarded << " in " << m_nbEventWithJetDiscarded << " events.");
  return StatusCode::SUCCESS;
}


StatusCode TTbarPlusJetsFilter::filterEvent() {
  m_nbEventProcessed++;
  int ilep = 0;
  int nLepMinus = 0;
  int nLepPlus = 0;

  McEventCollection::const_iterator itr;
  std::vector<CLHEP::HepLorentzVector> electronFakingJetCandidates;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto pitr:  *genEvt) {
      // Look for an electron from a W or a tau (it could end up as a jet):
      if (std::abs(pitr->pdg_id()) == 24 || std::abs(pitr->pdg_id()) == 15) {
        if (pitr->end_vertex()) {
          for (auto thisChild: *(pitr->end_vertex())) {
            if (std::abs(thisChild->pdg_id()) == 11 && std::abs(thisChild->momentum().pseudoRapidity()) <= m_etaMaxJetB) {
              ATH_MSG_VERBOSE("Electron from W or tau decay found in the ID acceptance: ");
              ATH_MSG_VERBOSE(" eta = " << thisChild->momentum().pseudoRapidity() <<
                              " phi = " << thisChild->momentum().phi() <<
                              " pt = "  << thisChild->momentum().perp());
              CLHEP::HepLorentzVector momentum_lv(thisChild->momentum().px(),
                                           thisChild->momentum().py(),
                                           thisChild->momentum().pz(),
                                           thisChild->momentum().e());
              electronFakingJetCandidates.push_back(momentum_lv);
            }
          }
        }
      }

      // Count good leptons
      if ( pitr->status() == 1 &&
           ((std::abs(pitr->pdg_id()) == 11 && pitr->momentum().perp() >= m_ptMinElec && std::abs(pitr->momentum().pseudoRapidity()) <= m_etaMaxElec ) ||
            (std::abs(pitr->pdg_id()) == 13 && pitr->momentum().perp() >= m_ptMinMuon && std::abs(pitr->momentum().pseudoRapidity()) <= m_etaMaxMuon )) ) {
        ATH_MSG_VERBOSE("Good lepton found: PDG ID = " << pitr->pdg_id()
                        << " eta = " << pitr->momentum().pseudoRapidity()
                        << " phi = " << pitr->momentum().phi()
                        << " pt  = " << pitr->momentum().perp());
        ilep++;
      }
      // Count signs of lepton(s)
      if (std::abs(pitr->pdg_id()) == 24) {
        if (pitr->end_vertex()) {
          for (auto thisChild: *(pitr->end_vertex())) {
            if (thisChild->pdg_id() ==  11 || thisChild->pdg_id() ==  13) nLepMinus++;
            if (thisChild->pdg_id() == -11 || thisChild->pdg_id() == -13) nLepPlus++;
          }
        }
      }
    }
  }

  // Identify lepton(e/mu)+jets events:
  // (NB: Had means hadronic or tau)
  bool isHadHad = false;
  bool isLepLep = false;
  bool isLepHad = false;
  if (nLepMinus&&nLepPlus)   isLepLep = true;
  if (nLepMinus&&!nLepPlus)  isLepHad = true;
  if (!nLepMinus&&nLepPlus)  isLepHad = true;
  if (!nLepMinus&&!nLepPlus) isHadHad = true;
  ATH_MSG_DEBUG("W decays: " << (isHadHad ? "HadHad" : "") << (isLepLep ? "LepLep" : "") << (isLepHad ? "LepHad" : ""));

  // Retrieve Jet from TES:
  const xAOD::JetContainer* jetTES;
  CHECK(evtStore()->retrieve(jetTES, m_jetContainerLoc));

  // Loop to remove jet from isolated electron:
  int ijetindet = 0, ijet = 0;
  bool firstInEvent = true;
  ATH_MSG_VERBOSE("Total number of jets = " << jetTES->size() << "  Selected:");
  xAOD::JetContainer::const_iterator jiter = jetTES->begin();
  for (;jiter != jetTES->end();jiter++) {
    const xAOD::Jet* aJet = *jiter;    
    const TLorentzVector jet_tlv = (*jiter)->p4();
    const CLHEP::HepLorentzVector jet_hlv(jet_tlv.Px(), jet_tlv.Py(), jet_tlv.Pz(), jet_tlv.E());

    if (jet_tlv.Et() >= m_ptMinJet && std::abs(aJet->eta())<= m_etaMaxJet) {
      ijet++;
      ATH_MSG_VERBOSE("-> jet : eta = " << aJet->eta() << " phi = " << aJet->phi() << " pt = "  << jet_tlv.Et());
      if (aJet->pt() >= m_ptMinJetB && std::abs(aJet->eta()) <= m_etaMaxJetB) {
        bool isjet = true;
        for (unsigned int iele = 0; iele < electronFakingJetCandidates.size(); iele++) {
          double deltaR  = jet_hlv.deltaR( electronFakingJetCandidates[iele] );
          double deltaPt = std::abs(jet_tlv.Et() - electronFakingJetCandidates[iele].perp());
          ATH_MSG_VERBOSE("--> matching to electron " << iele << " dR = " << deltaR << " dPt = " << deltaPt << " dPt/Pt = " << deltaPt/jet_tlv.Et());
          if (deltaR < 0.4 && deltaPt/jet_tlv.Et() < 0.1) isjet = false;
        }
        if (!isjet) {
          ATH_MSG_VERBOSE("===> jet discarded !");
          m_nbJetDiscarded++;
          if (firstInEvent) {
            m_nbEventWithJetDiscarded++;
            firstInEvent = false;
          }
          ijet--;
        } else {
          ijetindet++;
        }
      }
    }
  }

  ATH_MSG_DEBUG("EventSummary: "
       << " NJet = " << ijet
       << " NJetB = " << ijetindet
       << " NLepOK = " << ilep
       << " Config = "
       << (isHadHad ? "HadHad" : "")
       << (isLepLep ? "LepLep" : "")
       << (isLepHad ? "LepHad" : ""));

  ATH_MSG_VERBOSE("Before this event: NFail= " << m_nFail << " NPass= " << m_nPass);
  if (ijet >= m_nbJet && ijetindet >= m_nbJetB && ilep >= m_nbLepton &&
      ( ( m_selectLepHad && isLepHad ) || (!m_selectLepHad) ) &&
      ( ( m_nbEventKept < m_nbEventMax ) || ( m_nbEventMax == 0 ) ) ) {
    ATH_MSG_DEBUG(">>> Event passed filter <<<");
    m_nbEventKept++;
    if (m_nbEventKept == m_nbEventMax) {
      ATH_MSG_DEBUG("All requested events done ! " << m_nbEventMax);
      ATH_MSG_DEBUG("The job needed " << m_nbEventProcessed << " inclusive events to get the " << m_nbEventKept << " filtered events");
    }
    return StatusCode::SUCCESS;
  }

  // If we get here we have failed
  setFilterPassed(false);
  if (m_nbEventKept < m_nbEventMax || m_nbEventMax == 0) ATH_MSG_DEBUG(">>> Event did not pass filter <<<");
  return StatusCode::SUCCESS;
}
