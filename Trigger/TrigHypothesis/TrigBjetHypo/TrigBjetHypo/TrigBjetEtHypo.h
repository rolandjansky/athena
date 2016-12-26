/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetEtHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetEtHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETETHYPO_H
#define TRIGBJETETHYPO_H

#include "TrigInterfaces/HypoAlgo.h"


class TriggerElement;


class TrigBjetEtHypo : public HLT::HypoAlgo {

 public:

  /** @brief Constructor. */
  TrigBjetEtHypo (const std::string&, ISvcLocator*);
  /** @brief Destructor. */
  ~TrigBjetEtHypo ();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, bool&);
  HLT::ErrorCode hltFinalize(); 

 private:

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  bool m_acceptAll;
  /** @brief DeclareProperty: Et threshold cut. */
  float m_etThreshold;
  /** @brief DeclareProperty: Et threshold. */
  //float m_et;
  /** @brief DeclareProperty: string corresponding to the trigger level in which the algorithm is running. */
  std::string m_instance;
  /** @brief DeclareProperty: string corresponding to the version of the algorithm (StartSequence, Vertexing, Btagging). */
  std::string m_version;
  /** @brief DeclareProperty: string corresponding to the name of the input jet collection */
  std::string m_jetKey;

};

#endif

