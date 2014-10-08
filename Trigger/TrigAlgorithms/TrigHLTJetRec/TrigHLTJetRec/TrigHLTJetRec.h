/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetRec.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: FexAlgo for jet reconstruction
//
// ********************************************************************

#ifndef TRIGHLTJETREC_TRIGHLTJETREC_H
#define TRIGHLTJETREC_TRIGHLTJETREC_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "xAODJet/JetContainer.h"

class IJetBuildTool;
// class JetRecTool;
class ITriggerPseudoJetGetter;

class TrigHLTJetRec: public HLT::FexAlgo {

 public:
  TrigHLTJetRec(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetRec();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltFinalize();
 
private:

  // IJetBuildTool - offline code to transform pseudojets to xAOD jets
  ToolHandle<IJetBuildTool> m_jetbuildTool;
  // ToolHandle<JetRecTool> m_jetbuildTool;

  /* A PseudojetGetter shared by this algorithm and the IJetBuildTool
     The algorithm loads the psg, the ijbt processes them. */
  ToolHandle<ITriggerPseudoJetGetter>  m_pseudoJetGetter;

  /* label saying which cluster calibration was configured.
     Used to label tools.*/
  std::string m_clusterCalib;

  HLT::ErrorCode attachJetCollection(HLT::TriggerElement*,
                                     const xAOD::JetContainer*);


}; 
#endif
