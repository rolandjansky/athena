/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_HYPOBASE_H
#define DECISIONHANDLING_HYPOBASE_H 1

#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"

class HypoBase : public ::AthReentrantAlgorithm {
  /**
   * @class HypoBase
   * @brief Hypothesis algorithms take the output of reco algorithms and the decision from the preceeding InputMaker, to produce a new decision.
This is a base class for HLT Hypos to reduce boilerplate and enforce the common naming scheme for decision handle keys. Derived classes will have to add specific reco data read & write keys to suit their purpose.
   **/
 public:
  /// constructor, to be called by sub-class constructors
  HypoBase( const std::string& name, ISvcLocator* pSvcLocator );
  /// destructor
  virtual ~HypoBase(); 
  /// initialise this base class and renounce input decision key handles
  virtual StatusCode sysInitialize() override;
  /// execute to be implemented in derived clas
  virtual StatusCode execute_r(const EventContext&) const override = 0;
  virtual StatusCode finalize() override = 0;
  virtual StatusCode initialize() override = 0;  
 protected:
  /// methods for derived classes to access handles of the base class input and output decisions; other read/write handles may be implemented by derived classes
  const SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer>& decisionInput() const;
  const SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer>& decisionOutput() const;
 private:
  
  /// input decisions, will be implicit (renounced).
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_input { this, "HypoInputDecisions", "HypoInputDecision", "Input Decision (implicit)" };
  /// output decisions
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_output { this, "HypoOutputDecisions", "HypoOutputDecision", "Ouput Decision" };
  
  // for future implementation: ToolHandleArray<ITestHypoTool> m_tools { this, "HypoTools", {}, "Hypo tools" };
};


#endif // DECISIONHANDLING_HYPOBASE_H
