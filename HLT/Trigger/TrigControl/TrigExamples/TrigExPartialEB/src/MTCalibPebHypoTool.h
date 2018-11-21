/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEXPARTIALEB_MTCALIBPEBHYPOTOOL_H
#define TRIGEXPARTIALEB_MTCALIBPEBHYPOTOOL_H

// Trigger includes
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// TDAQ includes
#include "eformat/StreamTag.h"

/** @class MTCalibPebHypoTool
 *  @brief Base class for tools used by MTCalibPebHypoAlg
 **/
class MTCalibPebHypoTool : virtual public AthAlgTool {
public:
  /// Standard constructor
  MTCalibPebHypoTool(const std::string& type, const std::string& name, const IInterface* parent);
  /// Standard destructor
  virtual ~MTCalibPebHypoTool();

  // ------------------------- AthAlgTool methods ------------------------------
  virtual StatusCode initialize() override;

  // ------------------------- Public types ------------------------------------
  struct Input {
    Input(TrigCompositeUtils::Decision* d, const EventContext& ctx)
    : decision(d), eventContext(ctx) {}
    TrigCompositeUtils::Decision* decision;
    const EventContext& eventContext;
  };

  // ------------------------- Specific methods of this class ------------------
  /// Decides whether to accept the event
  StatusCode decide(const Input& input) const;

private:
  // ------------------------- Properties --------------------------------------
  Gaudi::Property<double> m_acceptRate {
    this, "RandomAcceptRate", -1,
    "Rate of random accepts, <=0 is never, >=1 is always"
  };
  Gaudi::Property<unsigned int> m_burnTimePerCycleMillisec {
    this, "BurnTimePerCycleMillisec", 0,
    "Burn time per cycle in milliseconds"
  };
  Gaudi::Property<unsigned int> m_numBurnCycles {
    this, "NumBurnCycles", 0,
    "Number of time burning cycles"
  };
  Gaudi::Property<bool> m_burnTimeRandomly {
    this, "BurnTimeRandomly", true,
    "If true, burn time per cycle is a random value from uniform distribution between 0 and the given value"
  };
  Gaudi::Property<bool> m_failOnTimeout {
    this, "FailOnTimeout", true,
    "If true, the execution will return StatusCode::FAILURE when Athena timeout is detected"
  };

  // ------------------------- Other private members ---------------------------
  /// The decision id of the tool instance
  HLT::Identifier m_decisionId;
  /// The ConfiguredStreamTags property converted to vector of eformat stream tags
  std::vector<eformat::helper::StreamTag> m_configuredStreamTags;
};

#endif // TRIGEXPARTIALEB_MTCALIBPEBHYPOTOOL_H
