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
#undef NDEBUG
#include "TrkParticleCreator/TrackParticleCreatorTool.h"
// forward declares
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "ITrackToVertex/ITrackToVertex.h" 
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
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

#include "IdDictDetDescr/IdDictManager.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

//#include "TrkParameters/MeasuredPerigee.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

// helper methods to print messages 
template<class T>
inline MsgStream& operator<<( MsgStream& msg_stream, const std::map<std::string, T>& elm_map)    {
  for (const std::pair<const std::string, T> &elm : elm_map) {
    msg_stream  << " " << elm.first;
  }
  return msg_stream;
}

template<class T>
inline MsgStream& operator<<( MsgStream& msg_stream, const std::vector<std::string>& elm_vector)    {
  for (const std::string &elm : elm_vector) {
    msg_stream  << " " << elm;
  }
  return msg_stream;
}

namespace Trk
{
  const std::string TrackParticleCreatorTool::s_trtdEdxUsedHitsDecorationName {"TRTdEdxUsedHits"};

  namespace {
    void createEProbabilityMap(std::map<std::string,std::pair<Trk::eProbabilityType, bool> > &eprob_map) {
      // key: name to be used to activate copying of the electron probability values to the xAOD TrackParticle
      //      abd for those which are added as decoration the name to be used for the decoration
      // value.first: enum of the electron probability value
      // value.second: false is a non dynamic element of the xAOD TrackParticle and added via setTrackSummary
      //               true  will be added as a decoration.
      eprob_map.insert( std::make_pair("eProbabilityComb",std::make_pair(Trk::eProbabilityComb,false)) );
      eprob_map.insert( std::make_pair("eProbabilityHT",std::make_pair(Trk::eProbabilityHT,false)) );

      // added as decorations 
      eprob_map.insert( std::make_pair("eProbabilityToT",std::make_pair(Trk::eProbabilityToT,true)) );
      eprob_map.insert( std::make_pair("eProbabilityBrem",std::make_pair(Trk::eProbabilityBrem,true)) );
      eprob_map.insert( std::make_pair("TRTTrackOccupancy",std::make_pair(Trk::numberOfeProbabilityTypes,true)) );
      eprob_map.insert( std::make_pair("TRTdEdx",std::make_pair(static_cast<Trk::eProbabilityType>(Trk::numberOfeProbabilityTypes+1),
                                                                true)) );
    }

    void createExtraSummaryTypeMap(std::map<std::string,Trk::SummaryType > &extra_summary_type_map) {
      extra_summary_type_map.insert( std::make_pair("TRTdEdxUsedHits",Trk::numberOfTRTHitsUsedFordEdx));
    }
  }


  const SG::AuxElement::Decorator<uint8_t> TrackParticleCreatorTool::s_trtdEdxUsedHitsDecoration(TrackParticleCreatorTool::trtdEdxUsedHitsAuxName()) ;

