/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_Monitoring/HltTrackComparison.h"

using namespace TrigFTK;

HltTrackComparison::HltTrackComparison() :
  m_numb_ref_trks(0)
  , m_numb_filtered_trks(0)
  , m_numb_test_trks(0)
  , m_numb_match_trks(0)
  , m_2d_n_eta(1)          
  , m_2d_eta_min(-3)
  , m_2d_eta_max(3)
  , m_2d_z0_min(-300)
  , m_2d_z0_max(300)
  , m_deltaR_max(0.01)
  , m_min_pt(0.0)
  , m_max_eta(10)
  , m_max_z0(500)
  , m_max_d0(10)
  , m_max_z0err(100)
  , m_max_d0err(100)
  , m_max_reducedChi2(100)
  , m_min_siHits(0)
  , m_min_pixHits(0)
  , m_min_sctHits(0)
  , m_max_pixHoles(100)
  , m_max_sctHoles(100)
  , m_associator(NULL)
{
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
HltTrackComparison::~HltTrackComparison()
{
  if(m_associator)
    delete m_associator;
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void HltTrackComparison::TrackAnalyisis(
    const std::vector<const xAOD::TrackParticle*>& ref_track_vec,
    const std::vector<const xAOD::TrackParticle*>& test_track_vec, bool purityMode )
{
  // initializing
  if(!m_associator) 
    m_associator = new DeltaRAssociator("DeltaRAssociator", m_deltaR_max);
  monitored_ref_track.initialize(m_2d_n_eta, m_2d_eta_min, m_2d_eta_max, m_2d_z0_min, m_2d_z0_max);
  monitored_ref_unmatched_track.initialize(m_2d_n_eta, m_2d_eta_min, m_2d_eta_max, m_2d_z0_min, m_2d_z0_max);
  monitored_test_matched_track.initialize(m_2d_n_eta, m_2d_eta_min, m_2d_eta_max, m_2d_z0_min, m_2d_z0_max);

  // filter ref tracks
  std::vector<const xAOD::TrackParticle*> filtered_track_vec;
  FilterTracks(ref_track_vec, filtered_track_vec);

  // track matching
  m_associator->match( filtered_track_vec, test_track_vec );

  m_numb_ref_trks       = ref_track_vec.size();
  m_numb_filtered_trks  = filtered_track_vec.size();
  m_numb_test_trks      = test_track_vec.size();
  m_numb_match_trks     = m_associator->size();

  // allocate memory
  monitored_ref_track.reserve(m_numb_filtered_trks);
  if(purityMode)
    monitored_ref_unmatched_track.reserve(m_numb_filtered_trks-m_numb_match_trks);
  else{
    monitored_test_matched_track.reserve(m_numb_match_trks);
    monitored_track_pair.reserve(m_numb_match_trks);
  }

  // loop over reference tracks
  double eff_weight(0.);
  double fake_weight(1);
  const xAOD::TrackParticle* match_track = NULL;
  for(auto& ref_track : filtered_track_vec){
    eff_weight  = 0.;
    fake_weight = 1.;
    match_track = m_associator->matched(ref_track);

    // found matching test track
    if ( match_track ){
      eff_weight  = 1.;
      fake_weight = 0.;
      if( !purityMode ){
        monitored_test_matched_track.push_back(match_track);
        monitored_track_pair.push_back(ref_track, match_track);
      }
    } 
    else if ( purityMode ){
      monitored_ref_unmatched_track.push_back(ref_track);
    }

    m_eff_weight_vec.push_back(eff_weight);
    m_fake_weight_vec.push_back(fake_weight);
    monitored_ref_track.push_back(ref_track);
  } // end loop over ref tracks
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void HltTrackComparison::Clear(void)
{
  m_eff_weight_vec.clear();
  m_fake_weight_vec.clear();

  monitored_ref_track.clear();
  monitored_ref_unmatched_track.clear();
  monitored_test_matched_track.clear();
  monitored_track_pair.clear();

  m_numb_ref_trks=0;
  m_numb_filtered_trks=0;
  m_numb_test_trks=0;
  m_numb_match_trks=0;
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void HltTrackComparison::FilterTracks(
    const std::vector<const xAOD::TrackParticle*>& track_in_vec, std::vector<const xAOD::TrackParticle*>& track_out_vec)
{
  track_out_vec.reserve(track_in_vec.size());
  for( auto& track: track_in_vec ){
    if( IsGoodTrack(track) ) track_out_vec.push_back(track); 
  }
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
bool HltTrackComparison::IsGoodTrack(const xAOD::TrackParticle* track)
{
  double pt       = std::abs(track->pt());
  double eta      = std::abs(track->eta());
  double z0       = std::abs(track->z0());
  double d0       = std::abs(track->d0());
  double deltaZ0  = std::sqrt(track->definingParametersCovMatrix()(Trk::z0,Trk::z0));
  double deltaD0  = std::sqrt(track->definingParametersCovMatrix()(Trk::d0,Trk::d0));

  uint8_t pixHits = 0;
  track->summaryValue( pixHits, xAOD::numberOfPixelHits);

  uint8_t sctHits = 0;
  track->summaryValue( sctHits, xAOD::numberOfSCTHits);

  uint8_t siHits = pixHits + sctHits;

  uint8_t sctHoles  = 0;
  track->summaryValue( sctHoles, xAOD::numberOfSCTHoles);

  uint8_t pixHoles  = 0;
  track->summaryValue( pixHoles, xAOD::numberOfPixelHoles);

  double reducedChi2 = track->chiSquared() / track->numberDoF();

/* for testing haven't found a good way to use athena message streams
   std::cout <<
    "pt          >= m_min_pt: "   << (pt >= m_min_pt) << std::endl <<
    "eta         <= m_max_eta: "  << (eta <= m_max_eta) << std::endl <<
    "z0          <= m_max_z0: "   << (z0 <= m_max_z0) << std::endl <<
    "d0          <= m_max_d0: "   << (d0 <= m_max_d0) << std::endl <<
    "deltaZ0     <= m_max_z0err: " << (deltaZ0 <= m_max_z0err) << std::endl <<
    "deltaD0     <= m_max_d0err: " << (deltaD0 <= m_max_d0err) << std::endl <<   
    "pixHits     >= m_min_pixHits: "  << (pixHits >= m_min_pixHits) << std::endl <<
    "sctHits     >= m_min_sctHits: "  << (sctHits >= m_min_sctHits) << std::endl <<
    "siHits      >= m_min_siHits: "   << (siHits >= m_min_siHits) << std::endl <<
    "pixHoles    <= m_max_pixHoles: " << (pixHoles <= m_max_pixHoles) <<  std::endl <<
    "sctHoles    <= m_max_sctHoles: " << (sctHoles <= m_max_sctHoles) << std::endl <<
    "reducedChi2 <= m_max_reducedChi2: " << (reducedChi2 <= m_max_reducedChi2) << std::endl;
    */

  return ( 
      pt          >= m_min_pt &&
      eta         <= m_max_eta &&
      z0          <= m_max_z0 &&
      d0          <= m_max_d0 &&
      deltaZ0     <= m_max_z0err &&
      deltaD0     <= m_max_d0err &&

      pixHits     >= m_min_pixHits &&
      sctHits     >= m_min_sctHits &&
      siHits      >= m_min_siHits &&  
      pixHoles    <= m_max_pixHoles &&
      sctHoles    <= m_max_sctHoles &&

      reducedChi2 <= m_max_reducedChi2
      );
}
