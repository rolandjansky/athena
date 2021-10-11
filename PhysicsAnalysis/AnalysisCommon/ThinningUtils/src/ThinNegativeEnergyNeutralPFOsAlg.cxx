///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    ATH_CHECK( m_neutralPFOsKey.initialize (m_streamName, m_doThinning && !m_neutralPFOsKey.key().empty()) );
    ATH_CHECK( m_LCNeutralPFOsKey.initialize (m_streamName, m_doThinning && !m_LCNeutralPFOsKey.key().empty()) );
    
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
    if (!m_doThinning || m_neutralPFOsKey.key().empty()) {
        return StatusCode::SUCCESS;
    }

    const EventContext& ctx = Gaudi::Hive::currentContext();

    SG::ThinningHandle<xAOD::FlowElementContainer> neutralFEs (m_neutralPFOsKey, ctx);
    std::vector<bool> mask;
    int nNeutralFEs = neutralFEs->size();
    m_nNeutralPFOsProcessed += nNeutralFEs;
    mask.assign(nNeutralFEs, false);

    for(int i=0; i<nNeutralFEs; i++){
        const xAOD::FlowElement* neutralFE = (*neutralFEs)[i];
        if(neutralFE->pt() > 0.0) mask[i] = true;
        else m_nNeutralPFOsThinned++;
    }
    neutralFEs.keep(mask);

    if(!m_LCNeutralPFOsKey.key().empty()){
        SG::ThinningHandle<xAOD::FlowElementContainer> LCNeutralFEs (m_LCNeutralPFOsKey, ctx);
        // LC PFOs are thinned based on the energy of the (EM-scale) originals
        LCNeutralFEs.keep(mask);
    }
    return StatusCode::SUCCESS;
}

