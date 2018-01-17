/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef TRIGT2HISTOPRMVTX_TRIGT2HISTOPRMVTXBASE_H
#define TRIGT2HISTOPRMVTX_TRIGT2HISTOPRMVTXBASE_H


#include "GaudiKernel/MsgStream.h"

class HistoVertexHelper;
class TrigInDetTrack;
class TrigInDetTrackCollection;
class TrigT2HistoPrmVtx;
class TrigT2HistoPrmVtxAllTE;

namespace Rec { 
  class TrackParticleContainer;
  class TrackParticle; 
}


class TrigT2HistoPrmVtxBase {

 public:

  friend class TrigT2HistoPrmVtx;
  friend class TrigT2HistoPrmVtxAllTE;

  TrigT2HistoPrmVtxBase(MsgStream&, unsigned int);
  TrigT2HistoPrmVtxBase(const TrigT2HistoPrmVtxBase&);
  ~TrigT2HistoPrmVtxBase();

  TrigT2HistoPrmVtxBase& operator=(const TrigT2HistoPrmVtxBase&);

  /** @brief To check if phi RoI is in [-Pi;Pi] range. */
  float phiCorr(float);
  /** @brief To get d0 and z0 refered to the BeamSpot */
  void IPCorr(float, float, float &, float &, float, float, float);

  /**
   * @brief To perform track selection at LVL2 in order to evaluate the likelihood weight.
   * @return boolean variable set to true if the track has been selected
   *
   * This method is called for all the tracks in a certain collection and perform track selection
   * with the following default criteria (different selection criteria can be set through declareProperty):
   *
   * - chi square > 50 
   * - number of space points in the silicon detectors >= 4
   * - transverse impact parameter >= 1mm
   * - longitudinal impact parameter >= 2mm
   *
   */
  bool l2TrackSel(const TrigInDetTrack*&, unsigned int, float, float);
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
  bool efTrackSel(const xAOD::TrackParticle*&, unsigned int, float, float);

  /** @brief To calculate, through a sliding window approach, the three positions of the window in the histogram that maximize the entries of longitudinal impact parameter. */
  void findPrmVtx();

 private:

  /** @brief To retrieve selected tracks in percentage. */ 
  float totSelectedTracks() const {
    if (!m_totTracks) return -0.1;
    else return (float)m_totSelTracks/(float)m_totTracks;
  };

  /** @brief To use properly message service. */
  MsgStream m_log;
  /** @brief To use properly message service. */
  unsigned int m_logLvl; 

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

  /** @brief Pointer to HistoVertexHelper class. */
  HistoVertexHelper* m_hisVtx;

  /** @brief DeclareProperty: track reconstruction algorithm at LVL2. */
  int m_algo;
  /** @brief DeclareProperty: string corresponding to the trigger level in which the algorithm is running. */
  std::string m_instance;
  /** @brief DeclareProperty: bin number for histogramming method. */
  int m_nBins;

  /** @brief DeclareProperty: string corresponding to reference vertex used */
  bool m_useBeamSpot;
  /** @brief DeclareProperty: switch to perform track-RoI eta/phi matching when selecting tracks. */
  bool m_useEtaPhiTrackSel;

  /** @brief DeclareProperty: lower bound of the chi square of the reconstructed track at LVL2 (to perform track selection). */
  float m_l2TrkSelChi2;
  /** @brief DeclareProperty: lower bound of the number of hits on b-layer of reconstructed track at LVL2 (to perform track selection). */
  int m_l2TrkSelBLayer;
  /** @brief DeclareProperty: lower bound of the number of hits in silicon detectors of reconstructed track at LVL2 (to perform track selection). */
  int m_l2TrkSelSiHits;
  /** @brief DeclareProperty: upper bound of transverse impact parameter of reconstructed track at LVL2 (to perform track selection). */
  float m_l2TrkSelD0;
  /** @brief DeclareProperty: lower bound of pT of the reconstructed track at L2 (to perform track selection). */
  float m_l2TrkSelPt;
  /** @brief DeclareProperty: lower bound of the chi square of the reconstructed track at EF (to perform track selection). */
  float m_efTrkSelChi2;
  /** @brief DeclareProperty: lower bound of the number of hits on b-layer of reconstructed track at EF (to perform track selection). */
  int m_efTrkSelBLayer;
  /** @brief DeclareProperty: lower bound of the number of hits in pixel detector of reconstructed track at EF (to perform track selection). */
  int m_efTrkSelPixHits;
  /** @brief DeclareProperty: lower bound of the number of hits in silicon detectors of reconstructed track at EF (to perform track selection). */
  int m_efTrkSelSiHits;
  /** @brief DeclareProperty: upper bound of transverse impact parameter of reconstructed track at EF (to perform track selection). */
  float m_efTrkSelD0;
  /** @brief DeclareProperty: lower bound of pT of the reconstructed track at EF (to perform track selection). */
  float m_efTrkSelPt;

  /** @brief Beam spot position and precision. */
  float m_xBeamSpot,m_yBeamSpot,m_zBeamSpot;
  float m_xBeamSpotTilt,m_yBeamSpotTilt;
  float m_xBeamSpotSigma,m_yBeamSpotSigma,m_zBeamSpotSigma;

  /** @brief DeclareProperty: to monitor track selection for likelihood methods. */
  std::vector<float> m_listCutApplied;

};
 
#endif 
