/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// TrackIsolationTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "TrackIsolationTool.h"

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  TrackIsolationTool::TrackIsolationTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
        m_tracksInConeTool("xAOD::TrackParticlesInConeTool/TrackParticlesInConeTool")
  {
    declareInterface<ITrackIsolationTool>(this);
    declareProperty("TrackParticleLocation",m_indetTrackParticleLocation = "InDetTrackParticles");
    declareProperty("TracksInConeTool", m_tracksInConeTool);
    declareProperty("SimpleIsolation",  m_simpleIsolation = false);
    declareProperty("OverlapCone",      m_overlapCone2 = 0.1); /// will be squared later
  }

  TrackIsolationTool::~TrackIsolationTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode TrackIsolationTool::initialize() {

    if( !m_simpleIsolation ) ATH_CHECK(m_tracksInConeTool.retrieve());

    /** square cone */
    m_overlapCone2 *= m_overlapCone2;
    
    return StatusCode::SUCCESS;
  }

  StatusCode TrackIsolationTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const TrackParticleContainer* TrackIsolationTool::retrieveTrackParticleContainer() const {
    // retrieve MuonSpectrometer tracks
    const TrackParticleContainer* indetTrackParticles = 0;
    if(evtStore()->contains<TrackParticleContainer>(m_indetTrackParticleLocation)) {
      if(evtStore()->retrieve(indetTrackParticles,m_indetTrackParticleLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_indetTrackParticleLocation );
        return 0;
      }
    }
    return indetTrackParticles;
  }

  bool TrackIsolationTool::trackIsolation( TrackIsolation& result, const IParticle& tp, const std::vector<Iso::IsolationType>& isoTypes, 
                                           TrackIsolationTool::SubtractionStrategy strategy, const Vertex* vertex, 
                                           const std::set<const xAOD::TrackParticle*>* exclusionSet,
                                           const TrackParticleContainer* indetTrackParticles ) {

    /// prepare input
    TrackIsolationInput input( &tp, strategy, vertex, exclusionSet );
    
    for( auto isoType : isoTypes ){
      Iso::IsolationFlavour flavour = Iso::isolationFlavour(isoType);
      if( flavour != Iso::ptcone && flavour != Iso::nucone ) {
        ATH_MSG_WARNING("Unsupported isolation type passed, cannot calculate isolation " << static_cast<int>(isoType));
        return false;
      }
      float coneSize = Iso::coneSize(isoType);
      input.cones.push_back(coneSize*coneSize);
    }
    std::sort(input.cones.begin(),input.cones.end(),[](float i, float j) { return i>j; });

    /// setup cones and max radius: 
    result.ptcones.resize(input.cones.size(),0.);
    result.nucones.resize(input.cones.size(),0.);
    input.maxRadius = sqrt(input.cones[0]);
    
    bool success = false;
    // run isolation code
    if( indetTrackParticles )    success = simpleIsolation(input,result,indetTrackParticles);
    else if( m_simpleIsolation ) success = simpleIsolation(input,result);
    else                         success = binnedIsolation(input,result);

    if( msgLvl(MSG::DEBUG) ){
      if( !success ){
        ATH_MSG_DEBUG(" Track isolation calculation failed");
      }else{
        msg(MSG::DEBUG) << "Calculated track isolation: ";
        for( unsigned int i = 0; i< result.ptcones.size();++i ){
          msg(MSG::DEBUG) << " coneSize " << std::setw(3) << sqrt(input.cones[i]) << " value " << result.ptcones[i];
        }
        msg(MSG::DEBUG) << endreq;
      }
    }
    return success;
  }


  bool TrackIsolationTool::binnedIsolation( TrackIsolationInput& input, TrackIsolation& result ) {

    /// prepare look-up structure
    std::vector<const TrackParticle*> tps;
    if( !m_tracksInConeTool->particlesInCone(input.particle->eta(),input.particle->phi(),input.maxRadius,tps) ) return false;
    
    for( const auto& tp : tps ) add( input,*tp, result );
    return true;
  }

  bool TrackIsolationTool::simpleIsolation( TrackIsolationInput& input, TrackIsolation& result, const TrackParticleContainer* indetTrackParticles ) const {

    /// retrieve track particles if not passed into the interface
    if( !indetTrackParticles ) indetTrackParticles = retrieveTrackParticleContainer();
    
    // exit if not track particles found
    if( !indetTrackParticles ) return false;

    // loop over all track particles
    for( const auto& tp : *indetTrackParticles ) add( input, *tp, result );
    
    return true;
  } 

}	// end of namespace
