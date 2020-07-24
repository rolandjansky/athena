/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DiPhotonFilter.h"
#include <vector>


// Pt High --> Low
/// @todo Move to a sorting utils module
class High2LowByGenParticleClassPt {
public:
  bool operator() (HepMC::ConstGenParticlePtr t1, HepMC::ConstGenParticlePtr t2) const {
    return t1->momentum().perp2() > t2->momentum().perp2();
  }
};


DiPhotonFilter::DiPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("PtCut1st",m_Ptmin_1st = 20000.);
  declareProperty("PtCut2nd",m_Ptmin_2nd = 15000.);
  declareProperty("PtCutOthers",m_Ptmin_others = 15000.);
  declareProperty("EtaCut1st",m_EtaRange_1st = 2.50);
  declareProperty("EtaCut2nd",m_EtaRange_2nd = 2.50);
  declareProperty("EtaCutOthers",m_EtaRange_others = 2.50);
  declareProperty("MassCutFrom",m_diphoton_massmin = -1.);
  declareProperty("MassCutTo",m_diphoton_massmax = -1.);
  declareProperty("Use1st2ndPhotons",m_use1st2ndPhotonsforMassCut = false);
}


StatusCode DiPhotonFilter::filterInitialize() {
  ATH_MSG_INFO("*** filter condition ***");
  ATH_MSG_INFO("At least two photons must be in an event.");
  ATH_MSG_INFO("PtCut for the 1st photon  = " << m_Ptmin_1st << " (CLHEP::MeV)");
  ATH_MSG_INFO("PtCut for the 2nd photon  = " << m_Ptmin_2nd << " (CLHEP::MeV)");
  ATH_MSG_INFO("PtCut for other photons   = " << m_Ptmin_others << " (CLHEP::MeV)");
  ATH_MSG_INFO("EtaCut for the 1st photon = " << m_EtaRange_1st);
  ATH_MSG_INFO("EtaCut for the 2nd photon = " << m_EtaRange_2nd);
  ATH_MSG_INFO("EtaCut for other photons  = " << m_EtaRange_others);
  ATH_MSG_INFO("MassCut(min)              = " << m_diphoton_massmin << " (CLHEP::MeV)");
  ATH_MSG_INFO("MassCut(max)              = " << m_diphoton_massmax << " (CLHEP::MeV)");
  ATH_MSG_INFO("   negative value on MassCut(min,max) -> no limit in the cut");
  ATH_MSG_INFO("Use only the 1st and the 2nd photons for mass cut, flag = " << m_use1st2ndPhotonsforMassCut);
  return StatusCode::SUCCESS;
}


