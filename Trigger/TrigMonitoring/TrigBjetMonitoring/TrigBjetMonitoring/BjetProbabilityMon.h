/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************
//
// NAME:     BjetProbability.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
// 
// AUTHOR:   Ariel Schwartzman / Andrea Coccaro
// EMAIL:    sch@slac.stanford.edu / Andrea.Coccaro@ge.infn.it
//
// ************************************************************

#ifndef TRIGBJETHYPO_BJETPROBABILITYMON
#define TRIGBJETHYPO_BJETPROBABILITYMON

#include <string>
#include <iostream>
#include <cmath>

#include "TrigBjetMonitoring/HLTBjetMonTool.h"


class MsgStream;
class HLTBjetMonTool;
class TaggerHelper;
class TuningLikelihood;

class TrigInDetTrackCollection;
class TrigInDetTrack;
class TrigVertexCollection;
class VxContainer;

namespace Rec {
  class TrackParticleContainer;
  class TrackParticle;
}

class ITrigTrackJetFinderTool;



class BjetProbabilityMon {
  
 public:

  BjetProbabilityMon(HLTBjetMonTool*, MsgStream*);
  ~BjetProbabilityMon();

  /** @brief To retrieve private member m_etaRoI and m_phiRoI. */
  void setEtaPhi(float etaRoI, float phiRoI)
    {m_etaRoI = etaRoI; m_phiRoI = phiRoI;};

  /** @brief To set chi2 probability to default value (when beam spot doesn't allow b-jet triggers to fire). */
  void getProbabilityTag();
  /** @brief To compute chi2 probability weight at EF. */  
  void getProbabilityTag(const Rec::TrackParticleContainer*&, const TrigVertexCollection*&, const ToolHandle<ITrigTrackJetFinderTool>);
  void getProbabilityTag(const Rec::TrackParticleContainer*&, float , const ToolHandle<ITrigTrackJetFinderTool>);
  /** @brief To retrieve selected tracks in percentage. */ 
  float totSelectedTracks() const {
    if (!m_totTracks) return -0.1;
    else return (float)m_totSelTracks/(float)m_totTracks;
  };

  /** @brief To retrieve particular element of private member m_taggersXMap. */ 
  float taggersXMap(std::string tagger) {return m_taggersXMap[tagger];};

  /** @brief To retrieve CHI2 tagger distribution (for monitoring). */
  float getXCHI2() const {return m_taggersXMap.find("CHI2")->second;};

  /** @brief To properly use a selected EF track. */
  void addTrack(const Rec::TrackParticle*&);
  /** @brief To set the phi position of the track-jet. */
  void setJetPhi(float jetPhi);
  
  /** @brief To retrieve the probability of a certain track to originate form primary vertex. */
  float getTrackProbability(const TrigInDetTrack*&);
  /** @brief To retrieve the probability of a certain track to originate form primary vertex. */
  float getTrackProbability(const Rec::TrackParticle*&);

  void getPositiveJetProb(float& prob, int& n);
  void getNegativeJetProb(float& prob, int& n);

 private:

  /**
   * @brief To perform track selection at EF in order to evaluate the likelihood weight.
   * @return boolean variable set to true if the track has been selected
   *
   * This tagger is called for all the tracks in a certain collection and perform track selection
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
  bool efTrackSel(const Rec::TrackParticle*&, unsigned int, float);

  /** @brief Resolution function (divided per class and algorithm) */
  float f_ip(float x, int algo, int iclass);

  float d0Error(float&, int);
  float pScat(const TrigInDetTrack*&);
  float pScat(const Rec::TrackParticle*&);

  /** @brief Pointer to HLTBjetMonTool class. */
  HLTBjetMonTool* m_bjetMon;

  /** @brief To use properly message service. */
  MsgStream* m_log;

  /** @brief Pointer to TaggerHelper class. */ 
  TaggerHelper* m_taggerHelper;

  /** @brief Eta and phi of current RoI. */ 
  float m_etaRoI, m_phiRoI;

  /** @brief Association between the name of the tagger and its X variable (used to perform the selection in TrigBjetHypo). */
  std::map<std::string, float> m_taggersXMap;

  /** @brief Number of reconstructed tracks per RoI. */
  unsigned int m_totTracks;
  /** @brief Number of selected tracks per RoI. */
  unsigned int m_totSelTracks; 

  /** @brief Number of primary vertices per RoI. */
  unsigned int m_totVertices;
  /** @brief Reconstructed primary vertex per RoI. */
  float m_zPrmVtx;

  /** @brief x beam spot. */
  float m_xBeamSpot;
  /** @brief y beam spot. */
  float m_yBeamSpot;
  
  float m_maxIP;
  float m_jetPhi;

  float m_PI_pos;
  float m_PI_neg;

  int m_n_pos;
  int m_n_neg;

};

#endif
