/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MissingEtFilter.h"
#include "TruthUtils/HepMCHelpers.h"


MissingEtFilter::MissingEtFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("MEtcut",m_MEtmin = 10000.);
}


StatusCode MissingEtFilter::filterEvent() {
  double sumx(0), sumy(0);
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {

      // Check that barcode <= 100K and has no end vertex
      /// eA Use truth helper functions instead
      //      if ((*pitr)->barcode() > 100000 || (*pitr)->end_vertex() != 0) continue;
      //     if ((*pitr)->status() != 1) continue;
      if (!MC::isGenStable(*pitr)) continue;

      // Stable neutrinos
      if (abs((*pitr)->pdg_id()) == 12 || abs((*pitr)->pdg_id()) == 14 || abs((*pitr)->pdg_id()) == 16) {
        ATH_MSG_DEBUG("Found neutrino: ID = " << (*pitr)->pdg_id() << " PX = " << (*pitr)->momentum().x() << " PY = "<< (*pitr)->momentum().y());
        sumx += (*pitr)->momentum().x();
        sumy += (*pitr)->momentum().y();
      }

      // Stable LSP's
      else if (abs((*pitr)->pdg_id()) == 1000022) {
        double ex = (*pitr)->momentum().t() * ( (*pitr)->momentum().x() / ((*pitr)->momentum()).rho() );
        double ey = (*pitr)->momentum().t() * ( (*pitr)->momentum().y() / ((*pitr)->momentum()).rho() );
        ATH_MSG_DEBUG("Found LSP: " <<
                      " PX = " << (*pitr)->momentum().x() << " PY = "<< (*pitr)->momentum().y() <<
                      " EX = " << ex << " EY = " << ey);
        sumx += ex;
        sumy += ey;
      }

    }
  }

  // Now see what the total missing Et is and compare to minimum
  double sumEt = sqrt(sumx*sumx + sumy*sumy);
  ATH_MSG_DEBUG("Totals for event: EX = " << sumx << ", EY = "<< sumy << ", ET = " << sumEt);
  setFilterPassed(sumEt >= m_MEtmin);
  return StatusCode::SUCCESS;
}