  TrackParticleCreatorTool::TrackParticleCreatorTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p),
    m_detID(0),
    m_trackSummaryTool    ("Trk::TrackSummaryTool/AtlasTrackSummaryTool"),
    m_extrapolator        ("Trk::Extrapolator/AtlasExtrapolator"),
    m_trackToVertex       ("Reco::TrackToVertex/TrackToVertex"),
    m_hitSummaryTool      ("Muon::MuonHitSummaryTool/MuonHitSummaryTool"),
    m_magFieldSvc         ("AtlasFieldSvc", n),
    m_beamConditionsService("BeamCondSvc", n),
    m_IBLParameterSvc("IBLParameterSvc",n),
    m_copyExtraSummaryName {"eProbabilityComb","eProbabilityHT","TRTTrackOccupancy","TRTdEdx","TRTdEdxUsedHits"},
    m_useTrackSummaryTool (true),
    m_useMuonSummaryTool  (false),
    m_doITk               (false),
    m_forceTrackSummaryUpdate (false),
    m_keepParameters      (false),
    m_keepFirstParameters(false),
    m_keepAllPerigee      (false),
    m_expressPerigeeToBeamSpot(true),
    m_perigeeExpression("BeamLine")
  {
    declareInterface<ITrackParticleCreatorTool>(this);
    declareProperty("TrackSummaryTool",       m_trackSummaryTool );
    declareProperty("MuonSummaryTool",       m_hitSummaryTool );
    declareProperty("ForceTrackSummaryUpdate",  m_forceTrackSummaryUpdate );
    declareProperty("Extrapolator",   m_extrapolator );
    declareProperty("TrackToVertex",            m_trackToVertex );
    declareProperty("MagFieldSvc",              m_magFieldSvc);
    declareProperty("UseTrackSummaryTool" , m_useTrackSummaryTool);
    declareProperty("UseMuonSummaryTool" , m_useMuonSummaryTool);
    declareProperty("DoITk" , m_doITk);
    declareProperty("KeepParameters",   m_keepParameters);
    declareProperty("KeepFirstParameters",   m_keepFirstParameters);
    declareProperty("KeepAllPerigee",   m_keepAllPerigee);
    declareProperty("ExpressPerigeeToBeamSpot", m_expressPerigeeToBeamSpot);
    declareProperty("CheckConversion",    m_checkConversion=true);
    declareProperty("MinSiHitsForCaloExtrap",   m_minSiHits = 4 );
    declareProperty("MinPtForCaloExtrap",       m_minPt = 1000. );
    declareProperty("PerigeeExpression",     m_perigeeExpression);
    declareProperty("BadClusterID",     m_badclusterID = 0); //0 = off, 1 = OOT, 2 = dE/dx, 3 = combination of OOT and dE/dx, 4 = combination of OOT, dE/dx, and size
    declareProperty("ExtraSummaryTypes",  m_copyExtraSummaryName);
  }
  
  TrackParticleCreatorTool::~TrackParticleCreatorTool() {}
  
  StatusCode TrackParticleCreatorTool::initialize()
  {

    ATH_MSG_DEBUG("initialize TrackParticleCreatorTool");

    if(std::find(std::begin(m_perigeeOptions), std::end(m_perigeeOptions),  m_perigeeExpression) == std::end(m_perigeeOptions)){ 
      ATH_MSG_ERROR("Unknown Configuration for Perigee Expression - please use one of " << m_perigeeOptions);      
      return StatusCode::FAILURE; 
    }     
    
    if(!m_expressPerigeeToBeamSpot){
      ATH_MSG_WARNING("Using old configuration option! please use one of " << m_perigeeOptions << ". Assuming Origin." );
      m_perigeeExpression = "Origin";
    }

    /* Retrieve track summary tool */
    if (m_useTrackSummaryTool)
    {
      if ( m_trackSummaryTool.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_trackSummaryTool );
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG( "Retrieved tool " << m_trackSummaryTool );
      }
    }
    
    /* Retrieve track extrapolator from ToolService */
    if ( m_extrapolator.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_extrapolator );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Retrieved tool " << m_extrapolator );
    }

    if (detStore()->retrieve(m_detID, "AtlasID" ).isFailure()) {
      ATH_MSG_FATAL ("Could not get AtlasDetectorID ");
      return StatusCode::FAILURE;
    }

    if (detStore()->retrieve(m_pixelID, "PixelID" ).isFailure()) {
      ATH_MSG_FATAL ("Could not get PixelID ");
      return StatusCode::FAILURE;
    }

    if (m_IBLParameterSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not retrieve IBLParameterSvc" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "Retrieved tool " << m_IBLParameterSvc );
    }

    m_doIBL = m_IBLParameterSvc->containsIBL();
    ATH_MSG_INFO( "doIBL set to "<<m_doIBL );

    if(m_doIBL && !m_IBLParameterSvc->contains3D()){
      ATH_MSG_WARNING( "Assuming hybrid 2D/3D IBL module composition, but geometry is all-planar" );
    }
    
    /* Retrieve track to vertex from ToolService */ 
    if ( m_trackToVertex.retrieve().isFailure() ) { 
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackToVertex );
      return StatusCode::FAILURE; 
    } else { 
      ATH_MSG_DEBUG( "Retrieved tool " << m_trackToVertex ); 
    } 

    if (m_useMuonSummaryTool){
	/* Retrieve hit summary tool from ToolService */
	if ( m_hitSummaryTool.retrieve().isFailure() ) {
	  ATH_MSG_FATAL("Failed to retrieve tool " << m_hitSummaryTool );
	  return StatusCode::FAILURE;
	} else {
	  ATH_MSG_DEBUG( "Retrieved tool " << m_hitSummaryTool);
	}
      }

    /* MagneticFieldSvc handles updates itself */
    if (m_magFieldSvc.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve MagneticFieldSvc." );
      return StatusCode::FAILURE;
    }   
  
    if ( m_beamConditionsService.retrieve().isFailure() ){
      ATH_MSG_WARNING( "Failed to retrieve service " << m_beamConditionsService << " - Tilts will not be filled!" );
    }

    StatusCode sc(StatusCode::SUCCESS);
    m_copyEProbabilities.clear();
    m_decorateEProbabilities.clear();
    m_decorateSummaryTypes.clear();

    if (!m_copyExtraSummaryName.empty()) {
      std::map<std::string,std::pair<Trk::eProbabilityType, bool> > eprob_map;
      std::map<std::string,Trk::SummaryType > extra_summary_type_map;
      createEProbabilityMap(eprob_map);
      createExtraSummaryTypeMap(extra_summary_type_map);

      std::vector<std::string> errors;
      for( const std::string &eprob_to_copy : m_copyExtraSummaryName) {
        std::map<std::string,std::pair<Trk::eProbabilityType, bool> >::const_iterator 
          eprob_iter = eprob_map.find(eprob_to_copy);
        if (eprob_iter == eprob_map.end()) {
          std::map<std::string,Trk::SummaryType >::const_iterator
            extra_summary_type_iter = extra_summary_type_map.find(eprob_to_copy);
          if (extra_summary_type_iter == extra_summary_type_map.end()) {
            errors.push_back(eprob_to_copy);
          }
          else {
            m_decorateSummaryTypes.push_back( std::make_pair(SG::AuxElement::Decorator<uint8_t>(extra_summary_type_iter->first),
                                                             extra_summary_type_iter->second));  
          }
        }
        else {
          if (!eprob_iter->second.second) {
            m_copyEProbabilities.push_back(eprob_iter->second.first);
          }
          else{
            m_decorateEProbabilities.push_back( std::make_pair(SG::AuxElement::Decorator<float>(eprob_iter->first),eprob_iter->second.first)); 
          }
        }
      }

      if (!errors.empty()) {
        ATH_MSG_ERROR( "Error in configuration. Unknown electron probability name: " << errors << ". known are " <<  eprob_map << " " << extra_summary_type_map);
        sc = StatusCode::FAILURE;
      }
    }


    ATH_MSG_VERBOSE( " initialize successful." );
    return sc;
  }
  
  StatusCode TrackParticleCreatorTool::finalize()
  {
    ATH_MSG_INFO( " finalize successful" );
    return StatusCode::SUCCESS;
  }
  
  Rec::TrackParticle* TrackParticleCreatorTool::createParticle(const Trk::Track*    track,
                                                               const Trk::VxCandidate*  vxCandidate,
                                                               Trk::TrackParticleOrigin prtOrigin)
  {
    if (track == 0) return 0;
    const Trk::Perigee* aPer(0);
    
    // the default way; I left it as it was because it is working fine!!
    if ( m_perigeeExpression == "Origin")
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

    else if  (m_perigeeExpression == "BeamSpot"){ //Express parameters at beamspot 
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
    }     
    else if (m_perigeeExpression == "Vertex"){
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
    else if (m_perigeeExpression == "BeamLine"){
      const Trk::Perigee* result = m_trackToVertex->perigeeAtBeamline( *track ); 
      if(!result){ 
	
        ATH_MSG_WARNING("Failed to extrapolate to Beamline"); 
        if( !track->perigeeParameters() ){ 
          return 0;       
        } 
        aPer = track->perigeeParameters()->clone();   
      } else { 
        aPer = result; 
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
      } // else ATH_MSG_VERBOSE("Got Summary for Track" );
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
    if ( m_perigeeExpression == "Origin") {
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
          ATH_MSG_WARNING ("Could not extrapolate to 0,0,0. No TrackParticle created.");
          return 0;
        }
      }
    }else if (m_perigeeExpression == "BeamSpot"){ //Express parameters at beamspot 
      const Trk::Perigee* result = m_trackToVertex->perigeeAtBeamspot(track); 
      if(!result){ 
        ATH_MSG_WARNING("Failed to extrapolate to first Beamspot - No TrackParticle created."); 
	return 0;         
      }else{ 
        parsToBeDeleted = result; 
        aPer = result; 
      }
    } else if(m_perigeeExpression == "Vertex"){  // the non default way, express the perigee wrt. the vertex position
      if (vxCandidate != 0 ) {
        const Trk::Perigee* result =  m_trackToVertex->perigeeAtVertex(track, vxCandidate->position());
        if (result != 0) {
          parsToBeDeleted = result;
          aPer = result ;
        }else{
          ATH_MSG_WARNING ("Could not extrapolate track to vertex region! No TrackParticle created.");
          return 0;
        }
      }
      else{
	ATH_MSG_WARNING ("Perigee expression at Vertex, but no vertex found! No TrackParticle created.");
      }
    }
    else if (m_perigeeExpression == "BeamLine"){
      const Trk::Perigee* result = m_trackToVertex->perigeeAtBeamline(track); 
      if(!result){ 
	ATH_MSG_WARNING("Failed to extrapolate to Beamline - No TrackParticle created."); 
	return 0;        
      }
      else{ 
	parsToBeDeleted = result; 
	aPer = result; 
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

    int nbc_meas_A1=0;
    int nbc_meas_B3=0;
    int nbc_meas_A1_or_B3=0;
    int nbc_meas_A1_or_B3_or_C=0;

    int isBC_A1=0;
    int isBC_B3=0;
    int isBC_C=0;

    const DataVector<const TrackStateOnSurface>* trackStates = track.trackStateOnSurfaces();
    const Trk::TrackParameters* first(nullptr) ;
    const Trk::TrackParameters* tp(nullptr) ;

    if (m_badclusterID!=0) {
    for ( DataVector<const TrackStateOnSurface>::const_iterator itTSoS = trackStates->begin(); itTSoS != trackStates->end(); ++itTSoS) {
      if ( (*itTSoS)->type(TrackStateOnSurface::Measurement) && (*itTSoS)->trackParameters()!=0 &&
           (*itTSoS)->measurementOnTrack()!=0 && !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*itTSoS)->measurementOnTrack())) {
	tp = (*itTSoS)->trackParameters();

	const InDet::SiClusterOnTrack *clus = dynamic_cast< const InDet::SiClusterOnTrack*>((*itTSoS)->measurementOnTrack());
	if(!clus){
	  ATH_MSG_DEBUG( "Failed dynamic_cast to InDet::SiClusterOnTrack ");
	  continue;
	} 
	const Trk::PrepRawData* prdc = 0;
	prdc = clus->prepRawData();
	if(!prdc){
	  ATH_MSG_DEBUG( "No PRD for Si cluster" );
	}
	const InDet::SiCluster *RawDataClus = dynamic_cast< const InDet::SiCluster*>(clus->prepRawData());
	if(!RawDataClus){
	  ATH_MSG_DEBUG( "No RDC for Si cluster" );
	  continue;
	}
	const Trk::MeasurementBase* mesb=(*itTSoS)->measurementOnTrack();
	  
	if(RawDataClus->detectorElement()->isPixel())
	  {
	    const InDetDD::SiDetectorElement* element = NULL;
	    const InDet::PixelCluster* pixelCluster=dynamic_cast<const InDet::PixelCluster*>(RawDataClus);
	    if(!pixelCluster){
	      ATH_MSG_DEBUG( "Pixel cluster null though detector element matches pixel" );
	    }

	    else{
	      float size = pixelCluster->rdoList().size();
	      float tot = pixelCluster->totalToT();
	      float charge = pixelCluster->totalCharge();
	      float cotthetaz = -1;
	      int zWidth   = -1;

	      element = pixelCluster->detectorElement();
	      if (!element) ATH_MSG_DEBUG(  "No element for track incidence angles!");
	      float PixTrkAngle = -1000;
	      float PixTrkThetaI = -1000;
	      float theta = -1000;
	      if (element)
		{
		  Amg::Vector3D my_track = tp->momentum();
		  Amg::Vector3D my_normal = element->normal();
		  Amg::Vector3D my_phiax = element->phiAxis();
		  Amg::Vector3D my_etaax = element->etaAxis();
		  // track component on etaAxis:
		  float trketacomp = my_track.dot(my_etaax);
		  // track component on phiAxis:
		  float trkphicomp = my_track.dot(my_phiax);
		  // track component on the normal to the module
		  float trknormcomp = my_track.dot(my_normal);
		  // Track angle
		  PixTrkAngle = atan2(trkphicomp,trknormcomp);
		  PixTrkThetaI = atan2(trketacomp,trknormcomp);
		  float length=sqrt(trketacomp*trketacomp + trkphicomp*trkphicomp + trknormcomp*trknormcomp);
		  theta=acos(trknormcomp/length);
		  cotthetaz = 1./tan(PixTrkThetaI);

		  // reducing the angle in the right quadrant
		  if(PixTrkThetaI > M_PI / 2 ) PixTrkThetaI -= M_PI;
		  else if(PixTrkThetaI < - M_PI / 2 ) PixTrkThetaI += M_PI;
		  PixTrkThetaI = ( M_PI / 2 ) - PixTrkThetaI;
		  if(PixTrkAngle > M_PI / 2 ) PixTrkAngle -= M_PI;
		  else if(PixTrkAngle < - M_PI / 2 ) PixTrkAngle += M_PI;
		  PixTrkAngle = ( M_PI / 2 ) - PixTrkAngle;
		  if(theta>M_PI/2){theta = M_PI-theta;}
		}
	      
	      Identifier surfaceID;
	      surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
	      if (m_detID->is_pixel(surfaceID))
		{
		  InDet::SiWidth width = pixelCluster->width();
		  zWidth = int(width.colRow().y());
		}

	      int isIBLclus =false;
	      if(m_doIBL && m_pixelID->barrel_ec(surfaceID) == 0 && m_pixelID->layer_disk(surfaceID) == 0){isIBLclus = true;}

	      //count bad clusters
	      if(!isIBLclus){
		if((size==1 && tot<8) || (size==2 && tot<15)){
		  isBC_A1=true;
		  nbc_meas_A1++;
		}
		if(charge<13750/cos(theta)-22500){
		  isBC_B3=true;
		  nbc_meas_B3++;
		}
		if(isBC_A1 || isBC_B3){
		  nbc_meas_A1_or_B3++;
		}
		if((zWidth==1 && cotthetaz>5.8) || (zWidth==2 && cotthetaz>5.8) || (zWidth==3 && cotthetaz>6.2) || (zWidth>3 && cotthetaz<2.5)){
		  isBC_C=true;
		}
		if(isBC_A1 || isBC_B3 || isBC_C){
		  nbc_meas_A1_or_B3_or_C++;
		}
	      }
	    }
	  }
      }

    }
    }
    if (m_keepParameters || m_keepFirstParameters) {
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

      if (!m_keepFirstParameters) {
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
    switch (m_badclusterID) {
    case 1: {
      trackparticle->auxdecor<int>("nBC_meas")=nbc_meas_A1;
      break;
    }
    case 2: {
      trackparticle->auxdecor<int>("nBC_meas")=nbc_meas_B3;
      break;
    }
    case 3: {
      trackparticle->auxdecor<int>("nBC_meas")=nbc_meas_A1_or_B3;
      break;
    }
    case 4: {
      trackparticle->auxdecor<int>("nBC_meas")=nbc_meas_A1_or_B3_or_C;
      break;
    }
    default: {
    }
    }

    if (m_trackSummaryTool!=0 && m_useTrackSummaryTool) delete summary;
    delete parsToBeDeleted;
    return trackparticle;

  }
  
  xAOD::TrackParticle* TrackParticleCreatorTool::createParticle( const Rec::TrackParticle& trackParticle, xAOD::TrackParticleContainer* container ) const {
    
    // Attempt to fill the position enums - will necessarily be a bit of a hack, since we don't have all the information.
    std::vector< xAOD::ParameterPosition> positions;
    bool firstMeasurement = false;
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
     
    if(!trackparticle){
      ATH_MSG_WARNING( "WARNING: Problem creating TrackParticle - Returning 0");
      return 0;
    }

    trackparticle->setTrackLink( *(trackParticle.trackElementLink()) );

    if( m_checkConversion ) compare(trackParticle,*trackparticle);
    
    return trackparticle;
  }
  
  xAOD::TrackParticle* TrackParticleCreatorTool::createParticle( const ElementLink<TrackCollection>& trackLink,
                                                                xAOD::TrackParticleContainer* container,
                                                                const xAOD::Vertex* vxCandidate,
                                                                xAOD::ParticleHypothesis prtOrigin) const {
   
    xAOD::TrackParticle* trackparticle = createParticle( **trackLink, container, vxCandidate, prtOrigin );
 
     if(!trackparticle){
      ATH_MSG_WARNING( "WARNING: Problem creating TrackParticle - Returning 0");
      return 0;
    }
   
    trackparticle->setTrackLink( trackLink );
    
    return trackparticle;
  }
  
  
  xAOD::TrackParticle* TrackParticleCreatorTool::createParticle( const Perigee* perigee, const FitQuality* fq, const TrackInfo* trackInfo, const TrackSummary* summary,
                                                                const std::vector<const Trk::TrackParameters*>& parameters, 
                                                                const std::vector< xAOD::ParameterPosition>& positions,
                                                                xAOD::ParticleHypothesis prtOrigin,
                                                                xAOD::TrackParticleContainer* container ) const {

    xAOD::TrackParticle* trackparticle = new xAOD::TrackParticle;
   

     if(!trackparticle){
      ATH_MSG_WARNING( "WARNING: Problem creating TrackParticle - Returning 0");
      return 0;
     }

    if( container ) container->push_back( trackparticle );
    else trackparticle->makePrivateStore();

    // Fit quality
    if( fq ) setFitQuality(*trackparticle,*fq);
    
    // Track Info
    if( trackInfo ) setTrackInfo(*trackparticle,*trackInfo,prtOrigin);
    
    // track summary
    if (summary){
      setTrackSummary(*trackparticle,*summary);
      setHitPattern(*trackparticle,summary->getHitPattern());
      setNumberOfUsedHits(*trackparticle,summary->numberOfUsedHitsdEdx());
      setNumberOfOverflowHits(*trackparticle,summary->numberOfOverflowHitsdEdx());
    }
    
    if (!m_beamConditionsService.empty()) setTilt(*trackparticle,m_beamConditionsService->beamTilt(0),m_beamConditionsService->beamTilt(1));
    
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
      if (positions[i]==xAOD::FirstMeasurement){
        float x_position = tp.parameterX(i);
        float y_position = tp.parameterY(i);
        tp.setRadiusOfFirstHit(std::sqrt(x_position*x_position + y_position*y_position));
        tp.setIdentifierOfFirstHit(parameters[i]->associatedSurface().associatedDetectorElementIdentifier().get_compact());
      }
    }
  }

  void TrackParticleCreatorTool::setTilt( xAOD::TrackParticle& tp, float tiltx, float tilty ) const {
    tp.setBeamlineTiltX(tiltx);
    tp.setBeamlineTiltY(tilty);
  }  

 void TrackParticleCreatorTool::setHitPattern( xAOD::TrackParticle& tp, unsigned long hitpattern ) const {
    tp.setHitPattern(hitpattern);
  }  

  void TrackParticleCreatorTool::setNumberOfUsedHits( xAOD::TrackParticle& tp, int hits ) const {
    tp.setNumberOfUsedHitsdEdx(hits);
  }

  void TrackParticleCreatorTool::setNumberOfOverflowHits( xAOD::TrackParticle& tp, int overflows ) const {
    tp.setNumberOfIBLOverflowsdEdx(overflows);
  }

void TrackParticleCreatorTool::setTrackSummary( xAOD::TrackParticle& tp, const TrackSummary& summary ) const {
    // int types
  unsigned int offset = 47;// where the floats start in xAOD::SummaryType

  // ensure that xAOD TrackSummary and TrackSummary enums are in sync. 
  constexpr unsigned int xAodReferenceEnum=static_cast<unsigned int>(xAOD::pixeldEdx);
  constexpr unsigned int TrkReferenceEnum=static_cast<unsigned int>(Trk::pixeldEdx_res);
  static_assert( xAodReferenceEnum == TrkReferenceEnum, "Trk and xAOD enums differ in their indices" );
  
  for (unsigned int i =0 ; i<Trk::numberOfTrackSummaryTypes ; i++){
      // Only add values which are +ve (i.e., which were created)
    if( i >= Trk::numberOfMdtHits && i <= Trk::numberOfRpcEtaHits ) continue;  
    if( i == Trk::numberOfCscUnspoiltEtaHits ) continue;
    if( i >= Trk::numberOfCscEtaHoles && i <= Trk::numberOfTgcPhiHoles ) continue;
    if ( i >= offset && i < offset+Trk::numberOfeProbabilityTypes+1){
      continue;
    }
    if( i >= Trk::numberOfStgcEtaHits && i <= Trk::numberOfMmHoles) continue;
    if( !m_doITk && i >= Trk::numberOfContribPixelBarrelFlatLayers && i <= Trk::numberOfPixelEndcapHoles) continue;
    // coverity[mixed_enums]
    if (i == Trk::numberOfTRTHitsUsedFordEdx ) continue;

    int value = summary.get(static_cast<Trk::SummaryType>(i));
    uint8_t uvalue = static_cast<uint8_t>(value);
    // coverity[first_enum_type]
    if (value>0) tp.setSummaryValue(uvalue, static_cast<xAOD::SummaryType>(i));
  }

  // first eProbabilities which are set in the xAOD track summary
  for( Trk::eProbabilityType copy : m_copyEProbabilities ) {
    float fvalue = summary.getPID(copy);
    tp.setSummaryValue(fvalue, static_cast<xAOD::SummaryType>(copy+xAOD::eProbabilityComb));
  }

  // now the eProbabilities which are set as a decoration.
  for( const std::pair<SG::AuxElement::Decorator<float>,Trk::eProbabilityType> &decoration :  m_decorateEProbabilities ) {
    float fvalue = summary.getPID(decoration.second);
    decoration.first(tp) = fvalue;
  }

  // now the extra summary types
  for( const std::pair<SG::AuxElement::Decorator<uint8_t>,Trk::SummaryType>  &decoration :  m_decorateSummaryTypes) {
    uint8_t summary_value = summary.get(decoration.second);
    decoration.first(tp) = summary_value;
  }

  // // float types
  // for (unsigned int i =0 ; i<Trk::numberOfeProbabilityTypes ; i++){
  //   float fvalue = summary.getPID(static_cast<Trk::eProbabilityType>(i));
  //   tp.setSummaryValue(fvalue, static_cast<xAOD::SummaryType>(i+offset));
  // }

  //this one is "special" so gets a different treatment...
  float fvalue = summary.getPixeldEdx();
  tp.setSummaryValue(fvalue, static_cast<xAOD::SummaryType>(51));

  //muon hit info
  if(m_useMuonSummaryTool){
    ATH_MSG_DEBUG("now do muon hit info");
    Muon::IMuonHitSummaryTool::CompactSummary msSummary = m_hitSummaryTool->summary(summary);
    uint8_t numberOfPrecisionLayers = msSummary.nprecisionLayers;
    ATH_MSG_DEBUG("# of prec layers: "<<numberOfPrecisionLayers);
    uint8_t numberOfPrecisionHoleLayers = msSummary.nprecisionHoleLayers;
    uint8_t numberOfPhiLayers = msSummary.nphiLayers;
    uint8_t numberOfPhiHoleLayers = msSummary.nphiHoleLayers;
    uint8_t numberOfTriggerEtaLayers = msSummary.ntrigEtaLayers;
    uint8_t numberOfTriggerEtaHoleLayers = msSummary.ntrigEtaHoleLayers;
    tp.setSummaryValue(numberOfPrecisionLayers,xAOD::numberOfPrecisionLayers);
    tp.setSummaryValue(numberOfPrecisionHoleLayers,xAOD::numberOfPrecisionHoleLayers);
    tp.setSummaryValue(numberOfPhiLayers,xAOD::numberOfPhiLayers);
    tp.setSummaryValue(numberOfPhiHoleLayers,xAOD::numberOfPhiHoleLayers);
    tp.setSummaryValue(numberOfTriggerEtaLayers,xAOD::numberOfTriggerEtaLayers);
    tp.setSummaryValue(numberOfTriggerEtaHoleLayers,xAOD::numberOfTriggerEtaHoleLayers);
  }
}

} // end of namespace Trk
