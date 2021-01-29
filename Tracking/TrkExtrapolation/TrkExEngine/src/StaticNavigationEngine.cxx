/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  m_materialEffectsEngine("")
{
    declareInterface<Trk::INavigationEngine>(this);
    // Tools needed
    declareProperty("PropagationEngine"                     , m_propagationEngine);
    declareProperty("MaterialEffectsEngine"                 , m_materialEffectsEngine);
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
    
#ifdef LEGACY_TRKGEOM
    if (!m_trackingGeometrySvc.empty()) {
       ATH_CHECK( m_trackingGeometrySvc.retrieve());
    }
#endif
    ATH_CHECK( m_trackingGeometryReadKey.initialize(!m_trackingGeometryReadKey.key().empty()) );
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

/** charged  */
Trk::ExtrapolationCode Trk::StaticNavigationEngine::resolvePosition(Trk::ExCellCharged& ecCharged, PropDirection dir, bool noLoop) const
{ return resolvePositionT<Trk::TrackParameters>(ecCharged,dir, noLoop); }

/** neutral */
Trk::ExtrapolationCode Trk::StaticNavigationEngine::resolvePosition(Trk::ExCellNeutral& ecNeutral, PropDirection dir, bool noLoop) const
{ return resolvePositionT<Trk::NeutralParameters>(ecNeutral,dir, noLoop); }

void Trk::StaticNavigationEngine::throwFailedToGetTrackingGeomtry() const {
   std::stringstream msg;
   msg << "Failed to get conditions data " << m_trackingGeometryReadKey.key() << ".";
   throw std::runtime_error(msg.str());
}
