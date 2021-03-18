/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigT2HistoPrmVtxBase.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2HistoPrmVtx
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#ifndef TRIGT2HISTOPRMVTX_TRIGT2HISTOPRMVTXBASE_MT_H
#define TRIGT2HISTOPRMVTX_TRIGT2HISTOPRMVTXBASE_MT_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

class HistoVertexHelper;
class TrigInDetTrack;
class TrigInDetTrackCollection;

namespace Rec { 
  class TrackParticleContainer;
  class TrackParticle; 
}


class TrigT2HistoPrmVtxBaseMT : public AthAlgorithm {

 public:
  TrigT2HistoPrmVtxBaseMT( const std::string&, ISvcLocator* );

  virtual StatusCode initialize() override;

 protected:
  /**
   * @brief To perform track selection at EF in order to evaluate the likelihood weight.
   * @return boolean variable set to true if the track has been selected
   *
   * This method is called for all the tracks in a certain collection and perform track selection
   * with the following default criteria (different selection criteria can be set through declareProperty):
   *
   * - chi square > 50
   * - number of hits in the b-layer >= 1
   * - number of hits in the pixel detector >= 2 
   * - number of space points in the silicon detectors >= 7
   * - transverse impact parameter >= 1mm
   * - longitudinal impact parameter >= 2mm
   *
   */
  bool trackSel(const xAOD::TrackParticle*, unsigned int, float, float);

  /** @brief To calculate, through a sliding window approach, the three positions of the window in the histogram that maximize the entries of longitudinal impact parameter. */
  void findPrmVtx( std::unique_ptr< HistoVertexHelper >& );

 protected:

  /** @brief To retrieve selected tracks in percentage. */ 
  float totSelectedTracks() const {
    if (!m_totTracks) return -0.1;
    else return (float)m_totSelTracks/(float)m_totTracks;
  };

  /** @brief Vector of primary vertex candidates (the first candidate is assumed to be the best). */
  std::vector<float> m_zPrmVtx;
  /** @brief Vector of primary vertex candidate sigmas (including 1-track vertices). */
  std::vector<float> m_zPrmVtxSigmaAll;
  /** @brief Vector of primary vertex candidate sigmas (excluding 1-track vertices). */
  std::vector<float> m_zPrmVtxSigma2Trk;
  /** @brief Vector of selected tracks per primary vertex candidated. */
  std::vector<int> m_nTrackVtx;
  /** @brief Number of primary vertex candidates found by the algorithm. */
  int m_nVtxFound;

  /** @brief Number of reconstructed tracks per RoI. */
  unsigned int m_totTracks;
  /** @brief Number of reconstructed tracks for all  RoI. */
  unsigned int m_totTracks_All;
  /** @brief Number of selected tracks per RoI. */
  unsigned int m_totSelTracks;
  /** @brief Number of selected tracks for all the RoIs. */
  unsigned int m_totSelTracks_All;



  /** @brief DeclareProperty: track reconstruction algorithm at LVL2. */
  Gaudi::Property< int > m_algo {this,"AlgoId",0,"track reconstruction algorithm at LVL2"};
  /** @brief DeclareProperty: bin number for histogramming method. */
  Gaudi::Property< int > m_nBins {this,"NumBins",0,"bin number for histogramming method."};

  /** @brief DeclareProperty: string corresponding to reference vertex used */
  Gaudi::Property< bool > m_useBeamSpot {this,"UseBeamSpot",false,"string corresponding to reference vertex used"};
  /** @brief DeclareProperty: switch to perform track-RoI eta/phi matching when selecting tracks. */
  Gaudi::Property< bool > m_useEtaPhiTrackSel {this,"UseEtaPhiTrackSel",false,"switch to perform track-RoI eta/phi matching when selecting tracks"};

  /** @brief DeclareProperty: lower bound of the chi square of the reconstructed track at HLT (to perform track selection). */
  Gaudi::Property< float > m_hltTrkSelChi2 {this,"HLTTrkSel_Chi2",0.0,"lower bound of the chi square of the reconstructed track at HLT (to perform track selection)"};
  /** @brief DeclareProperty: lower bound of the number of hits on b-layer of reconstructed track at HLT (to perform track selection). */
  Gaudi::Property< int > m_hltTrkSelBLayer {this,"HLTTrkSel_BLayer",1,"lower bound of the number of hits on b-layer of reconstructed track at HLT (to perform track selection)"};
  /** @brief DeclareProperty: lower bound of the number of hits in pixel detector of reconstructed track at HLT (to perform track selection). */
  Gaudi::Property< int > m_hltTrkSelPixHits {this,"HLTTrkSel_PixHits",2,"lower bound of the number of hits in pixel detector of reconstructed track at HLT (to perform track selection)"};
  /** @brief DeclareProperty: lower bound of the number of hits in silicon detectors of reconstructed track at HLT (to perform track selection). */
  Gaudi::Property< int > m_hltTrkSelSiHits {this,"HLTTrkSel_SiHits",7,"lower bound of the number of hits in silicon detectors of reconstructed track at HLT (to perform track selection)"};
  /** @brief DeclareProperty: upper bound of transverse impact parameter of reconstructed track at HLT (to perform track selection). */
  Gaudi::Property< float > m_hltTrkSelD0 {this,"HLTTrkSel_D0",1*Gaudi::Units::mm,"upper bound of transverse impact parameter of reconstructed track at HLT (to perform track selection)"};
  /** @brief DeclareProperty: lower bound of pT of the reconstructed track at HLT (to perform track selection). */
  Gaudi::Property< float > m_hltTrkSelPt {this,"HLTTrkSel_Pt",1*Gaudi::Units::GeV,"lower bound of pT of the reconstructed track at HLT (to perform track selection)"};

  /** @brief DeclareProperty: to monitor track selection for likelihood methods. */
  //  Gaudi::Property< std::vector<float> > m_listCutApplied {this,"ListCutApplied",{},"to monitor track selection for likelihood methods"};
  std::vector<float> m_listCutApplied;
};
 
#endif 
