/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "L1TopoOnlineMonitorHypoTool.h"

// TrigCompositeUtils types used here
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;

// TrigCompositeUtils methods used here
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::addDecisionID;
using TrigCompositeUtils::passed;
using TrigCompositeUtils::featureString;

// =============================================================================

L1TopoOnlineMonitorHypoTool::L1TopoOnlineMonitorHypoTool(const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent)
: AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {}

// =============================================================================

StatusCode L1TopoOnlineMonitorHypoTool::initialize() {
  ATH_CHECK(m_errorFlagsKey.initialize());
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode L1TopoOnlineMonitorHypoTool::decide(const InputVector& input,
                                               const EventContext& eventContext) const {
  // Retrieve the error flags
  SG::ReadHandle<xAOD::TrigCompositeContainer> errorFlagsCont(m_errorFlagsKey, eventContext);
  ATH_CHECK(errorFlagsCont.isValid());
  ATH_CHECK(!errorFlagsCont->empty());
  const xAOD::TrigComposite* errorFlags = errorFlagsCont->at(0);

  for (const auto& [decision, previousDecision] : input) {
    // Set a feature link
    ElementLink<xAOD::TrigCompositeContainer> featureLink(*errorFlagsCont, 0, eventContext);
    decision->setObjectLink(featureString(), featureLink);

    // Check previous decisions
    const DecisionIDContainer previousDecisionIDs{decisionIDs(previousDecision).begin(),
                                                  decisionIDs(previousDecision).end()};
    if (not passed(m_decisionId.numeric(), previousDecisionIDs)) {
      ATH_MSG_DEBUG("Skipping chain because previous step didn't pass");
      continue;
    }

    // Check error flags
    auto getFlag = [&errorFlags, this](const std::string& name){
      if (!errorFlags->hasDetail<bool>(name)) {
        ATH_MSG_WARNING("Error flag " << name << " missing from the TrigComposite object " << m_errorFlagsKey.key());
        return false;
      }
      const bool flag = errorFlags->getDetail<bool>(name);
      ATH_MSG_DEBUG("Error flag " << name << " = " << (flag ? "true" : "false"));
      return flag;
    };

    bool accept{false};
    if (m_acceptGenericRoiError.value()) {
      accept |= getFlag("hasGenericRoiError");
    }
    if (m_acceptGenericDaqError.value()) {
      accept |= getFlag("hasGenericDaqError");
    }
    if (m_acceptCrcTobError.value()) {
      accept |= getFlag("hasCrcTobError");
    }
    if (m_acceptCrcFibreError.value()) {
      accept |= getFlag("hasCrcFibreError");
    }
    if (m_acceptCrcDaqError.value()) {
      accept |= getFlag("hasCrcDaqError");
    }
    if (m_acceptRoibDaqDifference.value()) {
      accept |= getFlag("hasRoibDaqDifference");
    }
    if (m_acceptRoibCtpDifference.value()) {
      accept |= getFlag("hasRoibCtpDifference");
    }
    if (m_acceptDaqCtpDifference.value()) {
      accept |= getFlag("hasDaqCtpDifference");
    }

    ATH_MSG_DEBUG("The accept decision is: " << (accept ? "true" : "false"));
    if (accept) {
      addDecisionID(m_decisionId, decision);
    }
  }

  return StatusCode::SUCCESS;
}
