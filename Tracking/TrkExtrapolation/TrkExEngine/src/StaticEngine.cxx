/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StaticEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkExEngine/StaticEngine.h"

// constructor
Trk::StaticEngine::StaticEngine(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_propagationEngine(""),          
  m_navigationEngine(""),    
  m_materialEffectsEngine("")  
{
    declareInterface<Trk::IExtrapolationEngine>(this);
    // The Tools needed
    declareProperty("PropagationEngine"                     , m_propagationEngine);
    declareProperty("NavigationEngine"                      , m_navigationEngine);
    declareProperty("MaterialEffectsEngine"                 , m_materialEffectsEngine);
    // steering of the screen outoput (SOP)
    declareProperty("OutputPrefix"                          , m_sopPrefix);
    declareProperty("OutputPostfix"                         , m_sopPostfix);
}

// destructor
Trk::StaticEngine::~StaticEngine()
{}


// the interface method initialize
StatusCode Trk::StaticEngine::initialize()
{
    if (m_propagationEngine.retrieve().isFailure()){
        EX_MSG_FATAL("", "initialize", "", "failed to retrieve propagation engine '"<< m_propagationEngine << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG("", "initialize", "", "successfully  propagation engine '" << m_propagationEngine << "'." );
    
    if (m_navigationEngine.retrieve().isFailure()){
        EX_MSG_FATAL("", "initialize", "", "failed to retrieve navigation engine '"<< m_navigationEngine << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG("", "initialize", "", "successfully retrieved '" << m_navigationEngine << "'." );
            
    if (m_materialEffectsEngine.retrieve().isFailure()){
        EX_MSG_FATAL("", "initialize", "", "failed to retrieve material effect engine '"<< m_materialEffectsEngine << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG("", "initialize", "", "successfully retrieved '" << m_materialEffectsEngine << "'." );
    
    EX_MSG_DEBUG("", "initialize", "", "successful." );
    return StatusCode::SUCCESS;
}    

// the interface method finalize
StatusCode Trk::StaticEngine::finalize()
{    
    EX_MSG_DEBUG("", "finalize", "", "successful." );
    return StatusCode::SUCCESS;
}


/** charged extrapolation */
Trk::ExtrapolationCode Trk::StaticEngine::extrapolate(ExCellCharged& ecCharged,
                                           const Surface* sf,
                                           BoundaryCheck bcheck) const
{ return extrapolateT<TrackParameters>(ecCharged,sf,ecCharged.propDirection,bcheck); }


/** neutral extrapolation */
Trk::ExtrapolationCode Trk::StaticEngine::extrapolate(ExCellNeutral& ecNeutral,
                                           const Surface* sf,
                                           BoundaryCheck bcheck) const
{ return extrapolateT<NeutralParameters>(ecNeutral,sf,ecNeutral.propDirection,bcheck); }
