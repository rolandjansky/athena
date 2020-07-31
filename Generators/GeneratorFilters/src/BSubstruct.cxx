/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/BSubstruct.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "xAODJet/JetContainer.h"
#include "TH1F.h"

#include <cmath>
#include <set>


BSubstruct::BSubstruct(const std::string &name, ISvcLocator *pSvcLocator)
  : GenFilter(name, pSvcLocator),
    m_nEvents(0), m_nAccepted(0), m_nRejected(0),
    m_doJetMaxPTCut(false), m_doHeavyHadrons(false), /*m_drMin(0.),*/
    m_h_nBPass(0), m_h_nBAll(0), m_h_dRPass(0), m_h_dRAll(0),
    m_h_dPhiPass(0), m_h_dPhiAll(0), m_h_bPhiPass(0)
{
  declareProperty("JetKey", m_jetContainerName="AntiKt4TruthJets");
  declareProperty("pTMin", m_jetPTMin=180.*Gaudi::Units::GeV);
  declareProperty("pTMax", m_jetPTMax=-1.*Gaudi::Units::GeV);
  declareProperty("filterB", m_doBHadrons=true);
  declareProperty("filterC", m_doCHadrons=true);
  declareProperty("dRMatch", m_drMatch = 1.2);
  declareProperty("doHistos", m_doHistos = false);
}


inline bool BSubstruct::hasCBQuark(int pdgID) const {
  // Fundamental particles have id <= 100.  None of them are b flavoured hadrons!
  if (abs(pdgID) <= 100) return false;

  // Any id over 10000000 is not part of the PDG numbering scheme
  if (abs(pdgID) > 10000000) return false;

  // These are the digits of a number, e.g. 1*digits[1] + 2*digits[2] + 3*digits[3] = 321
  static int digits[] = {0, 1, 10, 100, 1000};
  // Check the 4th, 3rd and 2nd digit for the presence of 5
  for (size_t pos = 4; pos !=1 ; --pos) {
    int digit = (abs(pdgID) / digits[pos]) % 10;
    if (m_doCHadrons && digit == 4) return true;
    if (m_doBHadrons && digit == 5) return true;
  }
  return false;
}


inline double BSubstruct::deltaPhi(double phi1, double phi2) const {
  double dPhi = std::abs(fmod(phi1 - phi2, 2*M_PI));
  if (dPhi > M_PI) {
    dPhi = 2*M_PI - dPhi;
  }
  return dPhi;
}


inline double BSubstruct::deltaR(double y1, double phi1, double y2, double phi2) const {
  double dy = y1 - y2;
  double dPhi = deltaPhi(phi1, phi2);
  return sqrt(dy*dy + dPhi*dPhi);
}


inline  double BSubstruct::deltaR(HepMC::ConstGenParticlePtr particle1, HepMC::ConstGenParticlePtr  particle2) const {
  double rap1 = 0.5 * log((particle1->momentum().e() + particle1->momentum().pz()) /
                          (particle1->momentum().e() - particle1->momentum().pz()));
  double rap2 = 0.5 * log((particle2->momentum().e() + particle2->momentum().pz()) /
                          (particle2->momentum().e() - particle2->momentum().pz()));
  return deltaR(rap1, particle1->momentum().phi(), rap2, particle2->momentum().phi());
}


inline double BSubstruct::deltaR(HepMC::ConstGenParticlePtr  particle, const xAOD::Jet* jet) const {
  // GenParticle does not provide rapidity (only pseudo-rapidity)
  // Since we are likely dealing with massive b-hadrons and massive jets
  // and the jet-clustering alg uses rapidity, I think we should use that
  double particleRap = 0.5 * log((particle->momentum().e() + particle->momentum().pz()) /
                                 (particle->momentum().e() - particle->momentum().pz()));
  return deltaR(particleRap, particle->momentum().phi(), jet->rapidity(), jet->phi());
}


