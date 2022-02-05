/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODChargedTracksFilter.h"
#include "TruthUtils/HepMCHelpers.h"


xAODChargedTracksFilter::xAODChargedTracksFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  /// @todo Document
  declareProperty("Ptcut", m_Ptmin = 50.0);
  declareProperty("Etacut", m_EtaRange = 2.5);
  declareProperty("NTracks", m_NTracks = 40);
}


StatusCode xAODChargedTracksFilter::filterEvent() {

      // Retrieve full TruthEventContainer container
    const xAOD::TruthEventContainer *xTruthEventContainer=NULL;
    if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
    {
        ATH_MSG_ERROR("No TruthEvent collection with name " << "TruthEvents" << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    int nChargedTracks = 0;
    for ( xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr) {
        unsigned int nPart = (*itr)->nTruthParticles();
        for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
            const xAOD::TruthParticle* part =  (*itr)->truthParticle(iPart);
            // We only care about stable particles
            if (!MC::isGenStable(part->status(),part->barcode())) continue;

            // Particle's charge
            int pID = part->pdgId();
            double pCharge = MC::PID::charge(pID);

            // Count tracks in specified acceptance
            const double pT = part->pt();
            const double eta = part->eta();
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
