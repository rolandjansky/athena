/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TauFilter.h"
#include "CLHEP/Vector/LorentzVector.h"

TauFilter::TauFilter( const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter( name,pSvcLocator ),
    m_eventse(0), m_eventsmu(0), m_eventshad(0), 
    m_eventseacc(0), m_eventsmuacc(0), m_eventshadacc(0)
{
  declareProperty( "Ntaus", m_Ntau = 1 );
  declareProperty( "EtaMaxe", m_etaMaxe = 2.5 );
  declareProperty( "EtaMaxmu", m_etaMaxmu = 2.5 );
  declareProperty( "EtaMaxhad", m_etaMaxhad = 2.5 );

  // pt cuts for a0 -> tau + atau -> l + l + X
  declareProperty( "Ptcute", m_pTmine = 12000.0 );
  declareProperty( "Ptcutmu", m_pTminmu = 12000.0 );
  declareProperty( "Ptcuthad", m_pTminhad = 12000.0 );
}


StatusCode TauFilter::filterInitialize() {
  m_eventse = 0;
  m_eventsmu = 0;
  m_eventshad = 0;
  m_eventseacc = 0;
  m_eventsmuacc = 0;
  m_eventshadacc = 0;
  return StatusCode::SUCCESS;
}


StatusCode TauFilter::filterFinalize() {
  ATH_MSG_INFO(" , e: " << m_eventse << " , mu: " << m_eventsmu << " , had: " << m_eventshad <<
               " , eacc: " << m_eventseacc << " , muacc: " << m_eventsmuacc << " , hadacc: " << m_eventshadacc);
  return StatusCode::SUCCESS;
}


CLHEP::HepLorentzVector TauFilter::sumDaughterNeutrinos( HepMC::ConstGenParticlePtr part ) {
  CLHEP::HepLorentzVector nu( 0, 0, 0, 0);
  if ( ( std::abs( part->pdg_id() ) == 12 ) || ( std::abs( part->pdg_id() ) == 14 ) || ( std::abs( part->pdg_id() ) == 16 ) ) {
    nu.setPx(part->momentum().px());
    nu.setPy(part->momentum().py());
    nu.setPz(part->momentum().pz());
    nu.setE(part->momentum().e());
  }

  if (part->end_vertex() == 0) return nu;

 for ( auto beg: *(part->end_vertex())) nu += sumDaughterNeutrinos( beg );

  return nu;
}


StatusCode TauFilter::filterEvent() {
  CLHEP::HepLorentzVector mom_tauprod;   // will contain the momentum of the products of the tau decay
  CLHEP::HepLorentzVector tauvis;
  CLHEP::HepLorentzVector nutau;
  int ntau = 0;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for ( auto  tau: *genEvt) {
      // Look for the first tau with genstat != 3
      if (std::abs(tau->pdg_id()) != 15 || tau->status() == 3)  continue;
        ATH_MSG_DEBUG("found tau with barcode " << HepMC::barcode(tau) << " status " << tau->status());
        ATH_MSG_DEBUG("pT\t\teta\tphi\tid");
        ATH_MSG_DEBUG(tau->momentum().perp() << "\t" <<
                      tau->momentum().eta() << "\t" <<
                      tau->momentum().phi() << "\t" <<
                      tau->pdg_id() << "\t");
        int leptonic = 0;
        if ( tau->production_vertex()) continue;
        for (auto beg: *(tau->end_vertex())) {
          if ( beg->production_vertex() != tau->end_vertex() ) continue;
          if ( std::abs( beg->pdg_id() ) == 12 ) leptonic = 1;
          if ( std::abs( beg->pdg_id() ) == 14 ) leptonic = 2;
          if ( std::abs( beg->pdg_id() ) == 15 ) leptonic = 11;
        }

        if (leptonic == 11) {
          ATH_MSG_DEBUG("tau has a tau as daughter - skipping");
          continue;
        }
        nutau = sumDaughterNeutrinos(tau);

        ATH_MSG_DEBUG("pT\t\teta\tphi\tlh");
        ATH_MSG_DEBUG(nutau.perp() << "\t" << nutau.eta() << "\t" << nutau.phi() << "\t" << leptonic);

        tauvis.setPx(tau->momentum().px()-nutau.px());
        tauvis.setPy(tau->momentum().py()-nutau.py());
        tauvis.setPz(tau->momentum().pz()-nutau.pz());
        tauvis.setE(tau->momentum().e()-nutau.e());

        ATH_MSG_DEBUG(tauvis.perp() << "\t" << tauvis.eta() << "\t" << tauvis.phi() << "\t" << leptonic);

        if ( leptonic == 1 ) {
          m_eventse++;
          if ( tauvis.perp() < m_pTmine ) continue;
          if ( std::abs( tauvis.eta() ) > m_etaMaxe ) continue;
          ntau++;
          m_eventseacc++;
        } else if ( leptonic == 2 ) {
          m_eventsmu++;
          if ( tauvis.perp() < m_pTminmu ) continue;
          if ( std::abs( tauvis.eta() ) > m_etaMaxmu ) continue;
          ntau++;
          m_eventsmuacc++;
        } else if ( leptonic == 0 ) {
          m_eventshad++;
          if ( tauvis.perp() < m_pTminhad ) continue;
          if ( std::abs( tauvis.eta() ) > m_etaMaxhad ) continue;
          ntau++;
          m_eventshadacc++;
        }
    }
  }

  setFilterPassed(ntau >= m_Ntau );
  return StatusCode::SUCCESS;
}
