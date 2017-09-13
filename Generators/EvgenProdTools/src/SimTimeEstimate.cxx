/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "EvgenProdTools/SimTimeEstimate.h"
#include "HepMC/GenEvent.h"
#include "TruthUtils/HepMCHelpers.h"

SimTimeEstimate::SimTimeEstimate(const std::string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator)
{
  declareProperty( "EtaLimit", m_etaMax = 6.0 );
  m_total_Events = 0;
  m_total_Energy = 0.;
  pidsToSkip = {12, 13, 14, 16};
}

StatusCode SimTimeEstimate::execute() 
{
    // Loop over all events in McEventCollection
    /// @todo Use C++ for(:)
    for (const auto itr : *(events_const()))
    {
        eventEnergy = 0.;  
        particleNumber = 0.;
        particleIDs.clear();
        particleEtas.clear();       
        particleEnergies.clear();       
        // Check particles
        for (HepMC::GenEvent::particle_const_iterator pitr = itr->particles_begin(); pitr != itr->particles_end(); ++pitr )
         { 
            // Only use stable particles
            if ((*pitr)->status()!=1) continue;
            // Only use particles that are interacting
            if (!(MC::isSimInteracting(*pitr))) continue;
            // Grab the momentum
            const HepMC::FourVector pmom = (*pitr)->momentum();
            // Only count particles with finite eta
            if (pmom.perp()==0 || fabs(pmom.eta())>m_etaMax) continue;
            particleEtas.push_back(pmom.eta());
            // add  ID of particle to list 
            particleIDs.push_back(abs((*pitr)->pdg_id()));
            // add energy per particle to get the distribution:
            particleEnergies.push_back(pmom.e());

            // Skip muons and neutrinos.  This should eventually be using a common
            //  "is MIP-like" function, but that's a bit tricky as for
            //  example sometimes a chargino is stable and sometimes
            //  it decays.  This algorithm will always be a little tricky 
            //  in those cases, but better to *overestimate* the sim time.
            
            if(std::find(pidsToSkip.begin(), pidsToSkip.end(), abs((*pitr)->pdg_id())) != pidsToSkip.end()) continue; 
            // Add in the total energy
            m_total_Energy += pmom.e(); 
            eventEnergy += pmom.e();
            particleNumber ++; 
        } // Loop over particles in the event
        
        //Report characterisitics of each event  
        ATH_MSG_INFO("==> EVENT INFORMATION | event number: " << m_total_Events << "| event energy: " << eventEnergy); // << " | particleNumber: " << particleNumber << " | particleIDs: " << particleIDs << " | etas: " << particleEtas << " | energies: " << particleEnergies);
    } // Loop over events in the event collection
    // One more event done!
    m_total_Events++;
    return StatusCode::SUCCESS;
}

StatusCode SimTimeEstimate::finalize()
 {
    // Reporting in!
    ATH_MSG_INFO("==> RUN INFORMATION | Processed " << m_total_Events << " events and found " << m_total_Energy << " of energy to be processed.");
    
    return StatusCode::SUCCESS;
}

#endif
