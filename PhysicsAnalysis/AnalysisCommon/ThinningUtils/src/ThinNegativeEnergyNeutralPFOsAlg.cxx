///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinNegativeEnergyNeutralPFOsAlg::ThinNegativeEnergyNeutralPFOsAlg( const std::string& name,
                                             ISvcLocator* pSvcLocator ) :
::AthAlgorithm( name, pSvcLocator ),
m_thinningSvc( "ThinningSvc/ThinningSvc", name ),
m_doThinning(true),
m_neutralPFOsKey("JetETMissNeutralParticleFlowObjects"),
m_nEventsProcessed(0),
m_nNeutralPFOsProcessed(0),
m_nNeutralPFOsThinned(0)
{
    
    declareProperty("ThinningSvc",          m_thinningSvc,
                    "The ThinningSvc instance for a particular output stream" );
    
    declareProperty("ThinNegativeEnergyNeutralPFOs", m_doThinning,
                    "Should the thinning of negative energy neutral PFOs be run?");
   
    declareProperty("NeutralPFOsKey", m_neutralPFOsKey,
                    "StoreGate key for the PFOContainer to be thinned");

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
    ATH_MSG_DEBUG ( " using = " << m_thinningSvc );

    // Is truth thinning required?
    if (!m_doThinning) {
        ATH_MSG_INFO("Negative energy NeutralPFO thinning not required");
    } else {
        ATH_MSG_INFO("Negative energy NeutralPFOs will be thinned");
    }

    
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
    
    // Retrieve the container
    const xAOD::PFOContainer* neutralPFOs(0);
    if (evtStore()->contains<xAOD::PFOContainer>(m_neutralPFOsKey)) {
        CHECK( evtStore()->retrieve( neutralPFOs , m_neutralPFOsKey ) );
    } else {
        ATH_MSG_INFO("No PFOContainer with key "+m_neutralPFOsKey+" found. Thinning cannot be applied to this container");
        m_doThinning = false;
        return StatusCode::SUCCESS;
    }

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
    if (m_thinningSvc->filter(*neutralPFOs, mask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service failed for NeutralPFOs! ");
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}

