/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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


InDetGlobalTrackMonAlg::InDetGlobalTrackMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator) {}


InDetGlobalTrackMonAlg::~InDetGlobalTrackMonAlg() {}


StatusCode InDetGlobalTrackMonAlg::initialize() {
  ATH_CHECK( m_trackToVertexIPEstimator.retrieve() );
  ATH_CHECK( m_IBLParameterSvc.retrieve() ); 
  
  m_doIBL = m_IBLParameterSvc->containsIBL();
  
  if (!m_trackSelTool.empty() )      ATH_CHECK( m_trackSelTool.retrieve() );
  if (!m_tight_trackSelTool.empty()) ATH_CHECK( m_tight_trackSelTool.retrieve() );
  if (!m_loose_trackSelTool.empty()) ATH_CHECK( m_loose_trackSelTool.retrieve() );

  ATH_CHECK( m_trackParticleName.initialize() );
  ATH_CHECK( m_vxContainerName.initialize() );
  ATH_CHECK( m_jetContainerName.initialize() );
  
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
  auto trackParticles = SG::makeHandle(m_trackParticleName, ctx);
  
  // check for tracks
  if ( !(trackParticles.isValid()) ) {
    ATH_MSG_ERROR("InDetGlobalMonitoringRun3Test: Track container "<< m_trackParticleName.key() << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("InDetGlobalMonitoringRun3Test: Track container "<< trackParticles.name() <<" is found.");
  }
  
  // counters
  int nBase = 0;
  int nTight = 0;
  int nNoIBL = 0;
  int nNoBL = 0;
  int nNoTRText = 0;

  uint8_t iSummaryValue(0); // Dummy counter to retrieve summary values
  
  for (const auto& trackPart: *trackParticles) {
    const Trk::Track * track = trackPart->track();
    if ( !track )
      {
	ATH_MSG_DEBUG( "InDetGlobalMonitoringRun3Test: NULL track pointer in collection" );
	continue;
      }

    const Trk::Perigee *perigee = track->perigeeParameters();
    if ( !perigee )
      {
	ATH_MSG_DEBUG( "InDetGlobalMonitoringRun3Test: NULL track->perigeeParameters pointer " );
	continue;
      }
    
    nBase++;
    
    // =================================== //
    // Fill hits BEGINS
    
    float eta_perigee = perigee->eta();
    float phi_perigee = perigee->parameters()[Trk::phi0];
    
    auto eta_perigee_m   = Monitored::Scalar<float>( "m_eta_perigee", eta_perigee);
    auto phi_perigee_m   = Monitored::Scalar<float>( "m_phi_perigee", phi_perigee);

    auto eta_perigee_loose_m   = Monitored::Scalar<float>( "m_eta_perigee_loose", eta_perigee);
    auto phi_perigee_loose_m   = Monitored::Scalar<float>( "m_phi_perigee_loose", phi_perigee);

    // Loose tracks
    if ( m_loose_trackSelTool->accept(*track) ){
      fill(trackGroup, eta_perigee_loose_m, phi_perigee_loose_m);
    }

    // Base tracks
    if ( !m_trackSelTool->accept(*track) )
      continue;

    fill(trackGroup, eta_perigee_m, phi_perigee_m); // Trk_Base_eta_phi
    
    if ( m_doIBL )
      {
	int numberOfInnermostPixelLayerHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfInnermostPixelLayerHits) ? iSummaryValue : 0;
	auto numberOfInnermostPixelLayerHits_m = Monitored::Scalar<int>( "m_numberOfInnermostPixelLayerHits", numberOfInnermostPixelLayerHits);
	fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfInnermostPixelLayerHits_m);
	
	fill(trackGroup, lb_m, numberOfInnermostPixelLayerHits_m);
      }

    int numberOfPixelHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHits) ? iSummaryValue : 0;
    int numberOfPixelDeadSensors = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelDeadSensors) ? iSummaryValue : 0;
    int pixHits = numberOfPixelHits + numberOfPixelDeadSensors;
    auto pixHits_m  = Monitored::Scalar<int>( "m_pixHits", pixHits ); 
    fill(trackGroup, eta_perigee_m, phi_perigee_m, pixHits_m);
    fill(trackGroup, lb_m, pixHits_m);
    
    auto numberOfPixelDeadSensors_m = Monitored::Scalar<int>( "m_numberOfPixelDeadSensors", numberOfPixelDeadSensors );
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfPixelDeadSensors_m);

    int numberOfPixelSharedHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelSharedHits) ? iSummaryValue : 0;
    auto numberOfPixelSharedHits_m = Monitored::Scalar<int>( "m_numberOfPixelSharedHits", numberOfPixelSharedHits);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfPixelSharedHits_m);

    int numberOfPixelHoles = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHoles) ? iSummaryValue : 0;
    auto numberOfPixelHoles_m = Monitored::Scalar<int>( "m_numberOfPixelHoles", numberOfPixelHoles); 
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfPixelHoles_m);

    int numberOfPixelSplitHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelSplitHits) ? iSummaryValue : 0;
    auto numberOfPixelSplitHits_m = Monitored::Scalar<int>( "m_numberOfPixelSplitHits", numberOfPixelSplitHits);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfPixelSplitHits_m);

    int numberOfSCTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTHits) ? iSummaryValue : 0;
    int numberOfSCTDeadSensors = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTDeadSensors) ? iSummaryValue : 0;
    int sctHits = numberOfSCTHits + numberOfSCTDeadSensors;
    auto sctHits_m  = Monitored::Scalar<int>( "m_sctHits", sctHits ); 
    fill(trackGroup, eta_perigee_m, phi_perigee_m, sctHits_m);
    fill(trackGroup, lb_m, sctHits_m);
    
    auto numberOfSCTDeadSensors_m = Monitored::Scalar<int>( "m_numberOfSCTDeadSensors", numberOfSCTDeadSensors );
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfSCTDeadSensors_m);

    int numberOfSCTSharedHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTSharedHits) ? iSummaryValue : 0;
    auto numberOfSCTSharedHits_m = Monitored::Scalar<int>( "m_numberOfSCTSharedHits", numberOfSCTSharedHits);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfSCTSharedHits_m);

    int numberOfSCTHoles   = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTHoles) ? iSummaryValue : 0;
    auto numberOfSCTHoles_m   = Monitored::Scalar<int>( "m_numberOfSCTHoles", numberOfSCTHoles);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfSCTHoles_m);

    int numberOfTRTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTHits) ? iSummaryValue : 0;
    int numberOfTRTDeadStraws = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTDeadStraws) ? iSummaryValue : 0;
    int trtHits = numberOfTRTHits + numberOfTRTDeadStraws;
    auto trtHits_m  = Monitored::Scalar<int>( "m_trtHits", trtHits ); 
    fill(trackGroup, eta_perigee_m, phi_perigee_m, trtHits_m);
    fill(trackGroup, lb_m, trtHits_m);

    auto numberOfTRTDeadStraws_m = Monitored::Scalar<int>( "m_numberOfTRTDeadStraws", numberOfTRTDeadStraws);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, numberOfTRTDeadStraws_m);
    
    // Fill hits ENDS 
    // =================================== //
    
    // =================================== //
    // FillEtaPhi BEGINS
    
    int InnermostPixelLayerHit = 0;
    int NextToInnermostPixelLayerHit = 0;

    int expInHit = trackPart->summaryValue(iSummaryValue, xAOD::expectInnermostPixelLayerHit) ? iSummaryValue : 0;
    int nInHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfInnermostPixelLayerHits) ? iSummaryValue : 0;

    xAOD::SummaryType expNInHitField = ( m_doIBL ) ? xAOD::expectNextToInnermostPixelLayerHit : xAOD::expectInnermostPixelLayerHit;
    xAOD::SummaryType nNInHitField = ( m_doIBL ) ? xAOD::numberOfNextToInnermostPixelLayerHits : xAOD::numberOfInnermostPixelLayerHits;
    int expNInHit = trackPart->summaryValue(iSummaryValue, expNInHitField) ? iSummaryValue : 0;
    int nNInHits = trackPart->summaryValue(iSummaryValue, nNInHitField) ? iSummaryValue : 0;

    if ( m_doIBL )
      {
	// no IBL hit but a hit is expected
	if ( expInHit==1 && nInHits==0 ) InnermostPixelLayerHit = 1;
	auto InnermostPixelLayerHit_m = Monitored::Scalar<int>( "m_InnermostPixelLayerHit", InnermostPixelLayerHit);
	fill(trackGroup, eta_perigee_m, phi_perigee_m, InnermostPixelLayerHit_m);
      }

    // no b-layer hit but a hit is expected
    if ( expNInHit==1 && nNInHits==0 ) NextToInnermostPixelLayerHit = 1 ;
    auto NextToInnermostPixelLayerHit_m = Monitored::Scalar<int>( "m_NextToInnermostPixelLayerHit", NextToInnermostPixelLayerHit);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, NextToInnermostPixelLayerHit_m);
    
    // No TRT extension
    int noTRTHits = 0;
    if ( numberOfTRTHits == 0 ) noTRTHits = 1;
    auto noTRTHits_m = Monitored::Scalar<int>( "m_noTRTHits", noTRTHits);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, noTRTHits_m);
    
    
    // Tight track selection
    int track_pass_tight = 0;
    if ( m_tight_trackSelTool -> accept(*track) ){
      track_pass_tight = 1; // tight selection
      nTight++;
    }
    auto track_pass_tight_m = Monitored::Scalar<int>( "m_track_pass_tight", track_pass_tight);
    fill(trackGroup, eta_perigee_m, phi_perigee_m, track_pass_tight_m);
    
    // =================================== //
    // FillEtaPhi ENDS    
    
    int NoIBL = 0;
    if ( m_doIBL )
      {
	// no IBL hit but a hit is expected
	if ( expInHit==1 && nInHits==0 ) NoIBL = 1;
	if (NoIBL == 1) nNoIBL++;
	auto NoIBL_m = Monitored::Scalar<int>( "m_NoIBL_LB", NoIBL);
	fill(trackGroup, lb_m, NoIBL_m);
      }
    
    int NoBL = 0;
    if ( expNInHit==1 && nNInHits==0 ) NoBL = 1;
    if (NoBL == 1) nNoBL++;
    auto NoBL_m = Monitored::Scalar<int>( "m_NoBL_LB", NoBL);
    fill(trackGroup, lb_m, NoBL_m);
    
    int NoTRText = 0;
    int numberOfTRTOutliers = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTOutliers) ? iSummaryValue : 0;
    if ( numberOfTRTHits + numberOfTRTOutliers == 0 ) NoTRText = 1;
    if (NoTRText == 1) nNoTRText++;
    auto NoTRText_m = Monitored::Scalar<int>( "m_NoTRText_LB", NoTRText);
    fill(trackGroup, lb_m, NoTRText_m);
    
    // FillHitMaps is false for now
    // FillHoles is false for now
    
    
  } // end of track loop
  
  // =================================== //
  // FillTide BEGINS
  if ( m_doTide )
    {
      // retrieving vertices
      auto handle_vxContainer = SG::makeHandle(m_vxContainerName, ctx);

      if (!handle_vxContainer.isPresent()) {
	ATH_MSG_DEBUG ("InDetGlobalTrackMonAlg: StoreGate doesn't contain primary vertex container with key "+m_vxContainerName.key()+",may not be able to produce TIDE histograms");
      }
      if (!handle_vxContainer.isValid()) {
	ATH_MSG_DEBUG ("InDetGlobalTrackMonAlg: Could not retrieve primary vertex container with key "+m_vxContainerName.key()+",may not be able to produce TIDE histograms");
      }

      auto vertexContainer = handle_vxContainer.cptr();

      // retrieving jets

      auto handle_jetContainer = SG::makeHandle(m_jetContainerName, ctx);

      if (!handle_jetContainer.isPresent()) {
	ATH_MSG_DEBUG ("InDetGlobalTrackMonAlg: StoreGate doesn't contain jet container with key "+m_jetContainerName.key()+",may not be able to produce TIDE histograms");
      }
      if (!handle_jetContainer.isValid()) {
	ATH_MSG_DEBUG ("InDetGlobalTrackMonAlg: Could not retrieve jet container with key "+m_jetContainerName.key()+",may not be able to produce TIDE histograms");
      }

      auto jetContainer = handle_jetContainer.cptr();

      if ( handle_jetContainer.isValid() ) {
	for ( auto jetItr = jetContainer->begin(); jetItr != jetContainer->end(); ++jetItr )
	  {
	    if ( (*jetItr)->pt() < 20000. ){
	      continue;
	    } 
	    std::vector<const xAOD::IParticle*> trackVector;
	    if ( !(*jetItr)->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack, trackVector) ){
	      continue;
	    } 

	    for ( std::vector<const xAOD::IParticle*>::const_iterator trkItr = trackVector.begin(); trkItr != trackVector.end() ; ++trkItr )
	      {
		const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*trkItr);
		if ( !trackPart ){
		  continue;
		} 
		uint8_t split;
		uint8_t shared;
		uint8_t pix;
		if ( trackPart->summaryValue(pix, xAOD::numberOfPixelHits) && pix )
		  {
		    const Trk::Perigee perigeeTIDE = trackPart->perigeeParameters();
		    const xAOD::Vertex* foundVertex { nullptr };
		    if ( handle_vxContainer.isValid() ){
		      for ( const auto *const vx : *vertexContainer )
			{
			  for ( const auto& tpLink : vx->trackParticleLinks() )
			    {
			      if ( *tpLink == trackPart )
				{
				  foundVertex = vx;
				  break;
				} 
			    } 
			  if (foundVertex) break;
			} 
		    } 
		    if ( foundVertex )
		      {
			std::unique_ptr<const Trk::ImpactParametersAndSigma>myIPandSigma(m_trackToVertexIPEstimator->estimate(trackPart,foundVertex));
			
			if ( myIPandSigma )
			  {
			    float jetassocdR = trackPart->p4().DeltaR( (*jetItr)->p4());
			    float jetassocd0Reso = std::abs( myIPandSigma->IPd0 / std::sqrt( myIPandSigma->sigmad0*myIPandSigma->sigmad0 + myIPandSigma->PVsigmad0*myIPandSigma->PVsigmad0 ) );
			    float jetassocz0Reso = std::abs( myIPandSigma->IPz0 / std::sqrt( myIPandSigma->sigmaz0*myIPandSigma->sigmaz0 + myIPandSigma->PVsigmaz0*myIPandSigma->PVsigmaz0 ) );
			    float jetassocIPReso = std::abs( myIPandSigma->IPd0 / std::sqrt( myIPandSigma->sigmad0*myIPandSigma->sigmad0 + myIPandSigma->PVsigmad0*myIPandSigma->PVsigmad0 ) );
			    auto jetassocdR_m = Monitored::Scalar<float>("m_jetassocdR", jetassocdR);
			    auto jetassocd0Reso_m = Monitored::Scalar<float>("m_jetassocd0Reso", jetassocd0Reso);
			    auto jetassocz0Reso_m = Monitored::Scalar<float>("m_jetassocz0Reso", jetassocz0Reso);
			    auto jetassocIPReso_m = Monitored::Scalar<float>("m_jetassocd0Reso", jetassocIPReso);

			    fill(trackGroup, jetassocdR_m, jetassocd0Reso_m);
			    fill(trackGroup, jetassocdR_m, jetassocz0Reso_m);
			    fill(trackGroup, lb_m, jetassocIPReso_m);    
			  } 
			
		      } 
		    if ( trackPart->summaryValue( split, xAOD::numberOfPixelSplitHits) ){
		      float frac = (double)split / pix;
		      float pixSplitdR = trackPart->p4().DeltaR( (*jetItr)->p4() );
		      auto pixSplitFrac_m = Monitored::Scalar<float>("m_pixSplitFrac", frac);
		      auto pixSplitdR_m =   Monitored::Scalar<float>("m_pixSplitdR", pixSplitdR);

		      fill(trackGroup, pixSplitdR_m, pixSplitFrac_m);
		      fill(trackGroup, lb_m, pixSplitFrac_m);
		    } 

		    if ( trackPart->summaryValue( shared, xAOD::numberOfPixelSharedHits) ){
                      float frac = (float)shared / pix;
                      float pixShareddR = trackPart->p4().DeltaR( (*jetItr)->p4() );
                      auto pixSharedFrac_m = Monitored::Scalar<float>("m_pixSharedFrac", frac);
                      auto pixShareddR_m =   Monitored::Scalar<float>("m_pixShareddR", pixShareddR);

                      fill(trackGroup, pixShareddR_m, pixSharedFrac_m);
                      fill(trackGroup, lb_m, pixSharedFrac_m);
		    } 
		  } 
	      } 
	  } 
      } 
    } 

  // FillTide ENDS
  // =================================== //

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
