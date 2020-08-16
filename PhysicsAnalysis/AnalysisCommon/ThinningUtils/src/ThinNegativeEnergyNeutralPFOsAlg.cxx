///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ThinNegativeEnergyNeutralPFOsAlg.cxx
// Author: Chris Young <christopher.young@cern.ch>
// based on similar code by Karsten Koeneke <karsten.koeneke@cern.ch>
// and James Catmore <James.Catmore@cern.ch>
// Uses thinning service to remove Neutral PFOs with negative energy
// Intended for use in ESD->AOD in reconstruction - use other tools
// for analysis (Expression evaluation is not used here)
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ThinNegativeEnergyNeutralPFOsAlg.h"
#include "xAODPFlow/PFOContainer.h"

// STL includes
#include <algorithm> 

// FrameWork includes
#include "StoreGate/ThinningHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinNegativeEnergyNeutralPFOsAlg::ThinNegativeEnergyNeutralPFOsAlg( const std::string& name,
                                             ISvcLocator* pSvcLocator ) :
::AthAlgorithm( name, pSvcLocator ),
m_nEventsProcessed(0),
m_nNeutralPFOsProcessed(0),
m_nNeutralPFOsThinned(0)
{
}

// Destructor
///////////////
ThinNegativeEnergyNeutralPFOsAlg::~ThinNegativeEnergyNeutralPFOsAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinNegativeEnergyNeutralPFOsAlg::initialize()
{
    ATH_MSG_DEBUG ("Initializing " << name() << "...");
    
    // Print out the used configuration
    ATH_MSG_DEBUG ( " using = " << m_streamName );

    // Is truth thinning required?
    if (!m_doThinning) {
        ATH_MSG_INFO("Negative energy NeutralPFO thinning not required");
    } else {
        ATH_MSG_INFO("Negative energy NeutralPFOs will be thinned");
    }

    if (m_doThinning && m_streamName.empty()) {
      ATH_MSG_ERROR( "StreamName property has not been initialized." );
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_neutralPFOsKey.initialize (m_streamName, m_doThinning) );
    
    // Initialize the counters to zero
    m_nEventsProcessed = 0;
    m_nNeutralPFOsProcessed = 0;
    m_nNeutralPFOsThinned = 0;
    
    ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
    
    return StatusCode::SUCCESS;
}



StatusCode ThinNegativeEnergyNeutralPFOsAlg::finalize()
{
    ATH_MSG_DEBUG ("Finalizing " << name() << "...");
    ATH_MSG_INFO("Processed " << m_nEventsProcessed << " events containing " << m_nNeutralPFOsProcessed << " NeutralPFOs");     
    ATH_MSG_INFO("Removed " << m_nNeutralPFOsThinned << " negative energy NeutralPFOs ");
    return StatusCode::SUCCESS;
}



StatusCode ThinNegativeEnergyNeutralPFOsAlg::execute()
{
    // Increase the event counter
    ++m_nEventsProcessed;
    
    // Is truth thinning required?
    if (!m_doThinning) {
        return StatusCode::SUCCESS;
    }

    const EventContext& ctx = Gaudi::Hive::currentContext();
    
    // Retrieve the container
    SG::ThinningHandle<xAOD::PFOContainer> neutralPFOs (m_neutralPFOsKey, ctx);

    // Set up masks
    std::vector<bool> mask;
    int nNeutralPFOs = neutralPFOs->size();
    m_nNeutralPFOsProcessed += nNeutralPFOs;
    mask.assign(nNeutralPFOs,false);
    
    // Loop over NeutralPFOs and update mask
    for (int i=0; i<nNeutralPFOs; ++i) {
        const xAOD::PFO* neutralPFO = (*neutralPFOs)[i];
        // Retain postive energy neutral PFOs
        if (neutralPFO->ptEM()>0.0) {mask[i] = true;}
        else {++m_nNeutralPFOsThinned;}
    }

    // Apply masks to thinning service
    neutralPFOs.keep (mask);
    
    return StatusCode::SUCCESS;
}

