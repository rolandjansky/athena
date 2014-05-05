/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ************************************************
//
// NAME:     BjetLikelihoodMon.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETHYPO_BJETLIKELIHOODMON
#define TRIGBJETHYPO_BJETLIKELIHOODMON

#include <map>
#include <string>
#include <vector>
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



class BjetLikelihoodMon {

 public:

  BjetLikelihoodMon(HLTBjetMonTool*, MsgStream*);
  ~BjetLikelihoodMon();

  /** @brief To retrieve private member m_etaRoI and m_phiRoI. */
  void setEtaPhi(float etaRoI, float phiRoI)
    {m_etaRoI = etaRoI; m_phiRoI = phiRoI;};

  /** @brief To retrieve private member m_totTracks. */
  unsigned int totTracks()     {return m_totTracks;};  
  /** @brief To retrieve private member m_totSelTracks. */ 
  unsigned int totSelTracks()  {return m_totSelTracks;};
  /** @brief To retrieve private member m_totVertices. */
  unsigned int totVertices()   {return m_totVertices;};
  /** @brief To retrieve private member m_xPrmVtx. */ 
  float xPrmVtx()              {return m_xPrmVtx;};
  /** @brief To retrieve private member m_yPrmVtx. */ 
  float yPrmVtx()              {return m_yPrmVtx;};
  /** @brief To retrieve private member m_zPrmVtx. */ 
  float zPrmVtx()              {return m_zPrmVtx;};

  /** @brief To retrieve selected tracks in percentage (for monitoring). */ 
  float totSelectedTracks() const {
    if (!m_totTracks) return -0.1;
    else return (float)m_totSelTracks/(float)m_totTracks;
  };

  /** @brief To retrieve particular element of private member m_taggersXMap. */ 
  float taggersXMap(std::string tagger) {return m_taggersXMap[tagger];};

  /** @brief To retrieve IP1D tagger distribution (for monitoring). */
  float getXIP1D() const {return m_taggersXMap.find("IP1D")->second;};
  /** @brief To retrieve IP2D tagger distribution (for monitoring). */
  float getXIP2D() const {return m_taggersXMap.find("IP2D")->second;};
  /** @brief To retrieve IP3D tagger distribution (for monitoring). */
  float getXIP3D() const {return m_taggersXMap.find("IP3D")->second;};
  /** @brief To retrieve SVTX tagger distribution (for monitoring). */
  float getXSVTX() const {return m_taggersXMap.find("SVTX")->second;};
  /** @brief To retrieve COMB tagger distribution (for monitoring). */
  float getXCOMB() const {return m_taggersXMap.find("COMB")->second;};

  /** @brief To fill maps of likelihood taggers using calibrations stored in python files. */
  void fillLikelihoodMap(const std::string tagger, TuningLikelihood* pointerToTuningLikelihood) 
    {m_likelihoodMap[tagger] = pointerToTuningLikelihood;};

  /** @brief To set likelihood weights to default values (when beam spot doesn't allow b-jet triggers to fire). */
  void getLikelihoodTag();
  /** @brief To compute likelihood weights at LVL2. */  
  void getLikelihoodTag(const TrigInDetTrackCollection*&, float ,const ToolHandle<ITrigTrackJetFinderTool>);

  /** @brief To compute likelihood weights based on impact parameters of reconstructed tracks at EF. */  
  void getLikelihoodTag(const Rec::TrackParticleContainer*&, float, const ToolHandle<ITrigTrackJetFinderTool>);
  /** @brief To compute best likelihood weight (combination of IP3D and SV). */  
  void getBestLikelihoodTag();

 private:

  /**
   * @brief To perform track selection at LVL2 in order to evaluate the likelihood weight.
   * @return boolean variable set to true if the track has been selected
   *
   * This tagger is called for all the tracks in a certain collection and perform track selection
   * with the following default criteria (different selection criteria can be set through declareProperty):
   *
   * - chi square > 50 
   * - number of space points in the silicon detectors >= 4
   * - transverse impact parameter >= 1mm
   * - longitudinal impact parameter >= 2mm
   *
   */
  //TrigInDetTrackCollection
  
  bool l2TrackSel(const TrigInDetTrack*&, unsigned int, float);
  // bool l2TrackSel(const TrigInDetTrackCollection*&, unsigned int, float);

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

  /** @brief To parametrize errors on track longitudinal impact parameter at LVL2. */
  float getIP1DErr(float);
  /** @brief To parametrize errors on track transverse impact parameter at LVL2. */
  float getIP2DErr(float);

  /** @brief To get the likelihood ratio of a given track. */
  float getW(const std::string, float);
  /** @brief To get the likelihood ratio of a given track for the 2D combination of significances likelihood taggers. */
  float getW(const std::string, float, float);
  /** @brief To get the likelihood ratio of the three-dimensioanl tagger based on all secondary vertex taggers. */
  float getW(const std::string, float, float, float);

  /** @brief To scale the final likelihood weight in the range [0,1] to be more manageable. */  
  float getX(float);
  /** @brief To scale the likelihood weight in order to be compatible with histogram tuning files. */  
  float scaleVar(const float&);

  /** @brief Pointer to HLTBjetMonTool class. */
  HLTBjetMonTool* m_bjetMon;

  /** @brief To use properly message service. */
  MsgStream* m_log;

  /** @brief Pointer to TaggerHelper class. */ 
  TaggerHelper* m_taggerHelper;

  /** @brief Eta and phi of current RoI. */ 
  float m_etaRoI, m_phiRoI;

  /** @brief Association between the name of the tagger and its W variable. */
  std::map<std::string, float> m_taggersWMap;
  /** @brief Association between the name of the tagger and its X variable (used to perform the selection in TrigBjetHypo). */
  std::map<std::string, float> m_taggersXMap;
  /** @brief Association between the likelihood tagger name and the relative TuningLikelihood object. */
  std::map<std::string, TuningLikelihood*> m_likelihoodMap;

  /** @brief Significance of the longitudinal impact parameter. */
  float m_IP1D;
  /** @brief Significance of the transverse impact parameter. */
  float m_IP2D;
 /** @brief Significance of the transverse impact parameter. */
  float m_IP3D;
  /** @brief Number of reconstructed tracks per RoI. */
  unsigned int m_totTracks;
  /** @brief Number of selected tracks per RoI. */
  unsigned int m_totSelTracks; 

  /** @brief Number of primary vertices per RoI. */
  unsigned int m_totVertices;
  /** @brief Reconstructed x primary vertex. */
  float m_xPrmVtx;
  /** @brief Reconstructed y primary vertex. */
  float m_yPrmVtx;
  /** @brief Reconstructed z primary vertex. */
  float m_zPrmVtx;

  /** @brief x beam spot. */
  float m_xBeamSpot;
  /** @brief y beam spot. */
  float m_yBeamSpot;

};
#endif
