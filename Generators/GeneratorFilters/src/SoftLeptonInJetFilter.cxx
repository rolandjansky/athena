/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/SoftLeptonInJetFilter.h"
#include "TruthUtils/PIDHelpers.h"

SoftLeptonInJetFilter::SoftLeptonInJetFilter(const std::string& name, ISvcLocator* pSvcLocator)
: GenFilter(name,pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = 1000.);
  declareProperty("Etacut",m_EtaRange = 10.0);
  declareProperty("NLeptons",m_NLeptons = 0);
  declareProperty("NPartons",m_NPartons = 0);
  declareProperty("PtPartcut",m_part_Ptmin = 15.0);
  declareProperty("EtaPartcut",m_part_EtaRange = 2.5);
  declareProperty("IDPart",m_part_ID = 0);
  declareProperty("JetCone",m_jet_cone = 0.4);
}


StatusCode SoftLeptonInJetFilter::filterEvent() {
  double eta_b[10];
  double phi_b[10];
  double eta_e[30];
  double phi_e[30];
  //int    ele_pos[10];
  int    e_found[2];

  int NLeptons = 0;
  int NPartons = 0;
  for (McEventCollection::const_iterator itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
#ifdef HEPMC3
    for (auto pitr: *genEvt) {
      if ( m_NPartons == 0 ) continue;
        if (isParton(pitr)) {
          eta_b[NPartons] = pitr->momentum().pseudoRapidity();
          phi_b[NPartons] = pitr->momentum().phi();
          NPartons++;
        }
        if (isElectron(pitr)) {
          for (auto thisParent: pitr->production_vertex()->particles_in()) {
            int parentID = std::abs(thisParent->pdg_id());
            if ( MC::PID::isBottomMeson(parentID) || MC::PID::isBottomBaryon(parentID) || MC::PID::isCharmMeson(parentID) || MC::PID::isCharmBaryon(parentID) ) {
              eta_e[NLeptons] = pitr->momentum().pseudoRapidity();
              phi_e[NLeptons] = pitr->momentum().phi();
              NLeptons++;
            }
          }
        }
    }
#else
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if ( m_NPartons !=0 ) {
        if (isParton(*pitr)) {
          eta_b[NPartons] = (*pitr)->momentum().pseudoRapidity();
          phi_b[NPartons] = (*pitr)->momentum().phi();
          NPartons++;
        }
        if (isElectron(*pitr)) {
          HepMC::GenVertex::particle_iterator firstParent, lastParent, thisParent;
          firstParent = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
          lastParent  = (*pitr)->production_vertex()->particles_end(HepMC::parents);
          for (thisParent = firstParent; thisParent != lastParent++; ++thisParent) {
            int parentID = abs((*thisParent)->pdg_id());
            if ( MC::PID::isBottomMeson(parentID) || MC::PID::isBottomBaryon(parentID) || MC::PID::isCharmMeson(parentID) || MC::PID::isCharmBaryon(parentID) ) {
              eta_e[NLeptons] = (*pitr)->momentum().pseudoRapidity();
              phi_e[NLeptons] = (*pitr)->momentum().phi();
              NLeptons++;
            }
          }
        }
      }
    }
#endif    
  }

  if (NPartons == m_NPartons && NLeptons >= m_NLeptons) {
    for (int ib=0; ib < NPartons; ib++) {
      double dR = 0.;
      e_found[ib] = 0 ;
      //ele_pos[ib] = -1;

      for (int ie=0; ie < NLeptons; ie++ ) {
        double deltaEta = eta_b[ib]-eta_e[ie];
        double deltaPhi = std::abs(phi_b[ib]-phi_e[ie]);
        if (deltaPhi  > M_PI ) deltaPhi = std::abs(deltaPhi-2*M_PI);
        dR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);

        if (dR < m_jet_cone) {
          e_found[ib]++;
          //ele_pos[ib] = ie;
        }
      }
    }
  }

  if (m_NPartons != 0) {
    if (NPartons == m_NPartons && NLeptons >= m_NLeptons) {
      int is_OK = 1;
      for (int ib = 0; ib < NPartons; ib++) {
        is_OK = is_OK * e_found[ib];
      }
      // e found for each parton in event
      if (is_OK > 0)     {
        ATH_MSG_INFO("Event is accepted ");
        m_nPass++;
        return StatusCode::SUCCESS;
      }
    }
  }

  // If we get here we have failed
  m_nFail++;
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}

bool SoftLeptonInJetFilter::isElectron(HepMC::ConstGenParticlePtr p) const {
  return (std::abs(p->pdg_id())==11 && p->status()==1 &&
          p->momentum().perp() >= m_Ptmin &&
          std::abs(p->momentum().pseudoRapidity()) <= m_EtaRange );
}

bool SoftLeptonInJetFilter::isParton(HepMC::ConstGenParticlePtr p) const {
  return (std::abs(p->pdg_id()) == m_part_ID && p->status()==3 &&
          p->momentum().perp() >= m_part_Ptmin  &&
          std::abs(p->momentum().pseudoRapidity()) <= m_part_EtaRange);
}