StatusCode DiPhotonFilter::filterEvent() {
  // get min pt
  double ptcut = m_Ptmin_1st > m_Ptmin_2nd ? m_Ptmin_2nd : m_Ptmin_1st;
  if (ptcut > m_Ptmin_others) ptcut = m_Ptmin_others;

  ATH_MSG_DEBUG("min pt(photon) = " << ptcut << " (CLHEP::MeV)");

  // find truth photons
  std::vector<HepMC::ConstGenParticlePtr> MCTruthPhotonList;
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto part: *genEvt) {
      if ( (part->pdg_id() == 22) ) {
        if ( part->status() == 1 && (part->momentum().perp() >= ptcut) ) {
          MCTruthPhotonList.push_back(part);
        }
      }
    }
  }

  // sort truth photons with pT
  std::sort(MCTruthPhotonList.begin(), MCTruthPhotonList.end(), High2LowByGenParticleClassPt());

  // check conditions
  bool isOK = true;
  ATH_MSG_DEBUG("# of truth photons = " << MCTruthPhotonList.size());
  if (MCTruthPhotonList.size() < 2) {
    isOK = false;
  } else {
    std::vector<HepMC::ConstGenParticlePtr> MCTruthPhotonList2;
    // check pT and eta to select truth photons
    for (size_t i = 0; i < MCTruthPhotonList.size(); ++i) {
      ATH_MSG_DEBUG(i << ": pT=" << MCTruthPhotonList[i]->momentum().perp() << ", eta=" << MCTruthPhotonList[i]->momentum().pseudoRapidity());
      double ptmin  = m_Ptmin_others;
      double etamax = m_EtaRange_others;
      if (MCTruthPhotonList2.size() == 0) {
        ptmin  = m_Ptmin_1st;
        etamax = m_EtaRange_1st;
      } else if (MCTruthPhotonList2.size() == 1) {
        ptmin  = m_Ptmin_2nd;
        etamax = m_EtaRange_2nd;
      }
      if (MCTruthPhotonList[i]->momentum().perp() >= ptmin &&
          std::abs(MCTruthPhotonList[i]->momentum().pseudoRapidity()) <= etamax) {
        MCTruthPhotonList2.push_back(MCTruthPhotonList[i]);
      }
    }
    ATH_MSG_DEBUG("# of truth photons after pT and eta cut = " << MCTruthPhotonList2.size());

    // check mass
    if (MCTruthPhotonList2.size() < 2) {
      isOK = false;
    } else {
      int nGood = 0;
      if (m_use1st2ndPhotonsforMassCut) {
        double sumPx = MCTruthPhotonList2[0]->momentum().px()+MCTruthPhotonList2[1]->momentum().px();
        double sumPy = MCTruthPhotonList2[0]->momentum().py()+MCTruthPhotonList2[1]->momentum().py();
        double sumPz = MCTruthPhotonList2[0]->momentum().pz()+MCTruthPhotonList2[1]->momentum().pz();
        double sumE  = MCTruthPhotonList2[0]->momentum().e() +MCTruthPhotonList2[1]->momentum().e();
        double m2 = sumE*sumE-(sumPx*sumPx+sumPy*sumPy+sumPz*sumPz);
        double mGamGam = m2 >= 0. ? sqrt(m2) : -sqrt(-m2);

        ATH_MSG_DEBUG("mass(gamgam) = " << mGamGam << " (CLHEP::MeV)");
        if (m_diphoton_massmin >= 0. && m_diphoton_massmax >= 0.) {
          if (mGamGam >= m_diphoton_massmin && mGamGam <= m_diphoton_massmax) ++nGood;
        } else if (m_diphoton_massmin >= 0. && m_diphoton_massmax <  0.) {
          if (mGamGam >= m_diphoton_massmin) ++nGood;
        } else if (m_diphoton_massmin <  0. && m_diphoton_massmax >= 0.) {
          if (mGamGam <= m_diphoton_massmax) ++nGood;
        } else {
          ++nGood;
        }
      } else {
        for (size_t i=0;i<MCTruthPhotonList2.size()-1;++i) {
          for (size_t j=i+1;j<MCTruthPhotonList2.size();++j) {
            double sumPx = MCTruthPhotonList2[i]->momentum().px()+MCTruthPhotonList2[j]->momentum().px();
            double sumPy = MCTruthPhotonList2[i]->momentum().py()+MCTruthPhotonList2[j]->momentum().py();
            double sumPz = MCTruthPhotonList2[i]->momentum().pz()+MCTruthPhotonList2[j]->momentum().pz();
            double sumE  = MCTruthPhotonList2[i]->momentum().e() +MCTruthPhotonList2[j]->momentum().e();
            double m2 = sumE*sumE-(sumPx*sumPx+sumPy*sumPy+sumPz*sumPz);
            double mGamGam = m2 >= 0. ? sqrt(m2) : -sqrt(-m2);
            ATH_MSG_DEBUG("mass(gamgam) = " << mGamGam << " (CLHEP::MeV)");
            if (m_diphoton_massmin >= 0. && m_diphoton_massmax >= 0.) {
              if (mGamGam >= m_diphoton_massmin && mGamGam <= m_diphoton_massmax) ++nGood;
            } else if (m_diphoton_massmin >= 0. && m_diphoton_massmax <  0.) {
              if (mGamGam >= m_diphoton_massmin) ++nGood;
            } else if (m_diphoton_massmin <  0. && m_diphoton_massmax >= 0.) {
              if (mGamGam <= m_diphoton_massmax) ++nGood;
            } else {
              ++nGood;
            }
          }
        }
      }

      ATH_MSG_DEBUG("# of good photon pair = " << nGood);
      if (nGood == 0) isOK = false;
    }
  }

  ATH_MSG_DEBUG("flag(final decision) = " << isOK);
  setFilterPassed(isOK);
  return StatusCode::SUCCESS;
}
