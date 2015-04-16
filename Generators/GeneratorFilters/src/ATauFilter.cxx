/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/ATauFilter.h"
#include <cmath>

//#include "HepMC/ParticleDataTable.h"
//#include "HepMC/ParticleData.h"
#include "CLHEP/Vector/LorentzVector.h"
using HepMC::GenVertex;
using HepMC::GenParticle;


ATauFilter::ATauFilter(const std::string& name, ISvcLocator* pSvcLocator )
  : GenFilter( name,pSvcLocator ),
    m_eventsaccepted(0), m_eventsrefused(0), m_eventsll(0),
    m_eventslh(0), m_eventshh(0),  m_eventsllacc(0), 
    m_eventslhacc(0), m_eventshhacc(0)
{
  declareProperty( "Etacut", m_EtaRange = 2.5 );
  // pt cuts for a0 -> tau + atau -> l + l + X
  declareProperty( "llPtcute", m_llPtmine = 5000.0 );
  declareProperty( "llPtcutmu", m_llPtminmu = 13000.0 );
  // pt cuts for a0 -> tau + atau -> l + h + X
  declareProperty( "lhPtcute", m_lhPtmine = 5000.0 );
  declareProperty( "lhPtcutmu", m_lhPtminmu = 13000.0 );
  declareProperty( "lhPtcuth", m_lhPtminh = 12000.0 );
  // pt cuts for a0 -> tau + atau -> h + h + X
  declareProperty( "hhPtcut", m_hhPtmin = 12000.0 );
  // dphi cuts for a0 -> tau(x) + atau(y) -> x + y + X, dphi(x, y) < maxdphi
  declareProperty( "maxdphi", m_maxdphi = 1000.0 );
}

StatusCode ATauFilter::filterInitialize() {
  m_eventsaccepted = 0;
  m_eventsrefused = 0;
  m_eventsll = 0;
  m_eventslh = 0;
  m_eventshh = 0;
  m_eventsllacc = 0;
  m_eventslhacc = 0;
  m_eventshhacc = 0;
  return StatusCode::SUCCESS;
}


StatusCode ATauFilter::filterFinalize() {
  ATH_MSG_INFO("eventsrefused: " << m_eventsrefused << " eventsaccepted: " << m_eventsaccepted <<
               " ll: " << m_eventsll << " lh: " << m_eventslh << " hh: " << m_eventshh <<
               " llacc: " << m_eventsllacc << " lhacc: " << m_eventslhacc << " hhacc: " << m_eventshhacc);
  return StatusCode::SUCCESS;
}


