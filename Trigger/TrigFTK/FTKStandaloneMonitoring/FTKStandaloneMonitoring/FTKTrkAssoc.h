#ifndef __FTKTRKASSOC_H__
#define __FTKTRKASSOC_H__
#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigInDetAnalysisUtils/BestMatcher.h"
#include "TMath.h"
typedef BestMatcher<const FTK_RawTrack> TrkMatch;
class FTKTrkAssoc : public TrkMatch { 
    public:
      FTKTrkAssoc(const std::string& name, double d) : 
        TrkMatch(name, d)
    { } 
      virtual ~FTKTrkAssoc() { } 
      double distance( const FTK_RawTrack* t0,  const FTK_RawTrack* t1 ) const {
        double deta = TMath::ASinH(t0->getCotTh())-TMath::ASinH(t1->getCotTh());
        double dphi = t0->getPhi()-t1->getPhi(); 
        if ( dphi> M_PI ) dphi-=2*M_PI; 
        if ( dphi<-M_PI ) dphi+=2*M_PI;
        return std::sqrt( deta*deta + dphi*dphi );
      }
  };
  /*
   * @brief Track comparison class
   */
//   class HltTrackComparison
//   {
//     public:
//       /*
//        * @brief constructor
//        */
//       HltTrackComparison();
//       /*
//        * @brief destructor
//        */
//       ~HltTrackComparison();
//       /*
//        * @brief Perform efficiency analyis. This is the only function that needs to be called from the outside.
//        *        Track filtering of reference tracks is performed automatically.
//        * @param ref_track_vec input vector of reference tracks
//        * @param test_track_vec input vector of test tracks
//        */
//       void TrackAnalyisis(const std::vector<const FTK_RawTrack*>& ref_track_vec, const std::vector<const FTK_RawTrack*>& test_track_vec, bool purityMode=false );
//       /*
//        * @brief Filters tracks via a simple cut selection
//        * @param track_in_vec input track vector
//        * @param track_out_vec output vector of selected tracks
//        */
//       void FilterTracks(const std::vector<const FTK_RawTrack*>& track_in_vec, std::vector<const FTK_RawTrack*>& track_out_vec);
//       /*
//        * @brief Determines whether a track is good or not by applying cuts and returns true or false accordingly
//        * @param track input track
//        */
//       bool IsGoodTrack(const FTK_RawTrack* track);
//       /*
//        * @brief Clears all variable storage vectors
//        */
//       void Clear(void);
//       // Variables to be monitored. I don't work with histos as it's not recommended for trigger.
//       // Histograms need to be managed by the trigger algorithm using HltTrackComparison
//       // see: also https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerValidationHistograms
//       std::vector<bool>   m_eff_weight_vec; // efficiency weight: 1 if matching test track found, else 0
//       std::vector<bool>   m_fake_weight_vec; // fake track weight: 0 if matching test track found, else 1
//       MonitoredTrackParticle monitored_ref_track;
//       MonitoredTrackParticle monitored_ref_unmatched_track;
//       MonitoredTrackParticle monitored_test_matched_track;
//       MonitoredTrackParticlePair monitored_track_pair;
//       int m_numb_ref_trks;
//       int m_numb_filtered_trks;
//       int m_numb_test_trks;
//       int m_numb_match_trks;
//       // 2d to 1d projection variables and vectors
//       // don't forget to adjust histograms accordingly
//       int m_2d_n_eta;
//       double m_2d_eta_min;
//       double m_2d_eta_max;
//       double m_2d_z0_min;
//       double m_2d_z0_max;
//       // track matching cut
//       double m_deltaR_max;
//       // track selection cuts
//       double        m_min_pt;
//       double        m_max_eta;
//       double        m_max_z0;
//       double        m_max_d0;
//       double        m_max_z0err;
//       double        m_max_d0err;
//       double        m_max_reducedChi2;
//       uint8_t       m_min_siHits;
//       uint8_t       m_min_pixHits;
//       uint8_t       m_min_sctHits;
//       uint8_t       m_max_pixHoles;
//       uint8_t       m_max_sctHoles;
//     private:
//       FTKTrkAssoc* m_associator;
//   }; // end
#endif
