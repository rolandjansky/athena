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

// System includes
#include <random>

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
    Input(TrigCompositeUtils::Decision* d)
    : decision(d) {}
    TrigCompositeUtils::Decision* decision;
  };

  // ------------------------- Specific methods of this class ------------------
  /// Decides whether to accept the event
  StatusCode decide(const std::vector<Input>& inputs) const;

private:
  // ------------------------- Properties --------------------------------------
  Gaudi::Property<double> m_acceptRate {
    this,
    "RandomAcceptRate",
    -1,
    "Rate of random accepts, <=0 is never, >=1 is always"
  };

  // ------------------------- Other private members ---------------------------
  /// The decision id of the tool instance
  HLT::Identifier m_decisionId;
  /// The ConfiguredStreamTags property converted to vector of eformat stream tags
  std::vector<eformat::helper::StreamTag> m_configuredStreamTags;
};

#endif // TRIGEXPARTIALEB_MTCALIBPEBHYPOTOOL_H