StatusCode ATauFilter::filterEvent() {
  GenParticle *tau;
  GenParticle *atau;
  GenParticle *nutau;
  GenParticle *anutau;
  GenParticle *taulep;
  GenParticle *ataulep;

  CLHEP::HepLorentzVector mom_tauprod;   // will contain the momentum of the products of the tau decay
  CLHEP::HepLorentzVector mom_atauprod;  //   calculated with mom_tauprod = mom_tau - mom_nutau

  tau = 0;               // will contain the tau once found by id only
  atau = 0;
  taulep = 0;            // lepton that has the genvertex in common with the endvertex of tau
  ataulep = 0;
  nutau = 0;             // nutau that has the genvertex in common with the endvertex of tau
  anutau = 0;

  m_eventsaccepted++;

  int fsr;
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr ) {
      // Look for the first tau with genstat != 3 which has not a tau as daughter
      fsr = 0;
      if ( (*pitr)->end_vertex()!= 0 ) {
        HepMC::GenVertex::particles_out_const_iterator  itr = (*pitr)->end_vertex()->particles_out_const_begin();
        for ( ; itr != (*pitr)->end_vertex()->particles_out_const_end(); itr++ ) {
          if ( (*pitr)->pdg_id() == (*itr)->pdg_id() ) fsr = 1;
        }
      }

      if ( ( (*pitr)->pdg_id() == 15 ) && ( tau == 0 ) && ( (*pitr)->status() != 3 ) && ( fsr == 0 ) ) {
        tau = (*pitr);
        ATH_MSG_DEBUG("Found tau with barcode " << tau->barcode() << " status " << (*pitr)->status());
      }

      // Look for the first atau with genstat != 3 which has not a tau as daughter
      if ( ( (*pitr)->pdg_id() == -15 ) && ( atau == 0 ) && ( (*pitr)->status() != 3 ) && ( fsr == 0 ) ) {
        atau = (*pitr);
        ATH_MSG_DEBUG("Found atau with barcode " << atau->barcode() << " status " << (*pitr)->status());
      }

      // If tau was already found look for his nu
      if ( ( tau != 0 ) && ( (*pitr)->pdg_id() == 16 ) ) {
        if ( tau->end_vertex() == (*pitr)->production_vertex() ) {
          ATH_MSG_DEBUG("Found nutau with barcode " << (*pitr)->barcode() << " and pdg_id " <<
                        (*pitr)->pdg_id() << " pt=" << (*pitr)->momentum().perp());
          nutau = (*pitr);
        }
      }

      // If atau was already found look for his anu
      if ( ( atau != 0 ) && ( (*pitr)->pdg_id() == -16 ) ) {
        if ( atau->end_vertex() == (*pitr)->production_vertex() ) {
          ATH_MSG_DEBUG("Found anutau with barcode " << (*pitr)->barcode() << " and pdg_id " <<
                        (*pitr)->pdg_id() << " pt=" << (*pitr)->momentum().perp());
          anutau = (*pitr);
        }
      }

      // if tau was already found look for his lepton
      if ( ( tau != 0 ) && ( ( (*pitr)->pdg_id() == 13 ) || ( (*pitr)->pdg_id() == 11 ) ) ) {
        if ( tau->end_vertex() == (*pitr)->production_vertex() ) {
          ATH_MSG_DEBUG("Found tau-lepton with barcode " << (*pitr)->barcode() << " and pdg_id " <<
                        (*pitr)->pdg_id() << " pt=" << (*pitr)->momentum().perp());
          taulep = (*pitr);
        }
      }

      // If atau was already found look for his alepton
      if ( ( atau != 0 ) && ( ( (*pitr)->pdg_id() == -13 ) || ( (*pitr)->pdg_id() == -11 ) ) ) {
        if ( atau->end_vertex() == (*pitr)->production_vertex() ) {
          ATH_MSG_DEBUG("Found atau-lepton with barcode "  << (*pitr)->barcode() << " and pdg_id " <<
                        (*pitr)->pdg_id() << " pt=" << (*pitr)->momentum().perp());
          ataulep = (*pitr);
        }
      }
    }
  }

  // tau == 0 (atau == 0) mean that we did not find a tau (atau)
  if ( tau == 0 ) {
    ATH_MSG_DEBUG("No tau found!");
    return StatusCode::SUCCESS;
  }
  if ( atau == 0 ) {
    ATH_MSG_DEBUG("No atau found!");
    return StatusCode::SUCCESS;
  }
  if ( nutau == 0 ) {
    ATH_MSG_DEBUG("No nutau found !");
    return StatusCode::SUCCESS;
  }
  if ( anutau == 0 ) {
    ATH_MSG_DEBUG("No anutau found !");
    return StatusCode::SUCCESS;
  }

  // tau -> X + nu, calculate the momentum of X from p_X = p_tau - p_nu
  mom_tauprod.setPx(tau->momentum().px()-nutau->momentum().px());
  mom_tauprod.setPy(tau->momentum().py()-nutau->momentum().py());
  mom_tauprod.setPz(tau->momentum().pz()-nutau->momentum().pz());
  mom_tauprod.setE(tau->momentum().e()-nutau->momentum().e());

  mom_atauprod.setPx(atau->momentum().px() - anutau->momentum().px());
  mom_atauprod.setPy(atau->momentum().py() - anutau->momentum().py());
  mom_atauprod.setPz(atau->momentum().pz() - anutau->momentum().pz());
  mom_atauprod.setE(atau->momentum().e() - anutau->momentum().e());

  // Now we can classify the event by looking up what decay products we found in the particle list

  // tau -> lepton + nu + anu, atau -> alepton + anu + nu
  if ( ( taulep != 0 ) && ( ataulep != 0 ) ) {
    ATH_MSG_DEBUG("ll event");
    m_eventsll++;
    if ( ( ( ( taulep->momentum().perp() >= m_llPtmine ) && ( taulep->pdg_id() == 11 ) )
           || ( ( taulep->momentum().perp() >= m_llPtminmu ) && ( taulep->pdg_id() == 13 ) ) )
         && ( ( ( ataulep->momentum().perp() >= m_llPtmine ) && ( ataulep->pdg_id() == -11 ) )
              || ( ( ataulep->momentum().perp() >= m_llPtminmu ) && ( ataulep->pdg_id() == -13 ) ) )
         && ( ( fabs( taulep->momentum().pseudoRapidity() ) <= m_EtaRange ) )
         && ( fabs( ataulep->momentum().pseudoRapidity() ) <= m_EtaRange ) ) {
      CLHEP::HepLorentzVector mom_taulep(taulep->momentum().px(), taulep->momentum().py(), taulep->momentum().pz(), taulep->momentum().e());
      CLHEP::HepLorentzVector mom_ataulep(ataulep->momentum().px(), ataulep->momentum().py(), ataulep->momentum().pz(), ataulep->momentum().e());
      if ( fabs(mom_taulep.vect().deltaPhi(mom_ataulep.vect())) < m_maxdphi ) {
        m_eventsllacc++;
        return StatusCode::SUCCESS;
      }
    }
  }

  // tau -> lepton + nu + anu, atau -> had + anu + nu || cc
  if ( ( taulep != 0 ) && ( ataulep == 0 ) ) {
    ATH_MSG_DEBUG("lh event tauprod.pt=" << mom_atauprod.perp() << " eta=" << mom_tauprod.eta());
    m_eventslh++;
    if ( ( ( ( taulep->momentum().perp() >= m_lhPtmine ) && ( taulep->pdg_id() == 11 ) )
           || ( ( taulep->momentum().perp() >= m_lhPtminmu ) && ( taulep->pdg_id() == 13 ) ) )
         && ( mom_atauprod.perp() >= m_lhPtminh )
         && ( fabs( taulep->momentum().pseudoRapidity() ) <= m_EtaRange )
         && ( fabs( mom_atauprod.pseudoRapidity() ) <= m_EtaRange ) ) {
      CLHEP::HepLorentzVector mom_taulep(taulep->momentum().px(), taulep->momentum().py(), taulep->momentum().pz(), taulep->momentum().e());
      if ( fabs(mom_taulep.vect().deltaPhi(mom_atauprod.vect())) < m_maxdphi ) {
        m_eventslhacc++;
        return StatusCode::SUCCESS;
      }
    }
  }

  if ( ( ataulep != 0 ) && ( taulep == 0 ) ) {
    ATH_MSG_DEBUG("lh event tauprod.pt=" << mom_tauprod.perp() << " eta=" << mom_tauprod.eta());
    m_eventslh++;
    if ( ( ( ( ataulep->momentum().perp() >= m_lhPtmine ) && ( ataulep->pdg_id() == -11 ) )
           || ( ( ataulep->momentum().perp() >= m_lhPtminmu ) && ( ataulep->pdg_id() == -13 ) ) )
         && ( mom_tauprod.perp() >= m_lhPtminh )
         && ( fabs( mom_tauprod.pseudoRapidity() ) <= m_EtaRange )
         && ( fabs( ataulep->momentum().pseudoRapidity() ) <= m_EtaRange ) ) {
      CLHEP::HepLorentzVector mom_ataulep(ataulep->momentum().px(), ataulep->momentum().py(), ataulep->momentum().pz(), ataulep->momentum().e());
      if ( fabs(mom_ataulep.vect().deltaPhi(mom_tauprod.vect())) < m_maxdphi ) {
        m_eventslhacc++;
        return StatusCode::SUCCESS;
      }
    }
  }

  // tau -> had + anu + nu, atau -> had + nu + anu
  if ( ( taulep == 0 ) && ( ataulep == 0 ) ) {
    ATH_MSG_DEBUG("hh event tauprod.pt=" << mom_tauprod.perp() <<
                  ", atauprod.pt=" << mom_atauprod.perp() <<
                  " eta tauprod=" << mom_tauprod.eta() <<
                  " eta atauprod=" << mom_atauprod.eta());
    m_eventshh++;
    if ( ( mom_tauprod.perp() >= m_hhPtmin )                                  // check pt
         && ( mom_atauprod.perp() >= m_hhPtmin )
         && ( fabs( mom_tauprod.pseudoRapidity() ) <= m_EtaRange )            // check eta-range
         && ( fabs( mom_atauprod.pseudoRapidity() ) <= m_EtaRange ) ) {
      if ( fabs(mom_atauprod.vect().deltaPhi(mom_tauprod.vect())) < m_maxdphi ) {
        m_eventshhacc++;
        m_nPass++;
        return StatusCode::SUCCESS;
      }
    }
  }

  // If we get here the event did not pass the filter
  ATH_MSG_DEBUG("Event failed");

  m_nFail++;
  setFilterPassed(false);
  m_eventsaccepted--; // since we counted the event already as accepted
  m_eventsrefused++;

  return StatusCode::SUCCESS;
}
