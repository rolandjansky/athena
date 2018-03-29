/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETHYPO_H
#define TRIGBJETHYPO_H

#include "TrigInterfaces/HypoAlgo.h"


class TrigEFBjetContainer;
class TrigTimerSvc;
class TriggerElement;

/**
 * @brief Hypo class for HLT b-jet selection.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is the base hypothesis class for the b-jet selection. 
 * Selection can be performed using different likelihood methods or a track-based chi2 method (CHI2), all implemented in the TrigBjetFex class.
 *
 * The different likelihood methods implemented are:
 * - significance of longitudinal impact parameter (IP1D);
 * - significance of transverese impact parameter (IP2D);
 * - 2D combination of the track-based methods (IP3D);
 * - invariant mass of tracks linked to the secondary vertex (MVTX);
 * - energy fraction of the secondary vertex (EVTX);
 * - number of tracks lined to the secondary vertex (NVTX);
 * - 3D combination of the vertex-based methods (SVTX);
 * - combination of the two likelihoods based on tracks and secondary vertex informations (COMB).
 *
 * Selection can be performed using these different taggers: CHI2, IP2D, IP3D or COMB.
 */


class TrigBjetHypo : public HLT::HypoAlgo {

 public:

  /** @brief Constructor. */
  TrigBjetHypo (const std::string&, ISvcLocator*);
  /** @brief Destructor. */
  ~TrigBjetHypo ();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, bool&);
  HLT::ErrorCode hltFinalize(); 

 private:

  /** @brief Total execution time of TrigBjetFex class. */
  //TrigTimer *m_totTimer;

  std::string m_jetKey;

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  bool m_acceptAll;
  /** @brief DeclareProperty: list of likelihood methods to be effectively used to perform the selection. */
  std::string m_methodTag; 
  /** @brief DeclareProperty: lower bound of the discriminant variable to be selected (if flag acceptAll is set to false) for IP2D tagger. */
  float m_xcutIP2D;
  /** @brief DeclareProperty: lower bound of the discriminant variable to be selected (if flag acceptAll is set to false) for IP3D tagger. */
  float m_xcutIP3D;
  /** @brief DeclareProperty: lower bound of the discriminant variable to be selected (if flag acceptAll is set to false) for COMB tagger. */
  float m_xcutCOMB;
  /** @brief DeclareProperty: lower bound of the discriminant variable to be selected (if flag acceptAll is set to false) for CHI2 tagger. */
  float m_xcutCHI2;
  /** @brief DeclareProperty: lower bound of the discriminant variable to be selected (if flag acceptAll is set to false) for MV2 tagger. */
  float m_xcutMV2c20;
  float m_xcutMV2c10;

  /** @brief DeclareProperty: string corresponding to the trigger level in which the algorithm is running. */
  std::string m_instance;

  /** @brief to check the beam spot flag status. */
  bool m_useBeamSpotFlag;

  /** @brief Overide the requirement that the BS is valid. */
  /** @brief Used to not apply the correction to the GSC chains */
  bool m_overRideBeamSpotValid;
  
  /** @brief DeclareProperty: to monitor method used to perform the cut. */
  float m_monitorMethod;

  /** @brief Cut counter. */
  int m_cutCounter;

};

#endif

