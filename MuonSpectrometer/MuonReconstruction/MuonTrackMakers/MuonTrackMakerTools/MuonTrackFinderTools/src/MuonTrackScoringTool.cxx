/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "MuonTrackScoringTool.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "GaudiKernel/MsgStream.h"

//#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <cassert>
#include <vector>

namespace Muon {

  MuonTrackScoringTool::MuonTrackScoringTool(const std::string& t,
					     const std::string& n,
					     const IInterface*  p )
    :
    AthAlgTool(t,n,p),
    m_trkSummaryTool("Trk::TrackSummaryTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_summaryTypeScore(Trk::numberOfTrackSummaryTypes)
  {
    declareInterface<Trk::ITrackScoringTool>(this);
    declareProperty("SumHelpTool",          m_trkSummaryTool);
	
    //set some test values
    m_summaryTypeScore[Trk::numberOfPixelHits]	      =  20;
    m_summaryTypeScore[Trk::numberOfPixelSharedHits]  = -10;  // a shared hit is only half the weight
    m_summaryTypeScore[Trk::numberOfPixelHoles]	      = -10;  // a hole is bad

    m_summaryTypeScore[Trk::numberOfInnermostPixelLayerHits]	      =  10;  // addition for being b-layer
    m_summaryTypeScore[Trk::numberOfInnermostPixelLayerSharedHits] =  -5;  // a shared hit is only half the weight

    m_summaryTypeScore[Trk::numberOfGangedPixels]     =  -5;  // decrease for being ganged

    m_summaryTypeScore[Trk::numberOfSCTHits]	      =  10;  // half of a pixel, since only 1dim
    m_summaryTypeScore[Trk::numberOfSCTSharedHits]    =  -5;  // a shared hit is only half the weight
    m_summaryTypeScore[Trk::numberOfSCTHoles]	      =  -5;  // a hole is bad !

    m_summaryTypeScore[Trk::numberOfTRTHits]	          =   2;  // 5 straws ~ 1 SCT
    m_summaryTypeScore[Trk::numberOfTRTHighThresholdHits] =   1;  // addition for being TR
    m_summaryTypeScore[Trk::numberOfOutliersOnTrack]      =  -2;  // an outlier might happen

    // scoring for Muons is missing
    m_summaryTypeScore[Trk::numberOfMdtHits]	= 5;   
    m_summaryTypeScore[Trk::numberOfTgcPhiHits]	= 5; 
    m_summaryTypeScore[Trk::numberOfTgcEtaHits]	= 5;
    m_summaryTypeScore[Trk::numberOfCscPhiHits]	= 5;     
    m_summaryTypeScore[Trk::numberOfCscEtaHits]	= 5;
    m_summaryTypeScore[Trk::numberOfRpcPhiHits]	= 5;
    m_summaryTypeScore[Trk::numberOfRpcEtaHits]	= 5;
  }

  MuonTrackScoringTool::~MuonTrackScoringTool()
  {
  }

  StatusCode MuonTrackScoringTool::initialize()
  {
    StatusCode sc = AlgTool::initialize();
    if (sc.isFailure()) return sc;
    
    sc = m_trkSummaryTool.retrieve();
    if (sc.isFailure()) 
      {
	ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSummaryTool);
	return sc;
      } 
    else 
      ATH_MSG_DEBUG("Retrieved tool " << m_trkSummaryTool);

    return StatusCode::SUCCESS;
  }

  StatusCode MuonTrackScoringTool::finalize()
  {
    StatusCode sc = AlgTool::finalize();
    return sc;
  }

  Trk::TrackScore MuonTrackScoringTool::score( const Trk::Track& track, const bool suppressHoleSearch ) const
  {

    const Trk::TrackSummary* summary = 0;
    if (suppressHoleSearch)
      summary = m_trkSummaryTool->createSummaryNoHoleSearch(track);
    else
      summary = m_trkSummaryTool->createSummary(track);

    //log <<MSG::DEBUG<<"Track has TrackSummary "<<*summary<<endmsg;
    Trk::TrackScore score = Trk::TrackScore( simpleScore(track, *summary) );
    delete summary;
    return score;
  }

  Trk::TrackScore MuonTrackScoringTool::simpleScore( const Trk::Track& track, const Trk::TrackSummary& trackSummary ) const
  {
	
    // --- reject bad tracks
    if (track.fitQuality() && track.fitQuality()->numberDoF() < 0) {
      ATH_MSG_VERBOSE("numberDoF < 0, reject it");
      return Trk::TrackScore(0);
    }
    
    ATH_MSG_DEBUG(m_printer->print(track));
	 
    // --- now start scoring
    Trk::TrackScore score(200); // score of 100 per track

    // --- prob(chi2,NDF), protect for chi2<0
    if (track.fitQuality()!=0 && track.fitQuality()->chiSquared() > 0 && track.fitQuality()->numberDoF() > 0) {
      score += 5*track.fitQuality()->numberDoF() - track.fitQuality()->chiSquared();
    }

    // --- summary score analysis
    for (int i=0; i<Trk::numberOfTrackSummaryTypes; ++i) 
      {
	int value = trackSummary.get(static_cast<Trk::SummaryType>(i));
	//value is -1 if undefined.
	if (value>0) { 
	  score+=m_summaryTypeScore[i]*value; 
	  ATH_MSG_VERBOSE("\tType ["<<i<<"], value \t= "<<value<<"], score \t="<<score);
	}
      }
    ATH_MSG_DEBUG(" Track Score " << score);

    return score;
  }
}
