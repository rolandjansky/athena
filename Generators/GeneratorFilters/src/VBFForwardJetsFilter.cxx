/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/VBFForwardJetsFilter.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "ParticleEvent/ParticleBaseContainer.h"
#include "xAODJet/JetContainer.h"


// Pt  High --> Low
class High2LowByJetClassPt {
public:
  bool operator () (const xAOD::Jet *t1, const xAOD::Jet *t2) const {
    return (t1->pt() > t2->pt());
  }
};


VBFForwardJetsFilter::VBFForwardJetsFilter(const std::string & name, ISvcLocator * pSvcLocator)
  : GenFilter (name, pSvcLocator)
{
  declareProperty("JetMinPt",m_JetMinPt = 10.*Gaudi::Units::GeV);
  declareProperty("JetMaxEta",m_JetMaxEta = 5.);
  declareProperty("NJets",m_NJets = 2);
  declareProperty("Jet1MinPt",m_Jet1MinPt = 20.*Gaudi::Units::GeV);
  declareProperty("Jet1MaxEta",m_Jet1MaxEta = 5.);
  declareProperty("Jet2MinPt",m_Jet2MinPt = 10.*Gaudi::Units::GeV);
  declareProperty("Jet2MaxEta",m_Jet2MaxEta = 5.);
  declareProperty("UseOppositeSignEtaJet1Jet2",m_UseOppositeSignEtaJet1Jet2 = false);
  declareProperty("MassJJ",m_MassJJ = 300.*Gaudi::Units::GeV);
  declareProperty("DeltaEtaJJ",m_DeltaEtaJJ = 2.0);
  declareProperty("UseLeadingJJ",m_UseLeadingJJ = false);
  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt4TruthJets");
  declareProperty("LGMinPt",m_LGMinPt = 10.*Gaudi::Units::GeV);
  declareProperty("LGMaxEta",m_LGMaxEta = 2.5);
  declareProperty("DeltaRJLG",m_DeltaRJLG = 0.05);
  declareProperty("RatioPtJLG",m_RatioPtJLG = 0.3);
}


