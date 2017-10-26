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

#ifndef TRIGHLTJETREC_TRIGHLTJETRECBASE_H
#define TRIGHLTJETREC_TRIGHLTJETRECBASE_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "xAODJet/JetContainer.h"
#include "JetEDM/PseudoJetVector.h"
#include "TrigHLTJetRec/TrigHLTJetRecFunctions.h"
#include "TrigHLTJetRec/IParticleVector.h"

class IJetBuildTool;
// class JetRecTool;
class ITriggerPseudoJetGetter;
class IPseudoJetSelector;

namespace jet {
  class LabelIndex;
}

template <typename InputContainer>
class TrigHLTJetRecBase: public HLT::FexAlgo {

 public:
  TrigHLTJetRecBase(const std::string & name, ISvcLocator* pSvcLocator);
  virtual ~TrigHLTJetRecBase();
  
  virtual HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltFinalize();
  
 protected:

  virtual HLT::ErrorCode getPseudoJets(const InputContainer*,
                                       jet::LabelIndex* indexMap,
                                       jet::PseudoJetVector& pjv);

  std::string getClusterCalib() const {return m_clusterCalib;}

  // functions and variables for secondary(associated) pseudojets
  virtual HLT::ErrorCode checkforSecondaryPseudoJets(
                                      const HLT::TriggerElement* inputTE,
                                      jet::LabelIndex* indexMap,
                                      jet::PseudoJetVector& pjv_secondary);

  ToolHandle<ITriggerPseudoJetGetter>  m_secondarypseudoJetGetter; // a secondary pseudojet getter (for e.g. ghost association)
  std::string  m_secondarylabel; // to label secondary pseudojets
  //

  virtual const xAOD::JetContainer* build() const = 0; 
  const xAOD::JetContainer* defaultBuild() const;

 private:
  std::string m_clusterCalib;

 HLT::ErrorCode getInputContainer(const HLT::TriggerElement*,
                                  const InputContainer*&);

 // IJetBuildTool - offline code to transform pseudojets to xAOD jets
 ToolHandle<IJetBuildTool> m_jetbuildTool;
 // ToolHandle<JetRecTool> m_jetbuildTool;
 
 /* A PseudojetGetter shared by this algorithm and the IJetBuildTool
    The algorithm loads the psg, the ijbt processes them. */
 ToolHandle<ITriggerPseudoJetGetter>  m_pseudoJetGetter;
 
 /* A tool to select the pseudojets to be converted to jets */
 ToolHandle<IPseudoJetSelector>  m_IPseudoJetSelector;
 
 /* label saying which cluster calibration was configured.
    Used to label tools.*/

 std::string m_outputCollectionLabel;
 std::string m_pseudoJetLabelIndexArg;

 
  HLT::ErrorCode attachJetCollection(HLT::TriggerElement*,
                                     const xAOD::JetContainer*);

};

#include "TrigHLTJetRec/TrigHLTJetRecBase_tpl.h"

#endif
