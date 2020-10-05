/*
  Copyright (C) 2002-2019CERN for the benefit of the ATLAS collaboration
*/

#include "TMath.h"
#include "TrigInDetTrackMonitoringTool.h"
#include<iostream>

//#include "TrigInterfaces/Algo.h"
//
//Constructor-------------------------------------------------------------- 
TrigInDetTrackMonitoringTool::TrigInDetTrackMonitoringTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent):
  AthAlgTool(type,name,parent)
{
  ////declareInterface< ITrigInDetTrackMonitoringTool >(this);
  //declareProperty( "MonitoringTool",      m_monTool  );
}

//Destructor---------------------------------------------------------------
TrigInDetTrackMonitoringTool::~TrigInDetTrackMonitoringTool(){
  ATH_MSG_DEBUG( "TrigInDetTrackMonitoringTool destructor called");
}

//Initialize---------------------------------------------------------------
StatusCode TrigInDetTrackMonitoringTool::initialize(){
  ATH_MSG_DEBUG( "TrigInDetTrackMonitoringTool initialize() called");

  //Retrieve monitoring tool
  ATH_CHECK( m_monTool.retrieve());
   
  return StatusCode::SUCCESS;

}

StatusCode TrigInDetTrackMonitoringTool::finalize() {
  ATH_MSG_DEBUG("TrigInDetTrackMonitoringTool finalize()");
  return StatusCode::SUCCESS; 
}


//Monitoring track variables
//void TrigInDetTrackMonitoringTool::monitor_tracks(std::string prefix, std::string suffix, DataVector<xAOD::TrackParticle> tracks ){
//void TrigInDetTrackMonitoringTool::monitor_tracks( const std::string &prefix, const std::string &suffix, const xAOD::TrackParticleContainer &tracks ){
void TrigInDetTrackMonitoringTool::monitor_tracks( const std::string &prefix, const std::string &suffix, const xAOD::TrackParticleContainer &tracks ){
   auto  trackPt      = Monitored::Collection(  prefix + "Pt"       + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->pt()      ;});
   auto  trackQOverP  = Monitored::Collection(  prefix + "QOverP"       + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->qOverP()      ;});
   auto  trackEta     = Monitored::Collection(  prefix + "Eta"      + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->eta()     ;});
   auto  trackPhi     = Monitored::Collection(  prefix + "Phi"      + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->phi()     ;});
   auto  trackTheta     = Monitored::Collection(  prefix + "Theta"      + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->theta()     ;});
   auto  trackZ0      = Monitored::Collection(  prefix + "Z0"       + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->z0()      ;});
   auto  trackD0      = Monitored::Collection(  prefix + "D0"       + suffix, tracks, []( const xAOD::TrackParticle *t){ return t->d0()      ;});
//   auto  trackZ0err   = Monitored::Collection(  prefix + "Z0err"    + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->Z0err()   ;});
//   auto  trackD0err   = Monitored::Collection(  prefix + "D0err"    + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->D0err()   ;});
   auto  trackNDF     = Monitored::Collection(  prefix + "NDF"      + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->numberDoF()     ;});
//   auto  trackQual    = Monitored::Collection(  prefix + "Qual"     + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->Qual()    ;});
   auto  trackChi2Prob= Monitored::Collection(  prefix + "Chi2Prob" + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->chiSquared();});
   auto  trackPiHits  = Monitored::Collection(  prefix + "PIXHits"  + suffix, tracks, [](const xAOD::TrackParticle *t){  uint8_t nPixHits = 0; t->summaryValue( nPixHits,  xAOD::numberOfPixelHits); return nPixHits ;});
   auto  trackSCTHits  = Monitored::Collection(  prefix + "SCTHits"  + suffix, tracks, [](const xAOD::TrackParticle *t){ uint8_t nSCTHits = 0; t->summaryValue( nSCTHits,  xAOD::numberOfSCTHits); return nSCTHits ;});
   //auto  trackPiHits  = Monitored::Collection(  prefix + "PIXHits"  + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->numberOfPixelHits() ;});
   //auto  trackSCTHits = Monitored::Collection(  prefix + "SCTHits"  + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->numberOfSCTHits() ;});
//   auto  trackTRTHits = Monitored::Collection(  prefix + "TRTHits"  + suffix, tracks, [](const xAOD::TrackParticle *t){ return t->TRTHits() ;});
//   auto mon = Monitored::Group(m_monTool, trackPt, trackEta, trackPhi, trackZ0, trackD0, trackZ0err, trackD0err,
//         trackNDF, trackQual, trackChi2Prob, trackSiHits, trackPiHits, trackSCTHits, trackTRTHits )
//
//
//TODO need to record nHits  (from track summary?)
 //   uint8_t numberOfPixelHits = 0;
 //   particle->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits);
 //   m_dqm_npix_hits.push_back(static_cast<int>(numberOfPixelHits));
 //   uint8_t numberOfSCTHits = 0;
 //   particle->summaryValue(numberOfSCTHits, xAOD::numberOfSCTHits);
 //   m_dqm_nsct_hits.push_back(static_cast<int>(numberOfSCTHits));
 //   uint8_t numberOfTRTHits = 0;
 //   particle->summaryValue(numberOfTRTHits, xAOD::numberOfTRTHits);
 //   if (fabs(particle->eta())<1.9){
 //     m_dqm_ntrt_hits.push_back(static_cast<int>(numberOfTRTHits));
 //   } else {
 //     m_dqm_ntrt_hits.push_back(-1);
 //   }

 //   uint8_t expectInnermostHit = 0;
 //   particle->summaryValue(expectInnermostHit, xAOD::expectInnermostPixelLayerHit  );
 //   uint8_t numberOfInnermostHits = 0;
 //   particle->summaryValue(numberOfInnermostHits, xAOD::numberOfInnermostPixelLayerHits  );
 //   if (numberOfInnermostHits>1) numberOfInnermostHits = 1;
 //   if (expectInnermostHit>0){
 //     m_dqm_ibl_hit_expected_found.push_back(float(numberOfInnermostHits));
 //   }
 //   else {
 //     m_dqm_ibl_hit_expected_found.push_back(-1.);
 //   }

 //   if(particle->numberDoF()>0) {
 //     m_dqm_chi2dof.push_back(particle->chiSquared() / particle->numberDoF());
 //   }



   auto mon = Monitored::Group(m_monTool, trackPt, trackEta, trackQOverP, trackPhi, trackTheta, trackD0 ,trackZ0, trackNDF, trackChi2Prob );
}

