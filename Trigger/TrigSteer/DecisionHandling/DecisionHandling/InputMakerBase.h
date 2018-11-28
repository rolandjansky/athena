/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_INPUTMAKERBASE_H
#define DECISIONHANDLING_INPUTMAKERBASE_H 1

#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"

using namespace TrigCompositeUtils;

class InputMakerBase : public ::AthReentrantAlgorithm {
  /**
   * @class InputMakerBase
   * @brief Input Makers are used at the start of a sequence: retrieve filtered collection via the input decision from the previous step and write it out directly so it can be used as input by the reco alg that follows in sequence.
This is a base class for HLT InputMakers to reduce boilerplate and enforce the common naming scheme for decision handle keys. Derived classes will have to add specific reco data read & write keys to suit their purpose.
   **/
 public:
  /// constructor, to be called by sub-class constructors
  InputMakerBase( const std::string& name, ISvcLocator* pSvcLocator );
  /// destructor
  virtual ~InputMakerBase(); 
  /// initialise this base class and renounce input decision key handles
  virtual StatusCode sysInitialize() override;
  /// execute to be implemented in derived clas
  virtual StatusCode execute_r(const EventContext&) const override = 0;
  virtual StatusCode finalize() override = 0;
  virtual StatusCode initialize() override = 0;
  
 protected:
  /// methods for derived classes to access handles of the base class input and output decisions; other read/write handles may be implemented by derived classes
  const SG::ReadHandleKeyArray<DecisionContainer>& decisionInputs() const;

  /// methods for derived classes to access handles of the base class input and output decisions; other read/write handles may be implemented by derived classes
  const SG::WriteHandleKeyArray<DecisionContainer>& decisionOutputs() const;

  // name of link to the RoI
  StringProperty m_roisLink { this, "RoIsLink", "initialRoI", "Name of EL to RoI object linked to the decision" };
  
  // helper methods for derived classes to reduce boiler plate code  //
  /////////////////////////////////////////////////////////////////////
  
  /// provides debug printout of the output of the algorithm
  StatusCode debugPrintOut(const EventContext& context, const std::vector< SG::WriteHandle<DecisionContainer> >& outputHandles) const;

   /// does the standard handling of input decisions: read from handles with all the checks, create corresponding output handles and link them, copies links and return outputHandles
  StatusCode decisionInputToOutput(const EventContext& context, std::vector< SG::WriteHandle<DecisionContainer> > & outputHandles) const;


  /// counts valid input decisions
  size_t countInputHandles( const EventContext& context ) const;
  
 private:
  
  /// input decisions, will be implicit (renounced).
  SG::ReadHandleKeyArray<DecisionContainer> m_inputs { this, "InputMakerInputDecisions", {}, "Input Decisions (implicit)" };

  /// output decisions
  SG::WriteHandleKeyArray<DecisionContainer> m_outputs { this, "InputMakerOutputDecisions", {}, "Ouput Decisions" };

  static const std::vector<std::string> m_baseLinks;

  StatusCode copyBaseLinks(const Decision* src, Decision* dest) const;

 
  /// copies decisions ID from unput to output
  //  StatusCode insertDecisions( const Decision* src, Decision* dest ) const;
  
};


#endif // DECISIONHANDLING_INPUTMAKERBASE_H