inline double BSubstruct::deltaR(const xAOD::Jet* jet1, const xAOD::Jet* jet2) const {
  return deltaR(jet1->rapidity(), jet1->phi(), jet2->rapidity(), jet2->phi());
}


inline std::vector<HepMC::ConstGenParticlePtr> BSubstruct::ancestorCBs(HepMC::ConstGenParticlePtr  p) const {
  std::vector<HepMC::ConstGenParticlePtr> parentBs;
  auto vtx = p->production_vertex();

  // If the particle has no production vertex then can only assume it is beam or similar
  // therefore return empty set of parents.
  if (vtx == 0) return parentBs;

  // If the production vertex is the first one then there's nothing more to do
  if (vtx->id() == 1) return parentBs;
#ifdef HEPMC3
  for (auto parent: vtx->particles_in()) {
    if (hasCBQuark(parent->pdg_id())) parentBs.push_back(parent);
    std::vector<HepMC::ConstGenParticlePtr> ancestors = ancestorCBs(parent);
    parentBs.insert(parentBs.end(), ancestors.begin(), ancestors.end());
  }
#else

  for (HepMC::GenVertex::particles_in_const_iterator parent = vtx->particles_in_const_begin();
       parent != vtx->particles_in_const_end(); ++parent) {
    if (hasCBQuark((*parent)->pdg_id())) parentBs.push_back(*parent);
    std::vector<HepMC::ConstGenParticlePtr> ancestors = ancestorCBs(*parent);
    parentBs.insert(parentBs.end(), ancestors.begin(), ancestors.end());
  }
#endif  
  return parentBs;
}


void BSubstruct::fillHistos(TH1* phiHisto, TH1* rHisto, TH1* nHisto, TH1* bHisto,
                            const std::vector<HepMC::ConstGenParticlePtr>& bHadrons, double weight) {
  if (!m_doHistos) return;

  for (auto bHad1 = bHadrons.begin(); bHad1 != bHadrons.end(); ++bHad1) {
    if (bHisto != 0) {
      bHisto->Fill((*bHad1)->momentum().phi(), weight);
    }
    auto bHad2 = bHad1;
    ++bHad2;
    while (bHad2 != bHadrons.end()) {
      const double dPhi = deltaPhi((*bHad1)->momentum().phi(), (*bHad2)->momentum().phi());
      phiHisto->Fill(dPhi, weight);
      const double dr = deltaR(*bHad1, *bHad2);
      rHisto->Fill(dr, weight);
      ++bHad2;
    }
  }
  nHisto->Fill(bHadrons.size(), weight);
}


StatusCode BSubstruct::filterInitialize() {
  m_doHeavyHadrons = m_doBHadrons || m_doCHadrons;
  m_doJetMaxPTCut = (m_jetPTMax > 0);

  if (m_doHistos) {
    ITHistSvc *histSvc = 0;
    CHECK(service("THistSvc", histSvc));
    if (histSvc == 0) return StatusCode::FAILURE; // Double check!

    m_h_nBPass = new TH1F("nBPass", "No. of b-hadrons", 10, 0., 10.);
    m_h_nBAll  = new TH1F("nBAll", "No. of b-hadrons", 10, 0., 10.);
    CHECK(histSvc->regHist("/filter/nBPass", m_h_nBPass));
    CHECK(histSvc->regHist("/filter/nBAll", m_h_nBAll));

    m_h_dPhiPass = new TH1F("dPhiPass", "\\Delta\\phi between b-hadrons", 20, 0., M_PI);
    m_h_dPhiAll = new TH1F("dPhiAll", "\\Delta\\phi between b-hadrons", 20, 0., M_PI);
    CHECK(histSvc->regHist("/filter/dPhiPass", m_h_dPhiPass));
    CHECK(histSvc->regHist("/filter/dPhiAll", m_h_dPhiAll));

    m_h_dRPass = new TH1F("dRPass", "\\DeltaR between b-hadrons", 20, 0., 4.);
    m_h_dRAll = new TH1F("dRAll", "\\DeltaR between b-hadrons", 20, 0., 4.);
    CHECK(histSvc->regHist("/filter/dRPass", m_h_dRPass));
    CHECK(histSvc->regHist("/filter/dRAll", m_h_dRAll));

    m_h_bPhiPass = new TH1F("bPhiPass", "\\phi of b-hadrons", 64, -2.*M_PI, 2.*M_PI);
    CHECK(histSvc->regHist("/filter/bPhiPass", m_h_bPhiPass));
  }
  return StatusCode::SUCCESS;
}


