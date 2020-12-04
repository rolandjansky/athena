/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/ChargedTracksFilter.h"
#include "TruthUtils/HepMCHelpers.h"


ChargedTracksFilter::ChargedTracksFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  /// @todo Document
  declareProperty("Ptcut", m_Ptmin = 50.0);
  declareProperty("Etacut", m_EtaRange = 2.5);
  declareProperty("NTracks", m_NTracks = 40);
  declareProperty("NTracksMax", m_NTracksMax = -1);
}


StatusCode ChargedTracksFilter::filterEvent() {
  int nChargedTracks = 0;

  // Loop over all events in McEventCollection
  for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;

    // Loop over all particles in event
    HepMC::GenEvent::particle_const_iterator pitr;
    for (pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      // We only care about stable particles
      if (!MC::isGenStable(*pitr)) continue;

      // Particle's charge
      int pID = (*pitr)->pdg_id();
      double pCharge = MC::PID::charge(pID);

      // Count tracks in specified acceptance
      const double pT = (*pitr)->momentum().perp();
      const double eta = (*pitr)->momentum().pseudoRapidity();
      if (pT >= m_Ptmin && fabs(eta) <= m_EtaRange && pCharge != 0) {
        ATH_MSG_DEBUG("Found particle, " <<
                      " pT = " << pT <<
                      " eta = " << eta <<
                      " pdg id = " << pID <<
                      " charge = " << pCharge << " in acceptance");
        nChargedTracks += 1;
      }
    }
  }

  // Summarise event
  ATH_MSG_DEBUG("# of tracks " << nChargedTracks <<
                " with pT >= " << m_Ptmin <<
                " |eta| < " << m_EtaRange <<
                " minNTracks = " << m_NTracks <<
                " maxNTracks = " << m_NTracksMax);

  // Record passed status
  setFilterPassed( ((m_NTracksMax == -1) || (nChargedTracks <= m_NTracksMax)) && 
		   ((m_NTracks == -1) || (nChargedTracks >= m_NTracks)) );
  return StatusCode::SUCCESS;
}
