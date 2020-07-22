/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrackScoringTool.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <cmath> //for log10

Trk::TrackScoringTool::TrackScoringTool(const std::string& t,
					const std::string& n,
					const IInterface*  p )
					:
					AthAlgTool(t,n,p),
					m_trkSummaryTool("Trk::TrackSummaryTool"),
					m_summaryTypeScore(Trk::numberOfTrackSummaryTypes)
{
	declareInterface<ITrackScoringTool>(this);
	declareProperty("SumHelpTool",          m_trkSummaryTool);

	//set some test values
	m_summaryTypeScore[Trk::numberOfPixelHits]	      =  20;
	m_summaryTypeScore[Trk::numberOfPixelSharedHits]      = -10;  // a shared hit is only half the weight
	m_summaryTypeScore[Trk::numberOfPixelHoles]	      = -10;  // a hole is bad

	m_summaryTypeScore[Trk::numberOfInnermostPixelLayerHits]	      =  10;  // addition for being b-layer
	m_summaryTypeScore[Trk::numberOfInnermostPixelLayerSharedHits]     =  -5;  // a shared hit is only half the weight

	m_summaryTypeScore[Trk::numberOfGangedPixels]	      =  -5;  // decrease for being ganged

	m_summaryTypeScore[Trk::numberOfSCTHits]	      =  10;  // half of a pixel, since only 1dim
	m_summaryTypeScore[Trk::numberOfSCTSharedHits]        =  -5;  // a shared hit is only half the weight
	m_summaryTypeScore[Trk::numberOfSCTHoles]	      =  -5;  // a hole is bad !

	m_summaryTypeScore[Trk::numberOfTRTHits]	      =   2;  // 5 straws ~ 1 SCT
	m_summaryTypeScore[Trk::numberOfTRTHighThresholdHits] =   1;  // addition for being TR

	m_summaryTypeScore[Trk::numberOfOutliersOnTrack]      =  -2;  // an outlier might happen

	// scoring for Muons is missing
	m_summaryTypeScore[Trk::numberOfMdtHits]	= 20;
	m_summaryTypeScore[Trk::numberOfTgcPhiHits]	= 20;
	m_summaryTypeScore[Trk::numberOfTgcEtaHits]	= 10;
	m_summaryTypeScore[Trk::numberOfCscPhiHits]	= 20;
	m_summaryTypeScore[Trk::numberOfCscEtaHits]	= 20;
	m_summaryTypeScore[Trk::numberOfRpcPhiHits]	= 20;
	m_summaryTypeScore[Trk::numberOfRpcEtaHits]	= 10;
}

Trk::TrackScoringTool::~TrackScoringTool(){
}

StatusCode
Trk::TrackScoringTool::initialize(){
	ATH_CHECK( AlgTool::initialize());
	ATH_CHECK( m_trkSummaryTool.retrieve());
	ATH_MSG_VERBOSE("Retrieved tool " << m_trkSummaryTool );
	return StatusCode::SUCCESS;
}

StatusCode Trk::TrackScoringTool::finalize(){
	return AlgTool::finalize();
}

Trk::TrackScore
Trk::TrackScoringTool::score( const Track& track, const bool suppressHoleSearch ) const{

  // Used to call the now removed not MT safe
  // summary = m_trkSummaryTool->createSummary(track);
  // We keep the non-thread safe logic only here.
  // Locally to where is actually used
  // To decide for a better design
  //
  Trk::Track& nonConstTrack = const_cast<Trk::Track&>(track);
  if (suppressHoleSearch) {
    nonConstTrack.setTrackSummary(m_trkSummaryTool->summaryNoHoleSearch(track));
  } else {
    nonConstTrack.setTrackSummary(m_trkSummaryTool->summary(track));
  }
  Trk::TrackScore score = TrackScore(simpleScore(track, *(nonConstTrack.trackSummary())));
  return score;
}

Trk::TrackScore
Trk::TrackScoringTool::simpleScore( const Track& track, const TrackSummary& trackSummary ) const{
	// --- reject bad tracks
	if (track.fitQuality() && track.fitQuality()->numberDoF() < 0) {
	  ATH_MSG_VERBOSE("numberDoF < 0, reject it");
	  return TrackScore(0);
	}
	// --- now start scoring
	TrackScore score(100); // score of 100 per track

	// --- prob(chi2,NDF), protect for chi2<0
	if (track.fitQuality()!=nullptr && track.fitQuality()->chiSquared() > 0 && track.fitQuality()->numberDoF() > 0) {
	  score+= std::log10(1.0-Genfun::CumulativeChiSquare(track.fitQuality()->numberDoF())(track.fitQuality()->chiSquared()));
	}

	// --- summary score analysis
	for (int i=0; i<Trk::numberOfTrackSummaryTypes; ++i) {
		int value = trackSummary.get(static_cast<Trk::SummaryType>(i));
		//value is -1 if undefined.
		if (value>0) {
		  score+=m_summaryTypeScore[i]*value;
		  ATH_MSG_VERBOSE("\tType ["<<i<<"], value \t= "<<value<<"], score \t="<<score);
		}
	}
	return score;
}
