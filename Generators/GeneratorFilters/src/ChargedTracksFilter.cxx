/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
}


StatusCode ChargedTracksFilter::filterEvent() {
  int nChargedTracks = 0;

  // Loop over all events in McEventCollection
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;

  for (auto part: *genEvt){
      // We only care about stable particles
      if (!MC::isGenStable(part)) continue;

      // Particle's charge
      int pID = part->pdg_id();
      double pCharge = MC::PID::charge(pID);

      // Count tracks in specified acceptance
      const double pT = part->momentum().perp();
      const double eta = part->momentum().pseudoRapidity();
      if (pT >= m_Ptmin && std::abs(eta) <= m_EtaRange && pCharge != 0) {
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
                " minNTracks = " << m_NTracks);

  // Record passed status
  setFilterPassed(nChargedTracks > m_NTracks);
  return StatusCode::SUCCESS;
}
