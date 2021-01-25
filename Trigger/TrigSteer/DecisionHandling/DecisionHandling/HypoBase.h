/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_HYPOBASE_H
#define DECISIONHANDLING_HYPOBASE_H 1

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"

class HypoBase : public ::AthReentrantAlgorithm {
  /**
   * @class HypoBase
   * @brief Hypothesis algorithms take the output of reco algorithms and the decision from the preceeding InputMaker,
   * to produce a new decision.
   * This is a base class for HLT Hypos to reduce boilerplate and enforce the common naming scheme for decision handle keys. 
   * Derived classes will have to add specific reco data read & write keys to suit their purpose.
   **/
 public:
  /// constructor, to be called by sub-class constructors
  HypoBase( const std::string& name, ISvcLocator* pSvcLocator );
  /// destructor
  virtual ~HypoBase(); 
  /// initialise this base class 
  virtual StatusCode sysInitialize() override;

 protected:

  /// methods for derived classes to access handles of the base class input 
  /// other read/write handles may be implemented by derived classes
  const SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer>& decisionInput() const;
  /// methods for derived classes to access handles of the base class output 
  /// other read/write handles may be implemented by derived classes  
  const SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer>& decisionOutput() const;
  /// Base class function to be called once slice specific code has finished. Handles debug printing and validation.
  StatusCode hypoBaseOutputProcessing(SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle, 
    MSG::Level lvl = MSG::DEBUG) const;

 private:

  /// @name Runtime validation methods
  /// @{  

  enum LogicalFlowCheckMode { 
    kRequireOne, //!< Require all DecisionIDs to be present in at least one of my parent Decision objects 
    kRequireAll //<! Require all DecisionIDs to be present in all of my parent Decision objects
  };

  /// Executes all individual runtime tests.
  StatusCode runtimeValidation(SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle) const;

  /// Ensure all HypoAlg Decisions have a feature. Supports ComboHypo too
  StatusCode validateHasFeature(const ElementLink<TrigCompositeUtils::DecisionContainer>& dEL) const;

  /// Ensure that all DecisionIDs have propagated correctly from their parent
  StatusCode validateLogicalFlow(const ElementLink<TrigCompositeUtils::DecisionContainer>& dEL, const LogicalFlowCheckMode mode) const;

  /// Ensure that no space is being wasted by duplicated DecisionIDs in any Decision objects
  StatusCode validateDuplicatedDecisionID(const ElementLink<TrigCompositeUtils::DecisionContainer>& dEL) const;

  /// Ensure that all present IDs correspond to configured chains
  StatusCode validateDecisionIDs(const ElementLink<TrigCompositeUtils::DecisionContainer>& dEL) const;

  /// Ensure that the Decision has at least one valid parent, unless it is a initial Decision from the L1 Decoder
  StatusCode validateParentLinking(const ElementLink<TrigCompositeUtils::DecisionContainer>& dEL) const;

  /// Execute all checks on one node in the graph, d, then recursive call self on all parent nodes up to L1.
  StatusCode recursiveValidateGraph(const ElementLink<TrigCompositeUtils::DecisionContainer>& dEL) const;

  /// Print header line
  void printBangs() const;

  /// A problem was found, print common output data
  void printErrorHeader(const ElementLink<TrigCompositeUtils::DecisionContainer>& dEL)  const;

  /// @}

  /// Common base function to print information on chains passed by objects considered in the hypo
  StatusCode printDebugInformation(SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle,
    MSG::Level lvl) const;
  
  /// input decisions
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_input { this, "HypoInputDecisions", "UNSPECIFIED_INPUT", "Input Decision (implicit)" };
  /// output decisions
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_output { this, "HypoOutputDecisions", "UNSPECIFIED_OUTPUT", "Ouput Decision" };
  /// Enabling of detailed validation checks for use during development
  Gaudi::Property<bool> m_runtimeValidation { this, "RuntimeValidation", false, "Enable detailed runtime validation of HypoAlg output, and upstream Decisions." };
    
  // for future implementation: ToolHandleArray<ITestHypoTool> m_tools { this, "HypoTools", {}, "Hypo tools" };


};


#endif // DECISIONHANDLING_HYPOBASE_H
