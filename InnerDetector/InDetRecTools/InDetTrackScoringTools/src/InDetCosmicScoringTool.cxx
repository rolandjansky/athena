/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackScoringTools/InDetCosmicScoringTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "TMath.h"
#include <vector>

//---------------------------------------------------------------------------------------------------------------------

InDet::InDetCosmicScoringTool::InDetCosmicScoringTool(const std::string& t,
						  const std::string& n,
						  const IInterface*  p ) :
  AthAlgTool(t,n,p)
{
  declareInterface<Trk::ITrackScoringTool>(this);
  declareProperty("SummaryTool",          m_trkSummaryTool);  
  declareProperty("nWeightedClustersMin", m_nWeightedClustersMin = 0); 
  declareProperty("minTRTHits",           m_minTRTHits = 0);
}

//---------------------------------------------------------------------------------------------------------------------

InDet::InDetCosmicScoringTool::~InDetCosmicScoringTool()
{
}

//---------------------------------------------------------------------------------------------------------------------

StatusCode InDet::InDetCosmicScoringTool::initialize()
{
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  
  sc = m_trkSummaryTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSummaryTool << endmsg;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved tool " << m_trkSummaryTool << endmsg;
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------

StatusCode InDet::InDetCosmicScoringTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//---------------------------------------------------------------------------------------------------------------------

Trk::TrackScore InDet::InDetCosmicScoringTool::score( const Trk::Track& track, const bool suppressHoleSearch ) const
{
  (void) suppressHoleSearch;
  if (!track.trackSummary()) {
     ATH_MSG_FATAL("Track without a summary");
  }
  ATH_MSG_VERBOSE( "Track has TrackSummary "<< *track.trackSummary() );
  Trk::TrackScore score = Trk::TrackScore( simpleScore(track, *track.trackSummary()) );
  ATH_MSG_DEBUG( "Track has Score: "<<score );
  return score;
}

//---------------------------------------------------------------------------------------------------------------------

Trk::TrackScore InDet::InDetCosmicScoringTool::simpleScore( const Trk::Track& track, const Trk::TrackSummary& trackSummary ) const
{
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Summary for track: " << trackSummary << endmsg;

  if (!track.fitQuality()){
    msg(MSG::WARNING) << "No fit quality! Track info=" << track.info().dumpInfo() << endmsg;
    return Trk::TrackScore(track.measurementsOnTrack()->size());
  }
  else {
    int pixelhits = trackSummary.get(Trk::numberOfPixelHits); 
    int scthits   = trackSummary.get(Trk::numberOfSCTHits); 
    int trthits   = trackSummary.get(Trk::numberOfTRTHits); 
    
    int nWeightedClusters = 2 * pixelhits + scthits; 
    
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "pixelhits: " << pixelhits << "; scthits: " << scthits 
				    << "; trthits: " << trthits << "; nWeightedClusters: " << nWeightedClusters << endmsg; 
    
    if ((nWeightedClusters >= m_nWeightedClustersMin) and (trthits >= m_minTRTHits)){ 
      // calculate track score only if min number of hits
      int tubehits=0,drifthits=0;
      for (unsigned int i=0;i<track.measurementsOnTrack()->size();i++){
	if (dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>((*track.measurementsOnTrack())[i])){
	  double error=sqrt((*track.measurementsOnTrack())[i]->localCovariance()(0,0));
	  if (error>1) tubehits++;
	  else drifthits++;
	  
	}
      }
      int hitscore = 0;
      if (m_nWeightedClustersMin > 0) hitscore = 10 * nWeightedClusters + trthits;
      else hitscore =  10 * (pixelhits + scthits) + trthits;

      double fitscore = 0;
      if (track.fitQuality()->numberDoF() > 0) fitscore = 0.0001*track.fitQuality()->chiSquared()
						 / track.fitQuality()->numberDoF(); 
       
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "track score: " << hitscore - 0.25*tubehits - fitscore << endmsg; 
      return Trk::TrackScore (hitscore - 0.25*tubehits - fitscore);

    }
    else return Trk::TrackScore( 0 );
  }  
}

//---------------------------------------------------------------------------------------------------------------------

