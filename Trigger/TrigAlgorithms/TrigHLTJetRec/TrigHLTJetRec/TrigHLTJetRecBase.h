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
#include "TrigHLTJetRec/ITriggerJetBuildTool.h"

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

  /*
  virtual HLT::ErrorCode getPseudoJets(const InputContainer*,
                                       jet::LabelIndex* indexMap,
                                       jet::PseudoJetVector& pjv);
  */
  
  std::string getClusterCalib() const {return m_clusterCalib;}
  std::string getSecondaryLabel() const {return m_secondarylabel;}
  bool secondaryLabelisEmpty() const { return m_secondarylabel == ""; }
  bool secondaryLabelisTracks() const { return m_secondarylabel.find("Track") != std::string::npos; }

  // functions and variables for secondary(associated) pseudojets
  /*
  virtual HLT::ErrorCode checkforSecondaryPseudoJets(
                                      const HLT::TriggerElement*,
                                      jet::LabelIndex*,
                                      jet::PseudoJetVector&);
  */

  // ToolHandle<ITriggerPseudoJetGetter>  m_secondarypseudoJetGetter; // a secondary pseudojet getter (for e.g. ghost association)
  //

   virtual HLT::ErrorCode build(fastjet::ClusterSequence*&, xAOD::JetContainer*&) const = 0;
  
   HLT::ErrorCode defaultBuild(fastjet::ClusterSequence*&,
                               xAOD::JetContainer*&) const;

  const PseudoJetContainer* getPseudoJetContainer() const;

 private:
  std::string m_clusterCalib;
  std::string  m_secondarylabel; // to label secondary pseudojets
  // IJetBuildTool - offline code to transform pseudojets to xAOD jets
  ToolHandle<ITriggerJetBuildTool> m_jetBuildTool;

 
  HLT::ErrorCode getInputContainer(const HLT::TriggerElement*,
                                   const InputContainer*&);

  template<typename ghostInputContainer>
  HLT::ErrorCode getGhostInputContainer(const HLT::TriggerElement*,
                                  const ghostInputContainer*&);

 // IJetBuildTool - offline code to transform pseudojets to xAOD jets
 // ToolHandle<IJetBuildTool> m_jetbuildTool;
 // ToolHandle<JetRecTool> m_jetbuildTool;
 
 /* A PseudojetGetter shared by this algorithm and the IJetBuildTool
    The algorithm loads the psg, the ijbt processes them. */
 // ToolHandle<ITriggerPseudoJetGetter>  m_pseudoJetGetter;
 
 /* A tool to select the pseudojets to be converted to jets */
 // ToolHandle<IPseudoJetSelector>  m_IPseudoJetSelector;
 
 /* label saying which cluster calibration was configured.
    Used to label tools.*/

 std::string m_outputCollectionLabel;
 // std::string m_pseudoJetLabelIndexArg;

 
  HLT::ErrorCode attachJetCollection(HLT::TriggerElement*,
                                     const xAOD::JetContainer*,
                                     const fastjet::ClusterSequence*);
  

};

#include "TrigHLTJetRec/TrigHLTJetRecBase_tpl.h"

#endif
