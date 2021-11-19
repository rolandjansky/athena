/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECISIONHANDLING_INPUTMAKERBASE_H
#define DECISIONHANDLING_INPUTMAKERBASE_H 1

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"

/**
 * @class InputMakerBase
 * @brief Input Makers are used at the start of a sequence: retrieve filtered collection via the input decision from the previous step and write it out directly 
 * so it can be used as input by the reconstruction algorithms that follow in sequence.
 * This is a base class for HLT InputMakers to reduce boilerplate and enforce the common naming scheme for decision handle keys. 
 * Derived classes implement how the reconstruction is started. Current options are via WriteHandle<TrigROIDecscriptorCollection>, as used by the
 * InputMakerForRoI specialisation. Or via the spawning of EventView instances over a given sequencer, as used by the EventViewCreatorAlgorithm specialisation.
 **/
class InputMakerBase : public ::AthReentrantAlgorithm {

 public:
  /// constructor, to be called by sub-class constructors
  InputMakerBase( const std::string& name, ISvcLocator* pSvcLocator );
  /// destructor
  virtual ~InputMakerBase(); 
  /// initialise this base class and renounce input decision key handles
  virtual StatusCode sysInitialize() override;

 protected:
  /**
   * @struct MatchingCache
   * @brief Used to cache each incoming Decision object's ElementLink which is being used to identify the Decision object for the purposes of de-duplication merging
   **/
  struct MatchingCache {
    void setMatchingHash(const TrigCompositeUtils::Decision* inputDecision, const uint64_t matchingHash);
    void linkOutputToInput(const TrigCompositeUtils::Decision* outputDecision, const TrigCompositeUtils::Decision* inputDecision);
    uint64_t getMatchingHash(const TrigCompositeUtils::Decision* outputDecision) const;
    std::map<const TrigCompositeUtils::Decision*, const TrigCompositeUtils::Decision*> m_outputDecToInputDec;
    std::map<const TrigCompositeUtils::Decision*, uint64_t> m_inputDecToMatchingHash;
  };

  /// methods for derived classes to access handles of the base class input and output decisions; other read/write handles may be implemented by derived classes
  const SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer>& decisionInputs() const;

  /// methods for derived classes to access handles of the base class input and output decisions; other read/write handles may be implemented by derived classes
  const SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer>& decisionOutputs() const;

  // name of link to the RoI
  StringProperty m_roisLink {this, "RoIsLink", "initialRoI",
    "Name of EL to RoI object linked to the decision, used in merging input Decision objects when mergeUsingFeature=False." };
  
  Gaudi::Property<bool> m_mergeUsingFeature {this, "mergeUsingFeature", false,
    "True=the IParicle-derived feature from the previous step is used to determine identical inputs. False=the ROI located with the RoIsLink property is used to determine identical inputs" };

  Gaudi::Property<bool> m_isEmptyStep {this, "isEmptyStep", false,
    "True if this InputMaker instance is being used in an empty step. Forgoes the requirement to link an ROI, and auto-configures the 'mergeUsingFeature' property at runtime." };
  
  // helper methods for derived classes to reduce boiler plate code  //
  /////////////////////////////////////////////////////////////////////

  /// provides debug printout of the output of the algorithm
  void debugPrintOut(const EventContext& context, SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle) const;

  /// does the standard handling of input decisions: read from handles with all the checks, create merged output handles and link them, copies links and return outputHandles
  StatusCode decisionInputToOutput(const EventContext& context, SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle) const;  

  /// Checks for merge-able Decision objects coming from N upstream filters. Check based on most-recent element link with name 'linkNameToMatch'. Works for any link type.
  size_t matchDecision(const TrigCompositeUtils::DecisionContainer* outDecisions, 
    const TrigCompositeUtils::Decision* toMatch, 
    const std::string& linkNameToMatch,
    MatchingCache& matchingCache) const;

  /// Wrapper around matchDecision. Returns boolean if the match was successful.
  bool matchInCollection(const TrigCompositeUtils::DecisionContainer* outDecisions, 
    const TrigCompositeUtils::Decision* toMatch,
    size_t& matchIndex,
    bool& usedROIMatchingFlag,
    MatchingCache& matchingCache) const;

  /// Searches from toMatch to locate a single (type-less) Element Link with given edge name. Returns a hash corresponding to this Element Link
  uint64_t getMatchingHashForDecision(const TrigCompositeUtils::Decision* toMatch, const std::string& linkNameToMatch) const;

 private:

  /// input decisions array, will be implicit (renounced).
  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_inputs { this, "InputMakerInputDecisions", {}, "Input Decisions (implicit)" };

  /// output decisions
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_outputs { this, "InputMakerOutputDecisions", "", "Output Decisions" };
  
};

#endif // DECISIONHANDLING_INPUTMAKERBASE_H
