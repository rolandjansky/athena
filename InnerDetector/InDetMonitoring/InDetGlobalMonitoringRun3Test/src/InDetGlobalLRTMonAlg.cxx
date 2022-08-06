/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalLRTMonAlg.h
 * Implementation of inner detector global LRT  monitoring tool
 *
 *@author
 * Hamza Hanif  <hamza.hanif@cern.ch> 
 *
 * based on InDetGlobalTrackMonTool.cxx (Leonid Serkin  and  Per Johansson)
 *
 ****************************************************************************/

//main header
#include "InDetGlobalLRTMonAlg.h"

//Standard c++
#include <vector>
#include <memory>


InDetGlobalLRTMonAlg::InDetGlobalLRTMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_trackSelTool      ( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_doIBL(true)
{
  //jo flags go here, keys and some tools -> in class
  declareProperty("DoIBL", m_doIBL,"IBL present?");	
  declareProperty( "TrackSelectionTool", m_trackSelTool);
}


InDetGlobalLRTMonAlg::~InDetGlobalLRTMonAlg() {}


StatusCode InDetGlobalLRTMonAlg::initialize() {
  ATH_CHECK( m_IBLParameterSvc.retrieve() ); 
  
  m_doIBL = m_IBLParameterSvc->containsIBL();
  
  if (!m_trackSelTool.empty() )      ATH_CHECK( m_trackSelTool.retrieve() );

  ATH_CHECK( m_trackParticleName.initialize() );
  
  return AthMonitorAlgorithm::initialize();
}


StatusCode InDetGlobalLRTMonAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  
  //*******************************************************************************
  //************************** Begin of filling Track Histograms ******************
  //*******************************************************************************

  ATH_MSG_DEBUG("Filling InDetGlobalLRTMonAlg");
  
  // For histogram naming
  auto lrtGroup = getGroup("LRT");
  
  int lb       = GetEventInfo(ctx)->lumiBlock();
  auto lb_m    = Monitored::Scalar<int>( "m_lb", lb );
  
  
  float lumiPerBCID       = lbAverageInteractionsPerCrossing(ctx);
  auto lumiPerBCID_m    = Monitored::Scalar<float>( "m_lumiPerBCID", lumiPerBCID );


  // retrieving tracks
  auto LRT_trackParticles = SG::makeHandle(m_trackParticleName, ctx);
  
  // check for tracks
  if ( !(LRT_trackParticles.isValid()) ) {
    ATH_MSG_ERROR("InDetGlobalMonitoringRun3Test: Track container "<< m_trackParticleName.key() << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("InDetGlobalMonitoringRun3Test: Track container "<< LRT_trackParticles.name() <<" is found.");
  }
  
  // counters
  int nBase = 0;
  int nNoIBL = 0;
  int nNoBL = 0;
  int nNoTRText = 0;
  
  uint8_t iSummaryValue(0); // Dummy counter to retrieve summary values

  for (const auto& trackPart: *LRT_trackParticles) {
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

    // Loose primary tracks
    if ( !m_trackSelTool->accept(*track) )
      continue;
    
    nBase++;
    
    
    // =================================== //
    // Fill hits BEGINS

    float eta_perigee = perigee->eta();
    float phi_perigee = perigee->parameters()[Trk::phi0];
    
    auto eta_perigee_m   = Monitored::Scalar<float>( "m_eta_perigee", eta_perigee);
    auto phi_perigee_m   = Monitored::Scalar<float>( "m_phi_perigee", phi_perigee);
    fill(lrtGroup, eta_perigee_m, phi_perigee_m); // Trk_Base_eta_phi
    fill(lrtGroup, eta_perigee_m);
    fill(lrtGroup, phi_perigee_m);

    float d0_perigee = perigee->parameters()[Trk::d0];    
    auto d0_perigee_m   = Monitored::Scalar<float>( "m_d0_perigee", d0_perigee);
    fill(lrtGroup, d0_perigee_m);

    float z0_perigee = perigee->parameters()[Trk::z0];
    auto z0_perigee_m   = Monitored::Scalar<float>( "m_z0_perigee", z0_perigee);
    fill(lrtGroup, z0_perigee_m); 

    float theta = perigee->parameters()[Trk::theta];
    float qOverPt = perigee->parameters()[Trk::qOverP]/sin(theta);
    float charge = perigee->charge();
    float pT = 0;
    if ( qOverPt != 0 ) {
      pT = (1/qOverPt)*(charge);    
      auto pT_m = Monitored::Scalar<float>("m_trkPt", pT/1000); 
      fill(lrtGroup, pT_m);
    }

    if ( m_doIBL )
      {
	int numberOfInnermostPixelLayerHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfInnermostPixelLayerHits) ? iSummaryValue : 0;
	auto numberOfInnermostPixelLayerHits_m = Monitored::Scalar<int>( "m_numberOfInnermostPixelLayerHits", numberOfInnermostPixelLayerHits);
	fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfInnermostPixelLayerHits_m);
	
	fill(lrtGroup, lb_m, numberOfInnermostPixelLayerHits_m);
      }

    int numberOfPixelHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHits) ? iSummaryValue : 0;
    int numberOfPixelDeadSensors = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelDeadSensors) ? iSummaryValue : 0;
    int pixHits = numberOfPixelHits + numberOfPixelDeadSensors;
    auto pixHits_m  = Monitored::Scalar<int>( "m_pixHits", pixHits ); 
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, pixHits_m);
    fill(lrtGroup, lb_m, pixHits_m);
    
    auto numberOfPixelDeadSensors_m = Monitored::Scalar<int>( "m_numberOfPixelDeadSensors", numberOfPixelDeadSensors );
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfPixelDeadSensors_m);

    int numberOfPixelSharedHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelSharedHits) ? iSummaryValue : 0;
    auto numberOfPixelSharedHits_m = Monitored::Scalar<int>( "m_numberOfPixelSharedHits", numberOfPixelSharedHits);
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfPixelSharedHits_m);

    int numberOfPixelHoles = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHoles) ? iSummaryValue : 0;
    auto numberOfPixelHoles_m = Monitored::Scalar<int>( "m_numberOfPixelHoles", numberOfPixelHoles); 
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfPixelHoles_m);

    int numberOfPixelSplitHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelSplitHits) ? iSummaryValue : 0;
    auto numberOfPixelSplitHits_m = Monitored::Scalar<int>( "m_numberOfPixelSplitHits", numberOfPixelSplitHits);
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfPixelSplitHits_m);

    int numberOfSCTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTHits) ? iSummaryValue : 0;
    int numberOfSCTDeadSensors = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTDeadSensors) ? iSummaryValue : 0;
    int sctHits = numberOfSCTHits + numberOfSCTDeadSensors;
    auto sctHits_m  = Monitored::Scalar<int>( "m_sctHits", sctHits ); 
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, sctHits_m);
    fill(lrtGroup, lb_m, sctHits_m);
    
    auto numberOfSCTDeadSensors_m = Monitored::Scalar<int>( "m_numberOfSCTDeadSensors", numberOfSCTDeadSensors );
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfSCTDeadSensors_m);
    
    int numberOfSCTSharedHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTSharedHits) ? iSummaryValue : 0;
    auto numberOfSCTSharedHits_m = Monitored::Scalar<int>( "m_numberOfSCTSharedHits", numberOfSCTSharedHits);
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfSCTSharedHits_m);
    
    int numberOfSCTHoles   = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTHoles) ? iSummaryValue : 0;
    auto numberOfSCTHoles_m   = Monitored::Scalar<int>( "m_numberOfSCTHoles", numberOfSCTHoles);
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfSCTHoles_m);

    int numberOfTRTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTHits) ? iSummaryValue : 0;
    int numberOfTRTDeadStraws = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTDeadStraws) ? iSummaryValue : 0;
    int trtHits = numberOfTRTHits + numberOfTRTDeadStraws;
    auto trtHits_m  = Monitored::Scalar<int>( "m_trtHits", trtHits ); 
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, trtHits_m);
    fill(lrtGroup, lb_m, trtHits_m);

    auto numberOfTRTDeadStraws_m = Monitored::Scalar<int>( "m_numberOfTRTDeadStraws", numberOfTRTDeadStraws);
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, numberOfTRTDeadStraws_m);
    
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
	fill(lrtGroup, eta_perigee_m, phi_perigee_m, InnermostPixelLayerHit_m);
      }

    // no b-layer hit but a hit is expected
    if ( expNInHit==1 && nNInHits==0 ) NextToInnermostPixelLayerHit = 1 ;
    auto NextToInnermostPixelLayerHit_m = Monitored::Scalar<int>( "m_NextToInnermostPixelLayerHit", NextToInnermostPixelLayerHit);
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, NextToInnermostPixelLayerHit_m);
    
    // No TRT extension
    int noTRTHits = 0;
    if ( numberOfTRTHits == 0 ) noTRTHits = 1;
    auto noTRTHits_m = Monitored::Scalar<int>( "m_noTRTHits", noTRTHits);
    fill(lrtGroup, eta_perigee_m, phi_perigee_m, noTRTHits_m);
    
    // =================================== //
    // FillEtaPhi ENDS    
    
    int NoIBL = 0;
    if ( m_doIBL )
      {
	// no IBL hit but a hit is expected
	if ( expInHit==1 && nInHits==0 ) NoIBL = 1;
	if (NoIBL == 1) nNoIBL++;
	auto NoIBL_m = Monitored::Scalar<int>( "m_NoIBL_LB", NoIBL);
	fill(lrtGroup, lb_m, NoIBL_m);
      }
    
    int NoBL = 0;
    if ( expNInHit==1 && nNInHits==0 ) NoBL = 1;
    if (NoBL == 1) nNoBL++;
    auto NoBL_m = Monitored::Scalar<int>( "m_NoBL_LB", NoBL);
    fill(lrtGroup, lb_m, NoBL_m);
    
    int NoTRText = 0;
    int numberOfTRTOutliers = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTOutliers) ? iSummaryValue : 0;
    if ( numberOfTRTHits + numberOfTRTOutliers == 0 ) NoTRText = 1;
    if (NoTRText == 1) nNoTRText++;
    auto NoTRText_m = Monitored::Scalar<int>( "m_NoTRText_LB", NoTRText);
    fill(lrtGroup, lb_m, NoTRText_m);
    
    // FillHitMaps is false for now
    // FillHoles is false for now
    
    
  } // end of track loop
  
  // Filling per-event histograms
  auto nBase_m   = Monitored::Scalar<int>( "m_nBase", nBase);
  fill(lrtGroup, nBase_m);
  
  auto nBaseLB_m   = Monitored::Scalar<int>( "m_nBase_LB", nBase);
  fill(lrtGroup, lb_m, nBaseLB_m);

  fill(lrtGroup, lumiPerBCID_m, nBaseLB_m);

  if ( m_doIBL ) {
    auto nNoIBL_m   = Monitored::Scalar<int>( "m_nNoIBL_LB", nNoIBL);
    fill(lrtGroup, lb_m, nNoIBL_m);
  }
  
  auto nNoBL_m   = Monitored::Scalar<int>( "m_nNoBL_LB", nNoBL);
  fill(lrtGroup, lb_m, nNoBL_m);
  
  auto nNoTRText_m   = Monitored::Scalar<int>( "m_nNoTRText_LB", nNoTRText);
  fill(lrtGroup, lb_m, nNoTRText_m);
  
  //*******************************************************************************
  //**************************** End of filling Track Histograms ******************
  //*******************************************************************************
  
  return StatusCode::SUCCESS;
}
