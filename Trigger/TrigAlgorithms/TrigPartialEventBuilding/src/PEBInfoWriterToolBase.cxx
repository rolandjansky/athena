/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigPartialEventBuilding/PEBInfoWriterToolBase.h"

// TrigCompositeUtils types used here
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;

// TrigCompositeUtils methods used here
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;

// =============================================================================

PEBInfoWriterToolBase::PEBInfoWriterToolBase(const std::string& type, const std::string& name, const IInterface* parent)
: AthAlgTool(type,name,parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {}

// =============================================================================

PEBInfoWriterToolBase::~PEBInfoWriterToolBase() {}

// =============================================================================

StatusCode PEBInfoWriterToolBase::decide(std::vector<Input>& inputs) const {
  for (Input& input : inputs) {
    // Skip if previous step for this chain didn't pass
    if (not TrigCompositeUtils::passed(m_decisionId.numeric(), input.previousDecisionIDs)) {
      ATH_MSG_DEBUG("Skipping chain because previous step didn't pass");
      continue;
    }

    // Create new PEB Info for this input
    PEBInfo pebInfo = createPEBInfo(input);

    // Merge with previous ROBs    
    std::vector<uint32_t> previousRobs;
    if (input.decision->getDetail(robListKey(), previousRobs)) {
      pebInfo.robs.insert(previousRobs.begin(), previousRobs.end());
    }

    // Merge with previous SubDets
    std::vector<uint32_t> previousSubDets;
    if (input.decision->getDetail(subDetListKey(), previousSubDets)) {
      pebInfo.subdets.insert(previousSubDets.begin(), previousSubDets.end());
    }

    // Attach the PEB Info to the decision
    std::vector<uint32_t> robVec(pebInfo.robs.begin(), pebInfo.robs.end());
    if (not input.decision->setDetail(robListKey(), robVec)) {
      ATH_MSG_ERROR("Failed to attach the detail " << robListKey() << " to the decision");
      return StatusCode::FAILURE;
    }
    std::vector<uint32_t> subDetVec(pebInfo.subdets.begin(), pebInfo.subdets.end());
    if (not input.decision->setDetail(subDetListKey(), subDetVec)) {
      ATH_MSG_ERROR("Failed to attach the detail " << subDetListKey() << " to the decision");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Attached PEB Info with " << robVec.size() << " ROBs and "
                  << subDetVec.size() << " SubDets to the decision object.");
    ATH_MSG_DEBUG("PEBInfo: " << pebInfo);

    // Make an accept decision
    TrigCompositeUtils::addDecisionID(m_decisionId, input.decision);
  }
  return StatusCode::SUCCESS;
}

// =============================================================================

std::ostream& operator<< (std::ostream& str, const PEBInfoWriterToolBase::PEBInfo& pebi) {
  str << "ROBs = [";
  size_t i=0;
  for (const uint32_t robid : pebi.robs) {
    str << "0x" << std::hex << robid << std::dec;
    ++i;
    if (i!=pebi.robs.size()) str << ", ";
  }
  str << "], SubDets = [";
  i=0;
  for (const uint32_t subdetid : pebi.subdets) {
    str << "0x" << std::hex << subdetid << std::dec;
    ++i;
    if (i!=pebi.subdets.size()) str << ", ";
  }
  str << "]";
  return str;
}