StatusCode BSubstruct::filterEvent() {
  ++m_nEvents;
  std::vector<HepMC::ConstGenParticlePtr> bHadrons;
  for (McEventCollection::const_iterator evt = events()->begin(); evt != events()->end(); ++evt) {
    if ((*evt) == 0) { // WTF?
      ++m_nRejected;
      ATH_MSG_ERROR("HepMC::GenEvent pointer from McEventCollection is null");
      return StatusCode::FAILURE;
    }

    if (!m_doHeavyHadrons) continue;
    for (auto p: **evt) {
      if (hasCBQuark(p->pdg_id())) bHadrons.push_back(p);
    }
  }

  // Look for c or b-hadrons that are in the same decay chain as each other and remove the first one
  for (auto bHad = bHadrons.rbegin(); bHad != bHadrons.rend(); ++bHad) {
    if ((*bHad) == 0) continue;
    std::vector<HepMC::ConstGenParticlePtr> ancestors = ancestorCBs(*bHad);
    for (auto ancestor = ancestors.begin(); ancestor != ancestors.end(); ++ancestor) {
      for (auto p = bHadrons.begin(); p != bHadrons.end(); ++p) {
        if((*p)==0 || (*p)==(*bHad)) continue;
        if((*ancestor) == (*p)) (*p) = 0;
      }
    }
  }

  // Remove the null pointers and any duplicates
  auto p = bHadrons.begin();
  while (p != bHadrons.end()) {
    if ((*p)==0) {
      p = bHadrons.erase(p);
    } else {
      auto p2 = p;
      ++p2;
      while (p2 != bHadrons.end()) {
        if ((*p)==(*p2)) {
          p2 = bHadrons.erase(p2);
        } else {
          ++p2;
        }
      }
      ++p;
    }
  }

  if (m_doHeavyHadrons &&  bHadrons.size() < 2) {
    setFilterPassed(false);
    ++m_nRejected;
    return StatusCode::SUCCESS;
  }

  if (m_doHeavyHadrons) fillHistos(m_h_dPhiAll, m_h_dRAll, m_h_nBAll, 0, bHadrons, 1.);

  const xAOD::JetContainer* jets;
  StatusCode sc = evtStore()->retrieve(jets, m_jetContainerName);
  if (sc == StatusCode::FAILURE || jets == 0 || jets->size() == 0) {
    setFilterPassed(false);
    ++m_nRejected;
    return sc;
  }

  bool gotJet = false;
  for (xAOD::JetContainer::const_iterator jet = jets->begin(); jet != jets->end(); ++jet) {
    if ((*jet)->pt() < m_jetPTMin) continue;
    if (m_doJetMaxPTCut && (*jet)->pt() > m_jetPTMax) continue;
    gotJet = true;

    size_t nBs = 0;
    for (auto bHad = bHadrons.begin(); bHad != bHadrons.end(); ++bHad) {
      if (deltaR(*bHad, *jet) < m_drMatch) ++nBs;
    }

    if (nBs > 1) {
      setFilterPassed(true);
      ++m_nAccepted;
      fillHistos(m_h_dPhiPass, m_h_dRPass, m_h_nBPass, m_h_bPhiPass, bHadrons, 1.);
      return StatusCode::SUCCESS;
    }
  }

  if (!m_doHeavyHadrons && gotJet) {
    setFilterPassed(true);
    ++m_nAccepted;
    return StatusCode::SUCCESS;
  }

  setFilterPassed(false);
  ++m_nRejected;
  return StatusCode::SUCCESS;
}
