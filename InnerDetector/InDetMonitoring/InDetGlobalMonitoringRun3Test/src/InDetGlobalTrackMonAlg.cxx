/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalTrackMonAlg.h
 * Implementation of inner detector global track monitoring tool
 *
 *@author
 * Leonid Serkin <lserkin@cern.ch> @n
 * Per Johansson <Per.Johansson@cern.ch> @n
 *
 * based on InDetGlobalTrackMonTool.cxx
 *
 ****************************************************************************/

//main header
#include "InDetGlobalTrackMonAlg.h"

//Standard c++
#include <vector>
#include <memory>

// Id includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"



InDetGlobalTrackMonAlg::InDetGlobalTrackMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_holes_search_tool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool", this),
  m_trackSelTool      ( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this),
  m_tight_trackSelTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this ),
  m_atlasid(nullptr),
  m_trtID(nullptr),
  m_sctID(nullptr),
  m_pixelID(nullptr),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_doLumiblock(true),
  m_doHolePlots(false),
  m_DoHoles_Search(false),
  m_doHitMaps(false),
  m_doTide(false),
  m_doTideResiduals(false),
  m_doForwardTracks(false),
  m_doIBL(true)
{
  //jo flags go here, keys and some tools -> in class
  declareProperty("DoHoleSearch",m_DoHoles_Search,"Write hole data?");
  declareProperty("DoTide",m_doTide,"Make TIDE plots?");
  declareProperty("DoTideResiduals",m_doTideResiduals,"Make TIDE residual plots?");
  declareProperty("HoleSearchTool", m_holes_search_tool,"Tool to search for holes on track");	
  //    declareProperty("UpdatorTool"                  , m_iUpdator);
  declareProperty("DoHitMaps", m_doHitMaps,"Produce hit maps?");	
  declareProperty("DoForwardTracks", m_doForwardTracks,"Run over forward tracks?");	
  declareProperty("DoIBL", m_doIBL,"IBL present?");	
  //    declareProperty("ResidualPullCalculatorTool", m_residualPullCalculator);
  declareProperty( "TrackSelectionTool", m_trackSelTool);
  declareProperty( "Tight_TrackSelectionTool", m_tight_trackSelTool );
}


InDetGlobalTrackMonAlg::~InDetGlobalTrackMonAlg() {}


StatusCode InDetGlobalTrackMonAlg::initialize() {
  
  ATH_CHECK( detStore()->retrieve(m_atlasid, "AtlasID") );
  
  m_pixelID = nullptr;
  if (detStore()->retrieve(m_pixelID, "PixelID").isFailure()) {
    ATH_MSG_DEBUG("InDetGlobalMonitoringRun3Test: Could not get Pixel ID helper");
    m_doHitMaps = false;
  }
  
  m_sctID = nullptr;
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()) {
    ATH_MSG_DEBUG("InDetGlobalMonitoringRun3Test: Could not get SCT ID helper");
    m_doHitMaps = false;
  }
  
  m_trtID = nullptr;
  if (detStore()->retrieve(m_trtID, "TRT_ID").isFailure()) {
    ATH_MSG_DEBUG("InDetGlobalMonitoringRun3Test: Could not get TRT ID helper");
    m_doHitMaps = false;
  }
  
  
  ATH_CHECK( m_pixelCondSummaryTool.retrieve() ); 
  
  ATH_CHECK( m_IBLParameterSvc.retrieve() ); 
  
  m_doIBL = m_IBLParameterSvc->containsIBL();
  
  if (!m_trackSelTool.empty() )      ATH_CHECK( m_trackSelTool.retrieve() );
  if (!m_tight_trackSelTool.empty()) ATH_CHECK( m_tight_trackSelTool.retrieve() );
  
  if (!m_holes_search_tool.empty())  ATH_CHECK( m_holes_search_tool.retrieve());
  
  ATH_CHECK( m_CombinedTracksName.initialize() );
  ATH_CHECK( m_ForwardTracksName.initialize() );
  
  ATH_CHECK( m_clustersKey.initialize() );  // maybe not needed
  
  return AthMonitorAlgorithm::initialize();
}


StatusCode InDetGlobalTrackMonAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  
  //*******************************************************************************
  //************************** Begin of filling Track Histograms ******************
  //*******************************************************************************
  ATH_MSG_DEBUG("Filling InDetGlobalTrackMonAlg");
  
  // For histogram naming
  auto trackGroup = getGroup("Track");
  
  // m_manager->lumiBlockNumber() // not used anymore, now use
  int lb       = GetEventInfo(ctx)->lumiBlock();
  auto lb_m    = Monitored::Scalar<int>( "m_lb", lb );
  
  // retrieving tracks
  auto combined_tracks = SG::makeHandle(m_CombinedTracksName, ctx);
  
  // check for tracks
  if ( !(combined_tracks.isValid()) ) {
    ATH_MSG_ERROR("InDetGlobalMonitoringRun3Test: Track container "<< m_CombinedTracksName.key() << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("InDetGlobalMonitoringRun3Test: Track container "<< combined_tracks.name() <<" is found.");
  }
  
  // counters
  int nBase = 0;
  int nTight = 0;
  int nNoIBL = 0;
  int nNoBL = 0;
  int nNoTRText = 0;
  
  
  for (const Trk::Track* track: *combined_tracks) {
    
    if ( !track || track->perigeeParameters() == 0 )
      {
	ATH_MSG_DEBUG( "InDetGlobalMonitoringRun3Test: NULL track pointer in collection" );
	continue;
      }
    
    // not sure it works now....
    // Skip tracks that are not inside out
    //if ( ( m_dataType == AthenaMonManager::collisions || m_dataType == AthenaMonManager::userDefined )
    //     && ! track->info().patternRecoInfo( Trk::TrackInfo::SiSPSeededFinder ) )
    //    continue;
    
    
    // Loose primary tracks
    if ( !m_trackSelTool->accept(*track) )
      continue;
    
    // Create a new summary or get copy of the cached one
    std::unique_ptr<const Trk::TrackSummary> summary(m_trkSummaryTool->summary( * track ) );
    
    if ( !summary )
      {
	ATH_MSG_DEBUG( "InDetGlobalMonitoringRun3Test: NULL pointer to track summary" );
	continue;
      }
    
    nBase++;
    
    
    // =================================== //
    // Fill hits BEGINS
    int numberOfPixelHits = ( summary->get(Trk::numberOfPixelHits) >= 0 ) ? summary->get(Trk::numberOfPixelHits) : 0 ;
    int numberOfPixelDeadSensors = ( summary->get(Trk::numberOfPixelDeadSensors) >= 0 ) ? summary->get(Trk::numberOfPixelDeadSensors ) : 0 ;
    int pixHits = numberOfPixelHits + numberOfPixelDeadSensors;
    
    int numberOfSCTHits =  ( summary->get(Trk::numberOfSCTHits) >= 0 ) ? summary->get(Trk::numberOfSCTHits) : 0  ;
    int numberOfSCTDeadSensors =  ( summary->get(Trk::numberOfSCTDeadSensors) >= 0 ) ? summary->get(Trk::numberOfSCTDeadSensors) : 0 ; 
    int sctHits = numberOfSCTHits + numberOfSCTDeadSensors;
    
    int trtHits = summary->get(Trk::numberOfTRTHits) + summary->get(Trk::numberOfTRTDeadStraws);
    
    
    const Trk::Perigee *perigee = track->perigeeParameters();
    if ( !perigee )  
      {
	ATH_MSG_DEBUG( "InDetGlobalMonitoringRun3Test: NULL track->perigeeParameters pointer " );
	continue;
      }
    
    float eta_perigee = perigee->eta();
    float phi_perigee = perigee->parameters()[Trk::phi0];
    
    auto eta_perigee_m   = Monitored::Scalar<float>( "m_eta_perigee", eta_perigee);
    auto phi_perigee_m   = Monitored::Scalar<float>( "m_phi_perigee", phi_perigee);
    fill(trackGroup, eta_perigee_m, phi_perigee_m); // Trk_Base_eta_phi
    
    
    if ( m_doIBL )
      {
	int numberOfInnermostPixelLayerHits = summary->get( Trk::numberOfInnermostPixelLayerHits );
	auto numberOfInnermostPixelLayerHits_m = Monitored::Scalar<int>( "m_numberOfInnermostPixelLayerHits", numberOfInnermostPixelLayerHits);
	fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfInnermostPixelLayerHits_m);
	
	fill(trackGroup, lb_m, numberOfInnermostPixelLayerHits_m);
      }
    
    auto pixHits_m  = Monitored::Scalar<int>( "m_pixHits", pixHits ); 
    fill(trackGroup, eta_perigee_m, phi_perigee_m, pixHits_m);
    fill(trackGroup, lb_m, pixHits_m);
    
    auto numberOfPixelDeadSensors_m = Monitored::Scalar<int>( "m_numberOfPixelDeadSensors", numberOfPixelDeadSensors );
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfPixelDeadSensors_m);
    
    int numberOfPixelSharedHits = ( summary->get(Trk::numberOfPixelSharedHits) >= 0 ) ? summary->get(Trk::numberOfPixelSharedHits) : 0 ;
    auto numberOfPixelSharedHits_m = Monitored::Scalar<int>( "m_numberOfPixelSharedHits", numberOfPixelSharedHits);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfPixelSharedHits_m);
    
    int numberOfPixelHoles = summary->get(Trk::numberOfPixelHoles) >= 0 ? summary->get(Trk::numberOfPixelHoles) : 0;
    auto numberOfPixelHoles_m = Monitored::Scalar<int>( "m_numberOfPixelHoles", numberOfPixelHoles); 
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfPixelHoles_m);
    
    int numberOfPixelSplitHits = ( summary->get(Trk::numberOfPixelSplitHits) >= 0 ) ? summary->get(Trk::numberOfPixelSplitHits) : 0 ;
    auto numberOfPixelSplitHits_m = Monitored::Scalar<int>( "m_numberOfPixelSplitHits", numberOfPixelSplitHits);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfPixelSplitHits_m);
    
    auto sctHits_m  = Monitored::Scalar<int>( "m_sctHits", sctHits ); 
    fill(trackGroup, eta_perigee_m, phi_perigee_m, sctHits_m);
    fill(trackGroup, lb_m, sctHits_m);
    
    auto numberOfSCTDeadSensors_m = Monitored::Scalar<int>( "m_numberOfSCTDeadSensors", numberOfSCTDeadSensors );
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfSCTDeadSensors_m);
    
    int numberOfSCTSharedHits = ( summary->get(Trk::numberOfSCTSharedHits) >= 0 ) ? summary->get(Trk::numberOfSCTSharedHits) : 0 ;
    auto numberOfSCTSharedHits_m = Monitored::Scalar<int>( "m_numberOfSCTSharedHits", numberOfSCTSharedHits);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfSCTSharedHits_m);
    
    int numberOfSCTHoles   = summary->get(Trk::numberOfSCTHoles) >= 0 ? summary->get(Trk::numberOfSCTHoles) : 0;
    auto numberOfSCTHoles_m   = Monitored::Scalar<int>( "m_numberOfSCTHoles", numberOfSCTHoles);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfSCTHoles_m);
    
    auto trtHits_m  = Monitored::Scalar<int>( "m_trtHits", trtHits ); 
    fill(trackGroup, eta_perigee_m, phi_perigee_m, trtHits_m);
    fill(trackGroup, lb_m, trtHits_m);
    
    int numberOfTRTDeadStraws =    ( summary->get(Trk::numberOfTRTDeadStraws) >= 0 ) ? summary->get(Trk::numberOfTRTDeadStraws) : 0 ;
    auto numberOfTRTDeadStraws_m = Monitored::Scalar<int>( "m_numberOfTRTDeadStraws", numberOfTRTDeadStraws);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfTRTDeadStraws_m);
    
    // Fill hits ENDS 
    // =================================== //
    
    // =================================== //
    // FillEtaPhi BEGINS
    
    int InnermostPixelLayerHit = 0;
    int NextToInnermostPixelLayerHit = 0;
    if ( m_doIBL )
      {
	// no IBL hit but a hit is expected
	if ( summary->get( Trk::expectInnermostPixelLayerHit ) && !summary->get( Trk::numberOfInnermostPixelLayerHits ) ) InnermostPixelLayerHit = 1;
	auto InnermostPixelLayerHit_m = Monitored::Scalar<int>( "m_InnermostPixelLayerHit", InnermostPixelLayerHit);
	fill(trackGroup, eta_perigee_m, phi_perigee_m, InnermostPixelLayerHit_m);
	
	
	// no b-layer hit but a hit is expected
	if ( summary->get( Trk::expectNextToInnermostPixelLayerHit ) && !summary->get( Trk::numberOfNextToInnermostPixelLayerHits ) ) NextToInnermostPixelLayerHit = 1 ;
	auto NextToInnermostPixelLayerHit_m = Monitored::Scalar<int>( "m_NextToInnermostPixelLayerHit", NextToInnermostPixelLayerHit);
	fill(trackGroup, eta_perigee_m, phi_perigee_m, NextToInnermostPixelLayerHit_m);
	
      }
    else
      {
	if ( summary->get( Trk::expectInnermostPixelLayerHit ) && !summary->get( Trk::numberOfInnermostPixelLayerHits ) ) InnermostPixelLayerHit = 1;
	NextToInnermostPixelLayerHit = InnermostPixelLayerHit;
	auto NextToInnermostPixelLayerHit_m = Monitored::Scalar<int>( "m_NextToInnermostPixelLayerHit", NextToInnermostPixelLayerHit);
	fill(trackGroup, eta_perigee_m, phi_perigee_m, NextToInnermostPixelLayerHit_m);
	
      }
    
    // No TRT extension
    int noTRTHits = 0;
    if ( summary->get(Trk::numberOfTRTHits) == 0 ) noTRTHits = 1;
    auto noTRTHits_m = Monitored::Scalar<int>( "m_noTRTHits", noTRTHits);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, noTRTHits_m);
    
    
    // Tight track selection
    int track_pass_tight = 0;
    if ( m_tight_trackSelTool -> accept(*track) ) track_pass_tight = 1; // tight selection
    auto track_pass_tight_m = Monitored::Scalar<int>( "m_track_pass_tight", track_pass_tight);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, track_pass_tight_m);
    
    // =================================== //
    // FillEtaPhi ENDS    
    
    int NoIBL = 0;
    if ( m_doIBL )
      {
	// no IBL hit but a hit is expected
	if ( summary->get( Trk::expectInnermostPixelLayerHit ) && !summary->get( Trk::numberOfInnermostPixelLayerHits ) ) NoIBL = 1;
	if (NoIBL == 1) nNoIBL++;
	auto NoIBL_m = Monitored::Scalar<int>( "m_NoIBL_LB", NoIBL);
	fill(trackGroup, lb_m, NoIBL_m);
      }
    
    int NoBL = 0;
    if ( summary->get( ( m_doIBL ) ? Trk::expectNextToInnermostPixelLayerHit : Trk::expectInnermostPixelLayerHit ) && !summary->get( ( m_doIBL ) ? Trk::numberOfNextToInnermostPixelLayerHits : Trk::numberOfInnermostPixelLayerHits ) ) NoBL = 1;
    if (NoBL == 1) nNoBL++;
    auto NoBL_m = Monitored::Scalar<int>( "m_NoBL_LB", NoBL);
    fill(trackGroup, lb_m, NoBL_m);
    
    int NoTRText = 0;
    if ( summary->get(Trk::numberOfTRTHits) + summary->get(Trk::numberOfTRTOutliers) == 0 ) NoTRText = 1;
    if (NoTRText == 1) nNoTRText++;
    auto NoTRText_m = Monitored::Scalar<int>( "m_NoTRText_LB", NoTRText);
    fill(trackGroup, lb_m, NoTRText_m);
    
    
    if ( m_tight_trackSelTool -> accept(*track) )
      {
	nTight++;
      } 
    
    
    // FillHitMaps is false for now
    // FillHoles is false for now
    
    
  } // end of track loop
  
  // Filling per-event histograms
  auto nBase_m   = Monitored::Scalar<int>( "m_nBase", nBase);
  fill(trackGroup, nBase_m);
  
  auto nBaseLB_m   = Monitored::Scalar<int>( "m_nBase_LB", nBase);
  fill(trackGroup, lb_m, nBaseLB_m);
  
  auto nTight_m   = Monitored::Scalar<int>( "m_nTight_LB", nTight);
  fill(trackGroup, lb_m, nTight_m);
  
  if ( m_doIBL ) {
    auto nNoIBL_m   = Monitored::Scalar<int>( "m_nNoIBL_LB", nNoIBL);
    fill(trackGroup, lb_m, nNoIBL_m);
  }
  
  auto nNoBL_m   = Monitored::Scalar<int>( "m_nNoBL_LB", nNoBL);
  fill(trackGroup, lb_m, nNoBL_m);
  
  auto nNoTRText_m   = Monitored::Scalar<int>( "m_nNoTRText_LB", nNoTRText);
  fill(trackGroup, lb_m, nNoTRText_m);


  
  
  //*******************************************************************************
  //**************************** End of filling Track Histograms ******************
  //*******************************************************************************
  
  return StatusCode::SUCCESS;
}
