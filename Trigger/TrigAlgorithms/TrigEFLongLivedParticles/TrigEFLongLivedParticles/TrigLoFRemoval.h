/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigLoFRemoval.h
 * PACKAGE:  Trigger/TrigAlgorithms/TrigEFLongLivedParticles
 *
 * AUTHOR:   A. Policicchio
 *********************************************************************/
#ifndef TRIGLOFREMOVAL_TRIGLOFREMOVAL_H
#define TRIGLOFREMOVAL_TRIGLOFREMOVAL_H

#include "TrigInterfaces/FexAlgo.h"
#include "JetEvent/JetCollection.h"

class JetAlgToolBase;
class TrigTimer;

class TrigLoFRemoval : public HLT::FexAlgo {

 public:

  TrigLoFRemoval(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigLoFRemoval();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);

 private:



  /////////////////////////////////////////////////////////////////////////////
  /// Properties
  /////////////////////////////////////////////////////////////////////////////


  // Monitoring
  double m_et;
  double m_njet;
  double m_eta;
  double m_phi;
  float m_CellContainerSize;


public:

 
};
#endif
