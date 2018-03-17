/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_INPUTMAKERBASE_H
#define DECISIONHANDLING_INPUTMAKERBASE_H 1

#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"

class InputMakerBase : public ::AthReentrantAlgorithm {
  /**
   * @class InputMakerBase
   * @brief Input Makers are used at the start of a sequence: retrieve filtered collection via the input decision from the previous step and write it out directly so it can be used as input by the reco alg that follows in sequence.
This is a base class for HLT InputMakers to reduce boilerplate and enforce the common naming scheme for decision handle keys. Derived classes will have to add specific reco data read & write keys to suit their purpose.
   **/
 public:
  // initialise and as needed renounce standard handles
  StatusCode initialize() override;
  virtual StatusCode execute_r(const EventContext&) const = 0;
  //StatusCode execute(){};
  virtual StatusCode finalize() = 0;
  // for initialisation of derived classes, to be implemented by sub-classes
  virtual StatusCode subInitialize()= 0;  
 protected:
  // methods for derived classes to access input and output decisions
  const SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer>& decisionInputs() const;
  const SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer>& decisionOutputs() const;
 private:
  // input and output decisions, input will be implicit (renounced).
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_inputs { this, "InputDecisions", {}, "Input Decisions (implicit)" };
  SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_outputs { this, "OutputDecisions", {}, "Ouput Decisions" };
};


#endif //> DECISIONHANDLING_INPUTMAKERBASE_H
