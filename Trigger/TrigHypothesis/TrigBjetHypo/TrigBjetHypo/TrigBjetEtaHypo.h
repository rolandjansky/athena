/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetEtaHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetEtaHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETETAHYPO_H
#define TRIGBJETETAHYPO_H

#include "TrigInterfaces/HypoAlgo.h"


class TriggerElement;


class TrigBjetEtaHypo : public HLT::HypoAlgo {

 public:

  /** @brief Constructor. */
  TrigBjetEtaHypo (const std::string&, ISvcLocator*);
  /** @brief Destructor. */
  ~TrigBjetEtaHypo ();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, bool&);
  HLT::ErrorCode hltFinalize(); 

 private:

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  bool m_acceptAll;
  /** @brief DeclareProperty: Eta threshold cut. */
  float m_etaMax;
  /** @brief DeclareProperty: Eta of ROI. */
  float m_eta;
  /** @brief DeclareProperty: string corresponding to the trigger level in which the algorithm is running. */
  std::string m_instance;
  /** @brief DeclareProperty: string corresponding to the version of the algorithm (StartSequence, Vertexing, Btagging). */
  std::string m_version;

};

#endif

