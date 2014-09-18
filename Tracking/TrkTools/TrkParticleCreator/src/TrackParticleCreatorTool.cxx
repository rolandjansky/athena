/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 TrackParticleCreatorTool.cxx  -  Description
 -------------------
 begin   : Autumn 2003
 authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
 email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
 changes :
 
 ***************************************************************************/
#include "TrkParticleCreator/TrackParticleCreatorTool.h"
// forward declares
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "ITrackToVertex/ITrackToVertex.h" 
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkTrack/Track.h"
#include "Particle/TrackParticle.h"
#include "VxVertex/VxCandidate.h"
// normal includes
#include "DataModel/DataVector.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
#include "TrkEventPrimitives/JacobianLocalToCurvilinear.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkParameters/TrackParameters.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "AtlasDetDescr/AtlasDetectorID.h"


namespace Trk
{
  TrackParticleCreatorTool::TrackParticleCreatorTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p),
    m_detID(0),
    m_trackSummaryTool    ("Trk::TrackSummaryTool/AtlasTrackSummaryTool"),
    m_extrapolator        ("Trk::Extrapolator/AtlasExtrapolator"),
    m_trackToVertex       ("Reco::TrackToVertex/TrackToVertex"),
    m_magFieldSvc         ("AtlasFieldSvc", n),
    m_useTrackSummaryTool (true),
    m_forceTrackSummaryUpdate (false),
    m_keepParameters      (false),
    m_keepAllPerigee      (false),
    m_expressPerigeeToVertex(false),
    m_expressPerigeeToBeamSpot(true)
  {
    declareInterface<ITrackParticleCreatorTool>(this);
    declareProperty("TrackSummaryTool",       m_trackSummaryTool );
    declareProperty("ForceTrackSummaryUpdate",  m_forceTrackSummaryUpdate );
    declareProperty("Extrapolator",   m_extrapolator );
    declareProperty("TrackToVertex",            m_trackToVertex );
    declareProperty("MagFieldSvc",              m_magFieldSvc);
    declareProperty("UseTrackSummaryTool" , m_useTrackSummaryTool);
    declareProperty("KeepParameters",   m_keepParameters);
    declareProperty("KeepAllPerigee",   m_keepAllPerigee);
    declareProperty("ExpressPerigeeToVertex", m_expressPerigeeToVertex);
    declareProperty("ExpressPerigeeToBeamSpot", m_expressPerigeeToBeamSpot);
    declareProperty("CheckConversion",    m_checkConversion=true);
    declareProperty("MinSiHitsForCaloExtrap",   m_minSiHits = 4 );
    declareProperty("MinPtForCaloExtrap",       m_minPt = 1000. );
  }
  
  TrackParticleCreatorTool::~TrackParticleCreatorTool() {}
  
  StatusCode TrackParticleCreatorTool::initialize()
  {

    if(m_expressPerigeeToVertex && m_expressPerigeeToBeamSpot){ 
      ATH_MSG_ERROR("Configuration Error : can't express perigee at both BeamSpot and Vertex"); 
      return StatusCode::FAILURE; 
    }     
    

    /* Retrieve track summary tool */
    if (m_useTrackSummaryTool)
    {
      if ( m_trackSummaryTool.retrieve().isFailure() ) {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackSummaryTool << endreq;
        return StatusCode::FAILURE;
      } else {
        msg(MSG::DEBUG) << "Retrieved tool " << m_trackSummaryTool << endreq;
      }
    }
    
    /* Retrieve track extrapolator from ToolService */
    if ( m_extrapolator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::DEBUG) << "Retrieved tool " << m_extrapolator << endreq;
    }

    if (detStore()->retrieve(m_detID, "AtlasID" ).isFailure()) {
      ATH_MSG_FATAL ("Could not get AtlasDetectorID ");
      return StatusCode::FAILURE;
    }
    
    /* Retrieve track to vertex from ToolService */ 
    if ( m_trackToVertex.retrieve().isFailure() ) { 
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackToVertex << endreq; 
      return StatusCode::FAILURE; 
    } else { 
      msg(MSG::DEBUG) << "Retrieved tool " << m_trackToVertex << endreq; 
    } 


    /* MagneticFieldSvc handles updates itself */
    if (m_magFieldSvc.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve MagneticFieldSvc." );
      return StatusCode::FAILURE;
    }   
    msg(MSG::VERBOSE)  << name() <<" initialize successful." << endreq;
    return StatusCode::SUCCESS;
  }
  
  StatusCode TrackParticleCreatorTool::finalize()
  {
    msg(MSG::INFO)  << " finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }
  
  Rec::TrackParticle* TrackParticleCreatorTool::createParticle(const Trk::Track*    track,
                                                               const Trk::VxCandidate*  vxCandidate,
                                                               Trk::TrackParticleOrigin prtOrigin)
  {
    if (track == 0) return 0;
    const Trk::Perigee* aPer(0);
    
    // the default way; I left it as it was because it is working fine!!
    if (!m_expressPerigeeToVertex && !m_expressPerigeeToBeamSpot )
    {
      if ( (castPerigeeAndCheck(track, aPer) ) )
      {
        // aMeasPer clone will be created later if all perigee option selected
        if (m_keepAllPerigee)
        {
          aPer  = 0;
        }
        else
        {
          aPer  = aPer->clone();
        }
      } else
      {
	const Amg::Vector3D persf(0,0,0); 
	const Trk::Perigee* result = m_trackToVertex->perigeeAtVertex( *track, persf ); 
	if (result != 0) { 
	  aPer =  result; 
	}else{ 
	  ATH_MSG_DEBUG ("Could not extrapolate to 0,0,0. No TrackParticle created."); 
          return 0;
        }
      }
    } 

    else if (m_expressPerigeeToBeamSpot){ //Express parameters at beamspot 
      const Trk::Perigee* result = m_trackToVertex->perigeeAtBeamspot( *track ); 
      if(!result){ 
	
	ATH_MSG_WARNING("Failed to extrapolate to first Beamspot"); 
	if( !track->perigeeParameters() ){ 
	  return 0;       
	} 
	aPer = track->perigeeParameters()->clone();   
      } else { 
	aPer = result; 
      } 
    }     else // the non default way, express the perigee wrt. the vertex position
    {
      if (vxCandidate != 0)
	{
	  const Trk::Perigee* result =  m_trackToVertex->perigeeAtVertex( *track, vxCandidate->recVertex().position()); 
	  if (result != 0) { 
	    aPer = result ; 
	  } else{
	    ATH_MSG_DEBUG ("Could not extrapolate track to vertex region! No TrackParticle created.");
	    return 0;
        }
	} else {
        if ( (castPerigeeAndCheck(track, aPer) ) )
	  {
	    aPer = aPer->clone();
        } else
	  {
	    ATH_MSG_DEBUG ("No vertex given and track has no perigee either! No TrackParticle created.");
	    return 0;
	  }
      }
    }
    
    const Trk::TrackSummary* summary;
    if (m_trackSummaryTool!=0 && m_useTrackSummaryTool)
    {
      if (m_forceTrackSummaryUpdate)
      {
        // next lines are a bit ugly: we need to cast the const away to update the track and then get the new summary
        // and even clone it because it belongs to the track ...
        Trk::Track& nonConstTrack = const_cast<Trk::Track&>(*track);
	m_trackSummaryTool->updateTrack(nonConstTrack); 
      } 
      summary = m_trackSummaryTool->createSummary(*track); 
      
      if (summary == 0)
      {
        ATH_MSG_DEBUG ("No proper TrackSummary was returned. Creating TrackParticle with a dummy TrackSummary");
        summary = new Trk::TrackSummary;
      } // else msg(MSG::VERBOSE) << "Got Summary for Track" << endreq;
    } else{
      ATH_MSG_VERBOSE ("No proper TrackSummaryTool found. Creating TrackParticle with a dummy TrackSummary");
      summary = new Trk::TrackSummary;
    }
    
    // find the first and the last hit in track
    // we do that the same way as in the track slimming tool!
    // that way it is also ok on not slimmed tracks!
    std::vector<const Trk::TrackParameters*> parameters;
    
    if (m_keepParameters)
    {
      const DataVector<const TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
      
      const Trk::TrackParameters* first(0) ;
      
      // search first valid TSOS first
      for ( DataVector<const TrackStateOnSurface>::const_iterator itTSoS = trackStates->begin(); itTSoS != trackStates->end(); ++itTSoS)
      {
        if ( (*itTSoS)->type(TrackStateOnSurface::Measurement) && (*itTSoS)->trackParameters()!=0 && (*itTSoS)->measurementOnTrack()!=0 && !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*itTSoS)->measurementOnTrack()))
        {
          first = (*itTSoS)->trackParameters();
          parameters.push_back((*itTSoS)->trackParameters()->clone());
          break;
        }
      }
      
      // search last valid TSOS first
      for ( DataVector<const TrackStateOnSurface>::const_reverse_iterator rItTSoS = trackStates->rbegin(); rItTSoS != trackStates->rend(); ++rItTSoS)
      {
        if ( (*rItTSoS)->type(TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0 && !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*rItTSoS)->measurementOnTrack()))
        {
          if (!(first == (*rItTSoS)->trackParameters())) parameters.push_back((*rItTSoS)->trackParameters()->clone());
          break;
        }
      }
      
      // security check:
      if (parameters.size() > 2)
        ATH_MSG_WARNING ("More than two additional track parameters to be stored in TrackParticle!");
    }
    // KeepAllPerigee will keep all perigee's on the track plus the parameters at the first measurement,
    // provided this measurement precedes any second perigee.
    // The track (initial) perigee is the 'defining parameter' for the TrackParticle,
    // by convention this is pushed to the back of the parameter vector by the TP constructor.
    else if (m_keepAllPerigee)
    {
      DataVector<const TrackStateOnSurface>::const_iterator itTSoS =
      track->trackStateOnSurfaces()->begin();
      bool haveFirstMeasurementParameters = false;
      for ( ; itTSoS != track->trackStateOnSurfaces()->end(); ++itTSoS)
      {
        if (! (**itTSoS).trackParameters())     continue;
        
        if (! haveFirstMeasurementParameters
            && (**itTSoS).type(TrackStateOnSurface::Measurement)
            && ! (**itTSoS).type(TrackStateOnSurface::Outlier)
            && (**itTSoS).measurementOnTrack()
            && ! dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**itTSoS).measurementOnTrack()))
        {
          haveFirstMeasurementParameters  = true;
          parameters.push_back((**itTSoS).trackParameters()->clone());
          ATH_MSG_VERBOSE( " including first measurement parameters at R "
                          << (**itTSoS).trackParameters()->position().perp()
                          << ", Z " << (**itTSoS).trackParameters()->position().z() );
          continue;
        }
        if (! (**itTSoS).type(TrackStateOnSurface::Perigee)
            || ! dynamic_cast<const Perigee*>((**itTSoS).trackParameters()))  continue;
        if (! aPer)
        {
          aPer = dynamic_cast<const Perigee*>((**itTSoS).trackParameters()->clone());
        }
        else
        {
          parameters.push_back((**itTSoS).trackParameters()->clone());
        }
        
        ATH_MSG_VERBOSE( " including perigee at R "
                        << (**itTSoS).trackParameters()->position().perp()
                        << ", Z " << (**itTSoS).trackParameters()->position().z() );
        
        // we are not interested in keeping measurement parameters after any second perigee
        if (parameters.size() > 0) haveFirstMeasurementParameters = true;
      }
    }
    
    const Trk::FitQuality* fitQuality = new FitQuality( (*track->fitQuality()) );
    Rec::TrackParticle* tp = new Rec::TrackParticle(track, prtOrigin, vxCandidate, summary, parameters, aPer,  fitQuality);
    return tp;
  }
  
  xAOD::TrackParticle* TrackParticleCreatorTool::createParticle( const Trk::Track& track,
                                                                xAOD::TrackParticleContainer* container,
                                                                const xAOD::Vertex* vxCandidate,
                                                                xAOD::ParticleHypothesis prtOrigin) const {
    
    const Trk::Perigee* aPer(0);
    const Trk::TrackParameters* parsToBeDeleted = 0;
    // the default way; I left it as it was because it is working fine!!
    if (!m_expressPerigeeToVertex && !m_expressPerigeeToBeamSpot) {
      if ( (castPerigeeAndCheck(&track, aPer) ) )  {
        // aMeasPer clone will be created later if all perigee option selected
        if (m_keepAllPerigee) aPer  = 0;
      }else{
	const Amg::Vector3D persf(0,0,0); 
	const Trk::Perigee* result = m_trackToVertex->perigeeAtVertex(track, persf); 
	if (result != 0) { 
	  aPer =  result; 
	  parsToBeDeleted = result; 
	}
	else{
          ATH_MSG_DEBUG ("Could not extrapolate to 0,0,0. No TrackParticle created.");
          return 0;
        }
      }
    }else if (m_expressPerigeeToBeamSpot){ //Express parameters at beamspot 
    const Trk::Perigee* result = m_trackToVertex->perigeeAtBeamspot(track); 
    if(!result){ 
      ATH_MSG_WARNING("Failed to extrapolate to first Beamspot"); 
      if( !track.perigeeParameters() ){ 
	return 0;       
      } 
      aPer = track.perigeeParameters();   
    }else{ 
      parsToBeDeleted = result; 
      aPer = result; 
    }
    } else {  // the non default way, express the perigee wrt. the vertex position
      if (vxCandidate != 0 ) {
	const Trk::Perigee* result =  m_trackToVertex->perigeeAtVertex(track, vxCandidate->position());
        if (result != 0) {
	  parsToBeDeleted = result;
          aPer = result ;
        }else{
          ATH_MSG_DEBUG ("Could not extrapolate track to vertex region! No TrackParticle created.");
          return 0;
        }
      } else {
        if( !castPerigeeAndCheck(&track, aPer) ) {
          ATH_MSG_DEBUG ("No vertex given and track has no perigee either! No TrackParticle created.");
          return 0;
        }
      }
    }
    
    const Trk::TrackSummary* summary;
    if (m_trackSummaryTool!=0 && m_useTrackSummaryTool) {
      if(m_forceTrackSummaryUpdate){
        // next lines are a bit ugly: we need to cast the const away to update the track and then get the new summary
        // and even clone it because it belongs to the track ...
        Trk::Track& nonConstTrack = const_cast<Trk::Track&>(track);
        m_trackSummaryTool->updateTrack(nonConstTrack);
      }
      summary = m_trackSummaryTool->createSummary(track); 
    }else{
      ATH_MSG_VERBOSE ("No proper TrackSummaryTool found. Creating TrackParticle with a TrackSummary on track");
      summary = track.trackSummary();
    }
    
    // find the first and the last hit in track
    // we do that the same way as in the track slimming tool!
    // that way it is also ok on not slimmed tracks!
    std::vector<const Trk::TrackParameters*> parameters;
    std::vector<xAOD::ParameterPosition> parameterPositions;
    if (m_keepParameters) {
      const DataVector<const TrackStateOnSurface>* trackStates = track.trackStateOnSurfaces();
      
      const Trk::TrackParameters* first(0) ;
      
      // search first valid TSOS first
      for ( DataVector<const TrackStateOnSurface>::const_iterator itTSoS = trackStates->begin(); itTSoS != trackStates->end(); ++itTSoS) {
        if ( (*itTSoS)->type(TrackStateOnSurface::Measurement) && (*itTSoS)->trackParameters()!=0 &&
            (*itTSoS)->measurementOnTrack()!=0 && !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*itTSoS)->measurementOnTrack())) {
          first = (*itTSoS)->trackParameters();
          parameters.push_back((*itTSoS)->trackParameters());
          parameterPositions.push_back(xAOD::FirstMeasurement);
          break;
        }
      }
      
      // search last valid TSOS first
      for ( DataVector<const TrackStateOnSurface>::const_reverse_iterator rItTSoS = trackStates->rbegin(); rItTSoS != trackStates->rend(); ++rItTSoS) {
        if ( (*rItTSoS)->type(TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 &&
            (*rItTSoS)->measurementOnTrack()!=0 && !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*rItTSoS)->measurementOnTrack())) {
          if (!(first == (*rItTSoS)->trackParameters())) {
            parameters.push_back((*rItTSoS)->trackParameters());
            parameterPositions.push_back(xAOD::LastMeasurement);
          }
          break;
        }
      }
      
      // security check:
      if (parameters.size() > 2)
        ATH_MSG_WARNING ("More than two additional track parameters to be stored in TrackParticle!");
    }
    // KeepAllPerigee will keep all perigee's on the track plus the parameters at the first measurement,
    // provided this measurement precedes any second perigee.
    // The track (initial) perigee is the 'defining parameter' for the TrackParticle,
    // by convention this is pushed to the back of the parameter vector by the TP constructor.
    else if (m_keepAllPerigee) {
      DataVector<const TrackStateOnSurface>::const_iterator itTSoS = track.trackStateOnSurfaces()->begin();
      bool haveFirstMeasurementParameters = false;
      for ( ; itTSoS != track.trackStateOnSurfaces()->end(); ++itTSoS) {
        if (! (**itTSoS).trackParameters())     continue;
        
        if (! haveFirstMeasurementParameters
            && (**itTSoS).type(TrackStateOnSurface::Measurement)
            && ! (**itTSoS).type(TrackStateOnSurface::Outlier)
            && (**itTSoS).measurementOnTrack()
            && ! dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**itTSoS).measurementOnTrack())) {
          haveFirstMeasurementParameters  = true;
          parameters.push_back((**itTSoS).trackParameters());
          ATH_MSG_VERBOSE( " including first measurement parameters at R "
                          << (**itTSoS).trackParameters()->position().perp()
                          << ", Z " << (**itTSoS).trackParameters()->position().z() );
          parameterPositions.push_back(xAOD::FirstMeasurement);
          continue;
        }
        if (! (**itTSoS).type(TrackStateOnSurface::Perigee)
            || ! dynamic_cast<const Perigee*>((**itTSoS).trackParameters()))  continue;
        if (! aPer) {
          aPer = dynamic_cast<const Perigee*>((**itTSoS).trackParameters());
        } else {
          parameters.push_back((**itTSoS).trackParameters());
        }
        
        ATH_MSG_VERBOSE( " including perigee at R "
                        << (**itTSoS).trackParameters()->position().perp()
                        << ", Z " << (**itTSoS).trackParameters()->position().z() );
        
        // we are not interested in keeping measurement parameters after any second perigee
        if (parameters.size() > 0) haveFirstMeasurementParameters = true;
      }
    }
    
    xAOD::TrackParticle* trackparticle = createParticle(aPer,track.fitQuality(),&track.info(),summary,parameters,parameterPositions,prtOrigin,container);
    if (m_trackSummaryTool!=0 && m_useTrackSummaryTool) delete summary;
    delete parsToBeDeleted;
    return trackparticle;
  }
  
  xAOD::TrackParticle* TrackParticleCreatorTool::createParticle( const Rec::TrackParticle& trackParticle, xAOD::TrackParticleContainer* container ) const {
    // Attempt to fill the position enums - will necessarily be a bit of a hack, since we don't have all the information.
    std::vector< xAOD::ParameterPosition> positions;
    bool firstMeasurement;
    for (auto parameter : trackParticle.trackParameters()){
      if (!firstMeasurement && parameter && !parameter->associatedSurface().isFree()){
        // if the surface isn't free, it must belong to a detector element => measurement
        firstMeasurement=true;
        positions.push_back(xAOD::FirstMeasurement);
      } else if (firstMeasurement && parameter && !parameter->associatedSurface().isFree()){
        // Making the (possibly unfounded assumption that if we have the first measurement, the next will be the last)
        positions.push_back(xAOD::LastMeasurement);
      } else {
        positions.push_back(xAOD::BeamLine); // Don't have a default yet!
      }
    }

    xAOD::TrackParticle* trackparticle = createParticle(trackParticle.measuredPerigee(),trackParticle.fitQuality(),
                                                        &trackParticle.info(),trackParticle.trackSummary(),trackParticle.trackParameters(),positions,
                                                        static_cast<xAOD::ParticleHypothesis>(trackParticle.info().particleHypothesis()),container);
    trackparticle->setTrackLink( *(trackParticle.trackElementLink()) );
    
    if( m_checkConversion ) compare(trackParticle,*trackparticle);
    
    return trackparticle;
  }
  
  xAOD::TrackParticle* TrackParticleCreatorTool::createParticle( const ElementLink<TrackCollection>& trackLink,
                                                                xAOD::TrackParticleContainer* container,
                                                                const xAOD::Vertex* vxCandidate,
                                                                xAOD::ParticleHypothesis prtOrigin) const {
    xAOD::TrackParticle* trackparticle = createParticle( **trackLink, container, vxCandidate, prtOrigin );
    trackparticle->setTrackLink( trackLink );
    return trackparticle;
  }
  
  
  xAOD::TrackParticle* TrackParticleCreatorTool::createParticle( const Perigee* perigee, const FitQuality* fq, const TrackInfo* trackInfo, const TrackSummary* summary,
                                                                const std::vector<const Trk::TrackParameters*>& parameters, 
                                                                const std::vector< xAOD::ParameterPosition>& positions,
                                                                xAOD::ParticleHypothesis prtOrigin,
                                                                xAOD::TrackParticleContainer* container ) const {
    
    xAOD::TrackParticle* trackparticle = new xAOD::TrackParticle();
    if( container ) container->push_back( trackparticle );
    else trackparticle->makePrivateStore();
    
    // Fit quality
    if( fq ) setFitQuality(*trackparticle,*fq);
    
    // Track Info
    if( trackInfo ) setTrackInfo(*trackparticle,*trackInfo,prtOrigin);
    
    // track summary
    if (summary) setTrackSummary(*trackparticle,*summary);
    
    // Parameters
    if (perigee) setDefiningParameters(*trackparticle,*perigee);
    else      ATH_MSG_WARNING( "Track without perigee parameters? Not setting any defining parameters!");
    setParameters(*trackparticle,parameters, positions);
    
    return trackparticle;
  }
  
  
  //W.L. 2013-10-29: Since no dcast is needed any more the whole function could be removed
  bool TrackParticleCreatorTool::castPerigeeAndCheck(
                                                     const Trk::Track* track,
                                                     const Trk::Perigee* &aPer) const
  {
    aPer = track->perigeeParameters();
    if (aPer==0)
    {
      ATH_MSG_VERBOSE ("Track has no perigee parameters.");
      return false;
    }
    return true;
  }
  
  void TrackParticleCreatorTool::compare( const TrackParameters& tp1, const TrackParameters& tp2 ) const {
    int index = Amg::compare(tp1.parameters(),tp2.parameters(),1e-6,true);
    if( index != -1 ){
      ATH_MSG_WARNING("Bad parameters conversion " << Amg::toString(tp1.parameters(),7)
                      << " --- " << Amg::toString(tp2.parameters(),7) );
    }
    if( (tp1.covariance() && !tp2.covariance()) ||
       (!tp1.covariance() && tp2.covariance()) ){
      ATH_MSG_WARNING("Bad Covariance conversion " << tp1.covariance() << " --- " << tp2.covariance() );
    }else if( tp1.covariance() && tp2.covariance() ){
      std::pair<int,int> indices = Amg::compare(*tp1.covariance(),*tp2.covariance(),1e-6,true);
      if( indices.first != -1 )
        ATH_MSG_WARNING("Bad Covariance conversion " << std::endl
                        << Amg::toString(*tp1.covariance(),10) << std::endl 
                        << Amg::toString(*tp2.covariance(),10) );
    }
  }
  
  void TrackParticleCreatorTool::compare( const Rec::TrackParticle& tp, const xAOD::TrackParticle& tpx ) const {
    if( tp.measuredPerigee() ) compare(*tp.measuredPerigee(),tpx.perigeeParameters());
    
    // do to add other components 
    if( tp.fitQuality() ){
      
    }
    //trackParticle.info(),trackParticle.trackSummary(),
    if( tp.trackParameters().size() != tpx.numberOfParameters()){
      ATH_MSG_WARNING("Number of parameters not the same " << tp.trackParameters().size() << " --- " << tpx.numberOfParameters());
    }
  }

  
  void TrackParticleCreatorTool::setParameters( xAOD::TrackParticle& tp, const std::vector<const Trk::TrackParameters*>& parameters, const std::vector< xAOD::ParameterPosition>& positions  ) const {
    std::vector< std::vector < float > > parametersVec;
    parametersVec.resize(parameters.size());
    unsigned int numParam=0;
    for( auto param : parameters ){ 
      std::vector<float>& values = parametersVec[numParam];
      values.resize(6);
      const Amg::Vector3D & pos = param->position();
      const Amg::Vector3D & mom = param->momentum();
      values[0]=pos[0];values[1]=pos[1];values[2]=pos[2];
      values[3]=mom[0];values[4]=mom[1];values[5]=mom[2];


      AmgSymMatrix(5) covarianceMatrix;  covarianceMatrix.setIdentity();

      if( param->covariance() ){
        // has covariance matrix
        //now convert from to Curvilinear -- to be double checked for correctness
        Amg::Vector3D magnFieldVect; magnFieldVect.setZero();
        m_magFieldSvc->getField( &pos, &magnFieldVect );

        CurvilinearUVT curvilinearUVT(param->momentum().unit());     
        const Amg::Transform3D& localToGlobalTransform = param->associatedSurface().transform();
     
        JacobianLocalToCurvilinear jacobian(magnFieldVect,
                                            param->parameters()[Trk::qOverP],
                                            sin(param->parameters()[Trk::theta]),
                                            curvilinearUVT,
                                            localToGlobalTransform.rotation().col(0),
                                            localToGlobalTransform.rotation().col(1));     
     
        covarianceMatrix = param->covariance()->similarity(jacobian);
        //std::cout << "Before \n"<< *param->covariance() << std::endl;
        //std::cout << "jacob \n" <<  jacobian << std::endl;
        //std::cout << "After \n" << covarianceMatrix << std::endl;
      }
      std::vector<float> covMatrixVec;
      Amg::compress(covarianceMatrix,covMatrixVec);
      tp.setTrackParameterCovarianceMatrix(numParam,covMatrixVec);

      ++numParam;
    }
  
    tp.setTrackParameters(parametersVec);
    unsigned int i=0;
    for (;i<positions.size();++i) {
      tp.setParameterPosition(i,positions[i]);
    }
  }
} // end of namespace Trk
