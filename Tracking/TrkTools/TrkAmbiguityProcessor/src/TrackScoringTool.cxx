/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrackScoringTool.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <cassert>
#include <vector>

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

Trk::TrackScoringTool::~TrackScoringTool()
{
}

StatusCode Trk::TrackScoringTool::initialize()
{
	StatusCode sc = AlgTool::initialize();
	if (sc.isFailure()) return sc;

	sc = m_trkSummaryTool.retrieve();
	if (sc.isFailure()) 
	  {
	    msg(MSG::FATAL)<< "Failed to retrieve tool " << m_trkSummaryTool << endmsg;
	    return sc;
	  } 
	else 
	  msg(MSG::INFO)<< "Retrieved tool " << m_trkSummaryTool << endmsg;

	return StatusCode::SUCCESS;
}

StatusCode Trk::TrackScoringTool::finalize()
{
	StatusCode sc = AlgTool::finalize();
	return sc;
}

Trk::TrackScore Trk::TrackScoringTool::score( const Track& track, const bool suppressHoleSearch ) const
{
	const TrackSummary* summary = 0;
	if (suppressHoleSearch)
	  summary = m_trkSummaryTool->createSummaryNoHoleSearch(track);
	else
	  summary = m_trkSummaryTool->createSummary(track);

	Trk::TrackScore score = TrackScore( simpleScore(track, *summary) );
	delete summary;
	return score;
}

Trk::TrackScore Trk::TrackScoringTool::simpleScore( const Track& track, const TrackSummary& trackSummary ) const
{
	
	
	// --- reject bad tracks
	if (track.fitQuality() && track.fitQuality()->numberDoF() < 0) {
	  msg(MSG::VERBOSE)<<"numberDoF < 0, reject it"<<endmsg;
	  return TrackScore(0);
	}
	 
	// --- now start scoring
	TrackScore score(100); // score of 100 per track

	// --- prob(chi2,NDF), protect for chi2<0
	if (track.fitQuality()!=0 && track.fitQuality()->chiSquared() > 0 && track.fitQuality()->numberDoF() > 0) {
	  score+= log10(1.0-Genfun::CumulativeChiSquare(track.fitQuality()->numberDoF())(track.fitQuality()->chiSquared()));
	}

	// --- summary score analysis
	for (int i=0; i<Trk::numberOfTrackSummaryTypes; ++i) 
	{
		int value = trackSummary.get(static_cast<Trk::SummaryType>(i));
		//value is -1 if undefined.
		if (value>0) { 
		  score+=m_summaryTypeScore[i]*value; 
		  msg(MSG::VERBOSE)<<"\tType ["<<i<<"], value \t= "<<value<<"], score \t="<<score<<endmsg;
		}
	}
	return score;
}