StatusCode VBFForwardJetsFilter::filterInitialize() {
  ATH_MSG_INFO("*** Jet selection ***");
  ATH_MSG_INFO("xAOD::JetContainer=" <<  m_TruthJetContainerName);
  ATH_MSG_INFO("JetMinPt=" << m_JetMinPt);
  ATH_MSG_INFO("JetMaxEta=" << m_JetMaxEta);
  ATH_MSG_INFO("*** Apply number of jets(=Nj)? ***");
  ATH_MSG_INFO("NJets=" << m_NJets);
  ATH_MSG_INFO("*** Leading 1st jet (Nj>=1) ***");
  ATH_MSG_INFO("Jet1MinPt=" << m_Jet1MinPt);
  ATH_MSG_INFO("Jet1MaxEta=" << m_Jet1MaxEta);
  ATH_MSG_INFO("*** Leading 2nd jet (Nj>=2) ***");
  ATH_MSG_INFO("Jet2MinPt=" << m_Jet2MinPt);
  ATH_MSG_INFO("Jet2MaxEta=" << m_Jet2MaxEta);
  ATH_MSG_INFO("*** 1st and 2nd jets in opposite eta region (Nj>=2) ***");
  ATH_MSG_INFO("UseOppositeSignEtaJet1Jet2=" << m_UseOppositeSignEtaJet1Jet2);
  ATH_MSG_INFO("*** Mass of JJ (Nj>=2) ***");
  ATH_MSG_INFO("MassJJ=" << m_MassJJ);
  ATH_MSG_INFO("*** DeltaEta of JJ (Nj>=2) ***");
  ATH_MSG_INFO("DeltaEtaJJ=" << m_DeltaEtaJJ);
  ATH_MSG_INFO("*** Use only 1st and 2nd jets ***");
  ATH_MSG_INFO("UseLeadingJJ=" << m_UseLeadingJJ);
  ATH_MSG_INFO("*** e/gamma/tau/ matching ***");
  ATH_MSG_INFO("LGMinPt=" << m_LGMinPt);
  ATH_MSG_INFO("LGMaxEta=" << m_LGMaxEta);
  ATH_MSG_INFO("DeltaRJLG=" << m_DeltaRJLG);
  ATH_MSG_INFO("RatioPtJLG=" << m_RatioPtJLG);

  if (m_Jet1MinPt >= 0.) {
    if (m_Jet1MinPt < m_JetMinPt || m_Jet1MaxEta < m_JetMaxEta ) {
      ATH_MSG_ERROR("Inconsistent filter condition (Jet1)");
      return StatusCode::FAILURE;
    }
  }

  if (m_Jet2MinPt >= 0.) {
    if (m_Jet2MinPt < m_JetMinPt ||	m_Jet2MaxEta < m_JetMaxEta) {
      ATH_MSG_ERROR("Inconsistent filter condition (Jet2)");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode VBFForwardJetsFilter::filterEvent() {
  const xAOD::JetContainer* truthjetTES;
  CHECK(evtStore()->retrieve(truthjetTES, m_TruthJetContainerName));
  ATH_MSG_DEBUG("xAOD::JetContainer size = " << truthjetTES->size());

  // Get MCTruth Photon/Electon/Tau(HadronicDecay)
  std::vector<HepMC::GenParticlePtr> MCTruthPhotonList;
  std::vector<HepMC::GenParticlePtr> MCTruthElectronList;
  std::vector<CLHEP::HepLorentzVector>   MCTruthTauList;
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for (auto pitr: ((HepMC::GenEvent*)genEvt)->particles()) {
      // photon
      if ( pitr->pdg_id() == 22 && pitr->status() == 1 &&
           pitr->momentum().perp() >= m_LGMinPt && std::abs(pitr->momentum().pseudoRapidity()) <= m_LGMaxEta) {
        MCTruthPhotonList.push_back(pitr);
        ATH_MSG_INFO("photon pt(Gaudi::Units::GeV) = " << pitr->momentum().perp()/Gaudi::Units::GeV << " eta = " << pitr->momentum().pseudoRapidity());
      }
      // electon
      if ( std::abs(pitr->pdg_id()) == 11 && pitr->status() == 1 &&
           pitr->momentum().perp() >= m_LGMinPt && std::abs(pitr->momentum().pseudoRapidity()) <= m_LGMaxEta) {
        MCTruthElectronList.push_back(pitr);
        ATH_MSG_INFO("electron pt(Gaudi::Units::GeV) = " << pitr->momentum().perp()/Gaudi::Units::GeV << " eta = " << pitr->momentum().pseudoRapidity());
      }
      // tau
      if ( std::abs(pitr->pdg_id()) == 15 && pitr->status() != 3 ) {
        auto tau = pitr;
        int leptonic = 0;
        for (auto beg: tau->end_vertex()->particles_out() ) {
          if ( beg->production_vertex() != tau->end_vertex() ) continue;
          if ( std::abs( beg->pdg_id() ) == 12 ) leptonic = 1;
          if ( std::abs( beg->pdg_id() ) == 14 ) leptonic = 2;
          if ( std::abs( beg->pdg_id() ) == 15 ) leptonic = 11;
        }

        if (leptonic == 0) {
          CLHEP::HepLorentzVector nutau = sumDaughterNeutrinos( tau );
          CLHEP::HepLorentzVector tauvis = CLHEP::HepLorentzVector(tau->momentum().px()-nutau.px(),
                                                          tau->momentum().py()-nutau.py(),
                                                          tau->momentum().pz()-nutau.pz(),
                                                          tau->momentum().e()-nutau.e());
          if (tauvis.vect().perp() >= m_LGMinPt && std::abs(tauvis.vect().pseudoRapidity()) <= m_LGMaxEta) {
            MCTruthTauList.push_back(tauvis);
            ATH_MSG_INFO("had-tau pt(CLHEP::GeV) = " << tauvis.vect().perp()/CLHEP::GeV << " eta = " << tauvis.vect().pseudoRapidity());
          } 
        }
      }
    }
#else
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      // photon
      if ( (*pitr)->pdg_id() == 22 && (*pitr)->status() == 1 &&
           (*pitr)->momentum().perp() >= m_LGMinPt && std::abs((*pitr)->momentum().pseudoRapidity()) <= m_LGMaxEta) {
        MCTruthPhotonList.push_back((*pitr));
        ATH_MSG_INFO("photon pt(Gaudi::Units::GeV) = " << (*pitr)->momentum().perp()/Gaudi::Units::GeV << " eta = " << (*pitr)->momentum().pseudoRapidity());
      }
      // electon
      if ( abs((*pitr)->pdg_id()) == 11 && (*pitr)->status() == 1 &&
           (*pitr)->momentum().perp() >= m_LGMinPt && std::abs((*pitr)->momentum().pseudoRapidity()) <= m_LGMaxEta) {
        MCTruthElectronList.push_back((*pitr));
        ATH_MSG_INFO("electron pt(Gaudi::Units::GeV) = " << (*pitr)->momentum().perp()/Gaudi::Units::GeV << " eta = " << (*pitr)->momentum().pseudoRapidity());
      }
      // tau
      if ( std::abs((*pitr)->pdg_id()) == 15 && (*pitr)->status() != 3 ) {
        HepMC::GenParticle *tau = (*pitr);
        HepMC::GenVertex::particles_out_const_iterator begin = tau->end_vertex()->particles_out_const_begin();
        HepMC::GenVertex::particles_out_const_iterator end = tau->end_vertex()->particles_out_const_end();
        int leptonic = 0;
        for ( ; begin != end; begin++ ) {
          if ( (*begin)->production_vertex() != tau->end_vertex() ) continue;
          if ( abs( (*begin)->pdg_id() ) == 12 ) leptonic = 1;
          if ( abs( (*begin)->pdg_id() ) == 14 ) leptonic = 2;
          if ( abs( (*begin)->pdg_id() ) == 15 ) leptonic = 11;
        }

        if (leptonic == 0) {
          CLHEP::HepLorentzVector nutau = sumDaughterNeutrinos( tau );
          CLHEP::HepLorentzVector tauvis = CLHEP::HepLorentzVector(tau->momentum().px()-nutau.px(),
                                                          tau->momentum().py()-nutau.py(),
                                                          tau->momentum().pz()-nutau.pz(),
                                                          tau->momentum().e()-nutau.e());
          if (tauvis.vect().perp() >= m_LGMinPt && std::abs(tauvis.vect().pseudoRapidity()) <= m_LGMaxEta) {
            MCTruthTauList.push_back(tauvis);
            ATH_MSG_INFO("had-tau pt(CLHEP::GeV) = " << tauvis.vect().perp()/CLHEP::GeV << " eta = " << tauvis.vect().pseudoRapidity());
          } 
        }
      }
    }
#endif
  }

  // Select TruthJets
  std::vector<const xAOD::Jet*> jetList;
  for (xAOD::JetContainer::const_iterator it_truth = truthjetTES->begin(); it_truth != truthjetTES->end(); ++it_truth) {
    if ( (*it_truth)->pt() > m_JetMinPt && std::abs( (*it_truth)->eta() ) < m_JetMaxEta ) {
      jetList.push_back(*it_truth);
      ATH_MSG_INFO("jet pt(Gaudi::Units::GeV) = " << (*it_truth)->pt()/Gaudi::Units::GeV << " eta = " << (*it_truth)->eta());
    }
  }

  // Remove e/gamma/tau(had) from TruthJets
  removePseudoJets(jetList, MCTruthPhotonList, MCTruthElectronList, MCTruthTauList);
  ATH_MSG_INFO("# of jets = " << jetList.size());
  std::sort(jetList.begin(), jetList.end(), High2LowByJetClassPt());

  // Number of Jets
  int flagNJets = -1;
  if (m_NJets >= 1) flagNJets = int(jetList.size()) >= m_NJets ? 1 : 0;

  // Leading 1st jet
  int flag1stJet = -1;
  if (m_Jet1MinPt >= 0.) {
    flag1stJet = 0;
    if (jetList.size() >= 1) {
      const xAOD::Jet *j1 = jetList[0];
      if (j1->pt() > m_Jet1MinPt && std::abs(j1->eta()) < m_Jet1MaxEta) {
        flag1stJet = 1;
      }
    }
  }

  // Leading 2nd jet
  int flag2ndJet = -1;
  if (m_Jet2MinPt >= 0.) {
    flag2ndJet = 0;
    if (jetList.size() >= 2) {
      const xAOD::Jet *j2 = jetList[1];
      if (j2->pt() > m_Jet2MinPt && std::abs(j2->eta()) < m_Jet2MaxEta) {
        flag2ndJet = 1;
      }
    }
  }

  // Sign of Eta
  int flagSign = -1;
  if (m_UseOppositeSignEtaJet1Jet2) {
    flagSign = 0;
    if (jetList.size() >= 2) {
      const xAOD::Jet *j1 = jetList[0];
      const xAOD::Jet *j2 = jetList[1];
      if (j1->eta()*j2->eta() < 0.) flagSign = 1;
    }
  }

  // DeltaEta and Mass of JJ
  int flagJJ = -1;
  if (m_DeltaEtaJJ >= 0. || m_MassJJ >= 0.) {
    flagJJ = 0;
    if (jetList.size() >= 2) {
      int okDeltaEtaJJ = m_DeltaEtaJJ >= 0. ? 0 : 1;
      int okMassJJ = m_MassJJ >= 0. ? 0 : 1;
      for (unsigned i=0;i<jetList.size()-1;++i) {
        for (unsigned j=i+1;j<jetList.size();++j) {
          double dEta = std::abs(jetList[i]->eta()-jetList[j]->eta());
          double Mjj = (jetList[i]->p4()+jetList[j]->p4()).M();
          ATH_MSG_INFO("DeltaEtaJJ = " << dEta << " MassJJ(CLHEP::GeV) = " << Mjj/CLHEP::GeV << " (" << i << ", " << j << ")");
          if (okDeltaEtaJJ == 0 && dEta > m_DeltaEtaJJ) okDeltaEtaJJ = 1;
          if (okMassJJ == 0 && Mjj > m_MassJJ) okMassJJ = 1;
          if (okDeltaEtaJJ && okMassJJ) {
            flagJJ = 1;
            break;
          }
          if (m_UseLeadingJJ) break;
        }
        if (flagJJ == 1) break;
        if (m_UseLeadingJJ) break;
      }
    }
  }

  ATH_MSG_INFO("NJets  OK? : " << flagNJets);
  ATH_MSG_INFO("1stJet OK? : " << flag1stJet);
  ATH_MSG_INFO("2ndJet OK? : " << flag2ndJet);
  ATH_MSG_INFO("Sign   OK? : " << flagSign);
  ATH_MSG_INFO("JJ     OK? : " << flagJJ);

  setFilterPassed(flagNJets != 0 && flag1stJet != 0 && flag2ndJet != 0 && flagSign != 0 && flagJJ != 0);
  return StatusCode::SUCCESS;
}


CLHEP::HepLorentzVector VBFForwardJetsFilter::sumDaughterNeutrinos( HepMC::ConstGenParticlePtr part ) {
  CLHEP::HepLorentzVector nu( 0, 0, 0, 0);

  if ( ( abs( part->pdg_id() ) == 12 ) || ( abs( part->pdg_id() ) == 14 ) || ( abs( part->pdg_id() ) == 16 ) ) {
    nu.setPx(part->momentum().px());
    nu.setPy(part->momentum().py());
    nu.setPz(part->momentum().pz());
    nu.setE(part->momentum().e());
    return nu;
  }

  if ( !part->end_vertex() ) return nu;

  for (auto daughterparticle: *(part->end_vertex())) nu += sumDaughterNeutrinos( daughterparticle );
  return nu;
}


double VBFForwardJetsFilter::getMinDeltaR(const xAOD::Jet *jet, std::vector<HepMC::GenParticlePtr> &list) {
  double minDR = 999.;
  for (unsigned i=0;i<list.size();++i) {
    if (list[i]->momentum().perp() != 0.) {
      double dphi = jet->phi()-list[i]->momentum().phi();
      double deta = jet->eta()-list[i]->momentum().pseudoRapidity();
      if (dphi >  M_PI) { dphi -= 2.*M_PI; }
      if (dphi < -M_PI) { dphi += 2.*M_PI; }
      double dr = sqrt(deta*deta+dphi*dphi);
      double ratio_pt= std::abs((jet->pt()-list[i]->momentum().perp())/list[i]->momentum().perp());
      if (ratio_pt < m_RatioPtJLG && dr < minDR) minDR = dr;
    }
  }
  return minDR;
}


double VBFForwardJetsFilter::getMinDeltaR(const xAOD::Jet *jet, std::vector<CLHEP::HepLorentzVector> &list) {
  double minDR = 999.;
  for (unsigned i=0;i<list.size();++i) {
    if (list[i].vect().perp() != 0.) {
      double dphi = jet->phi()-list[i].phi();
      double deta = jet->eta()-list[i].vect().pseudoRapidity();
      if (dphi >  M_PI) { dphi -= 2.*M_PI; }
      if (dphi < -M_PI) { dphi += 2.*M_PI; }
      double dr = std::sqrt(deta*deta+dphi*dphi);
      double ratio_pt= std::abs((jet->pt()-list[i].vect().perp())/list[i].vect().perp());
      if (ratio_pt < m_RatioPtJLG && dr < minDR) minDR = dr;
    }
  }
  return minDR;
}


void VBFForwardJetsFilter::removePseudoJets( std::vector<const xAOD::Jet*> &jetList,
                                             std::vector<HepMC::GenParticlePtr> &MCTruthPhotonList,
                                             std::vector<HepMC::GenParticlePtr> &MCTruthElectronList,
                                             std::vector<CLHEP::HepLorentzVector>   &MCTruthTauList) {
  std::vector<const xAOD::Jet*> orgJetList = jetList;
  jetList.clear();
  for (unsigned i=0;i<orgJetList.size();++i) {
    const xAOD::Jet *jet = orgJetList[i];
    double dR1 = getMinDeltaR(jet,MCTruthPhotonList);
    double dR2 = getMinDeltaR(jet,MCTruthElectronList);
    double dR3 = getMinDeltaR(jet,MCTruthTauList);
    if (dR1 > m_DeltaRJLG && dR2 > m_DeltaRJLG && dR3 > m_DeltaRJLG) jetList.push_back(jet);
  }
}
