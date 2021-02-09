/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkExEngine/ExtrapolationEngine.h"
#include "TrkDetDescrUtils/GeometrySignature.h"

// constructor
Trk::ExtrapolationEngine::ExtrapolationEngine(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_propagationEngine(""),    
  m_navigationEngine(""),    
  m_forceSearchInit(false)
{
    declareInterface<Trk::IExtrapolationEngine>(this);
    // Extrapolation Engine retrieval 
    declareProperty("ExtrapolationEngines"                  , m_extrapolationEngines);    
    // The Tools needed
    declareProperty("PropagationEngine"                     , m_propagationEngine);
    declareProperty("NavigationEngine"                      , m_navigationEngine);
    // steering of the screen outoput (SOP)
    declareProperty("OutputPrefix"                          , m_sopPrefix);
    declareProperty("OutputPostfix"                         , m_sopPostfix);
    // the properties to be given 
    declareProperty("ForceSearchAtInit"                     , m_forceSearchInit);
}

// destructor
Trk::ExtrapolationEngine::~ExtrapolationEngine()
{}


// the interface method initialize
StatusCode Trk::ExtrapolationEngine::initialize()
{
            
    EX_MSG_DEBUG( "", "initialize", "", "starting initialize()" );
    // get the TrackingGeometrySvc
#ifdef LEGACY_TRKGEOM
    if (!m_trackingGeometrySvc.empty()) {
       ATH_CHECK( m_trackingGeometrySvc.retrieve());
    }
#endif
    ATH_CHECK( m_trackingGeometryReadKey.initialize(!m_trackingGeometryReadKey.key().empty()) );

    // retrieve the ExtrapolationEngines
    if (m_extrapolationEngines.retrieve().isFailure()){
        
    } else {
        EX_MSG_DEBUG( "", "initialize", "", "Successfully retrieved " << m_extrapolationEngines.size() << " ExtrapolationEngines. Ordering them now." );
        m_eeAccessor = std::vector<const Trk::IExtrapolationEngine*>(int(Trk::NumberOfGeometryTypes), (const Trk::IExtrapolationEngine*)NULL);
        for (auto& ee : m_extrapolationEngines){
            EX_MSG_DEBUG( "", "initialize", "", "Registering " << ee->name() << " - for GeometryType : "  << ee->geometryType() );
            m_eeAccessor[ee->geometryType()] = (&*ee);
        }
    }

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
    
    return StatusCode::SUCCESS;
}    

// the interface method finalize
StatusCode Trk::ExtrapolationEngine::finalize()
{    
    EX_MSG_DEBUG( "", "finalize", "", "successful" );    
    return StatusCode::SUCCESS;
}


/** charged extrapolation */
Trk::ExtrapolationCode Trk::ExtrapolationEngine::extrapolate(ExCellCharged& ecCharged,
                                                        const Surface* sf,
                                                        BoundaryCheck bcheck) const
{ return extrapolateT<TrackParameters>(ecCharged,sf,ecCharged.propDirection,bcheck); }


/** neutral extrapolation */
Trk::ExtrapolationCode Trk::ExtrapolationEngine::extrapolate(ExCellNeutral& ecNeutral,
                                                        const Surface* sf,
                                                        BoundaryCheck bcheck) const
{ return extrapolateT<NeutralParameters>(ecNeutral,sf,ecNeutral.propDirection,bcheck); }

void Trk::ExtrapolationEngine::throwFailedToGetTrackingGeomtry() const {
   std::stringstream msg;
   msg << "Failed to get conditions data " << m_trackingGeometryReadKey.key() << ".";
   throw std::runtime_error(msg.str());
}

