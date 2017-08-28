///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ThinNegativeEnergyCaloClustersAlg.cxx
// Author: James Catmore <James.Catmore@cern.ch>
// based on similar code by Karsten Koeneke <karsten.koeneke@cern.ch>
// Uses thinning service to remove CaloClusters with negative energy
// Intended for use in ESD->AOD in reconstruction - use other tools
// for analysis (Expression evaluation is not used here)
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ThinNegativeEnergyCaloClustersAlg.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

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
ThinNegativeEnergyCaloClustersAlg::ThinNegativeEnergyCaloClustersAlg( const std::string& name,
                                             ISvcLocator* pSvcLocator ) :
::AthAlgorithm( name, pSvcLocator ),
m_thinningSvc( "ThinningSvc/ThinningSvc", name ),
m_doThinning(true),
m_caloClustersKey("CaloCalTopoClusters"),
m_nEventsProcessed(0),
m_nCaloClustersProcessed(0),
m_nCaloClustersThinned(0)
{
    
    declareProperty("ThinningSvc",          m_thinningSvc,
                    "The ThinningSvc instance for a particular output stream" );
    
    declareProperty("ThinNegativeEnergyCaloClusters", m_doThinning,
                    "Should the thinning of negative energy calo clusters be run?");
   
    declareProperty("CaloClustersKey", m_caloClustersKey,
                    "StoreGate key for the CaloClustersContainer to be thinned");

}

// Destructor
///////////////
ThinNegativeEnergyCaloClustersAlg::~ThinNegativeEnergyCaloClustersAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinNegativeEnergyCaloClustersAlg::initialize()
{
    ATH_MSG_DEBUG ("Initializing " << name() << "...");
    
    // Print out the used configuration
    ATH_MSG_DEBUG ( " using = " << m_thinningSvc );

    // Is truth thinning required?
    if (!m_doThinning) {
        ATH_MSG_INFO("Negative energy CaloClusters thinning not required");
    } else {
        ATH_MSG_INFO("Negative energy CaloClusters will be thinned");
    }

    
    // Initialize the counters to zero
    m_nEventsProcessed = 0;
    m_nCaloClustersProcessed = 0;
    m_nCaloClustersThinned = 0;
    
    ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
    
    return StatusCode::SUCCESS;
}



StatusCode ThinNegativeEnergyCaloClustersAlg::finalize()
{
    ATH_MSG_DEBUG ("Finalizing " << name() << "...");
    ATH_MSG_INFO("Processed " << m_nEventsProcessed << " events containing " << m_nCaloClustersProcessed << " CaloClusters");     
    ATH_MSG_INFO("Removed " << m_nCaloClustersThinned << " negative energy CaloClusters ");
    return StatusCode::SUCCESS;
}



StatusCode ThinNegativeEnergyCaloClustersAlg::execute()
{
    // Increase the event counter
    ++m_nEventsProcessed;
    
    // Is thinning required?
    if (!m_doThinning) {
        return StatusCode::SUCCESS;
    } 
    
    // Retrieve truth and vertex containers
    const xAOD::CaloClusterContainer* caloClusters(0);
    if (evtStore()->contains<xAOD::CaloClusterContainer>(m_caloClustersKey)) {
        CHECK( evtStore()->retrieve( caloClusters , m_caloClustersKey ) );
    } else {
        ATH_MSG_INFO("No CaloClusterContainer with key "+m_caloClustersKey+" found. Thinning cannot be applied for this container");
        m_doThinning = false;
        return StatusCode::SUCCESS;
    }

    // Set up masks
    std::vector<bool> mask;
    int nCaloClusters = caloClusters->size();
    m_nCaloClustersProcessed += nCaloClusters;
    mask.assign(nCaloClusters,false);
    
    // Loop over CaloClusters and update mask
    for (int i=0; i<nCaloClusters; ++i) {
        const xAOD::CaloCluster* caloCluster = (*caloClusters)[i];
        // Retain postive energy clusters
        if (caloCluster->rawE()>0.0) {mask[i] = true;}
        else {++m_nCaloClustersThinned;}
    }

    // Apply masks to thinning service
    if (m_thinningSvc->filter(*caloClusters, mask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service failed for CaloClusters! ");
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}

