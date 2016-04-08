/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StaticNavigationEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkExEngine/StaticNavigationEngine.h"

// constructor
Trk::StaticNavigationEngine::StaticNavigationEngine(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_propagationEngine(""),
  m_materialEffectsEngine(""),
  m_trackingGeometry(0),
  m_trackingGeometryName("AtlasTrackingGeometry")
  
{
    declareInterface<Trk::INavigationEngine>(this);
    // Tools needed
    declareProperty("PropagationEngine"                     , m_propagationEngine);
    declareProperty("MaterialEffectsEngine"                 , m_materialEffectsEngine);
    // The TrackingGeometry
    declareProperty("TrackingGeometry"                      , m_trackingGeometryName);
    // steering of the screen outoput (SOP)
    declareProperty("OutputPrefix"                          , m_sopPrefix);
    declareProperty("OutputPostfix"                         , m_sopPostfix);
}

// destructor
Trk::StaticNavigationEngine::~StaticNavigationEngine()
{}

// the interface method initialize
StatusCode Trk::StaticNavigationEngine::initialize()
{
    
    if (m_propagationEngine.retrieve().isFailure()){
        EX_MSG_FATAL("", "initialize", "", "failed to retrieve propagation engine '"<< m_propagationEngine << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG("", "initialize", "", "successfully retrieved '" << m_propagationEngine << "'." );
    
    if (m_materialEffectsEngine.retrieve().isFailure()){
        EX_MSG_FATAL("", "initialize", "", "failed to retrieve material effect engine '"<< m_materialEffectsEngine << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG("", "initialize", "", "successfully retrieved '" << m_materialEffectsEngine << "'." );
    EX_MSG_DEBUG("", "initialize", "", "successful" );
    return StatusCode::SUCCESS;
}    

// the interface method finalize
StatusCode Trk::StaticNavigationEngine::finalize()
{    
    EX_MSG_DEBUG("", "finalize", "", "successful" );    
    return StatusCode::SUCCESS;
}

/** charged situation */
Trk::ExtrapolationCode Trk::StaticNavigationEngine::resolveBoundary(Trk::ExCellCharged& ecCharged, PropDirection dir) const
{ return resolveBoundaryT<Trk::TrackParameters>(ecCharged,dir); }

/** charged situation */
Trk::ExtrapolationCode Trk::StaticNavigationEngine::resolveBoundary(Trk::ExCellNeutral& ecNeutral, PropDirection dir) const
{ return resolveBoundaryT<Trk::NeutralParameters>(ecNeutral,dir); }

StatusCode Trk::StaticNavigationEngine::updateTrackingGeometry() const {
    // retrieve the TrackingGeometry from the detector store 
    if (detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName).isFailure()){
        EX_MSG_FATAL( "", "updateGeo", "", "Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore." );
        EX_MSG_FATAL( "", "updateGeo", "", "  - probably the chosen layout is not supported / no cool tag exists. "                     );
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

