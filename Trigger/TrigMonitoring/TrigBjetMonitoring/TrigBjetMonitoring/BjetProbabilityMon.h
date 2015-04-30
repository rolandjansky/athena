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

  /** @brief Pointer to HLTBjetMonTool class. */
  HLTBjetMonTool* m_bjetMon;

  /** @brief To use properly message service. */
  MsgStream* m_log;

  /** @brief Pointer to TaggerHelper class. */ 
  TaggerHelper* m_taggerHelper;


};

#endif
