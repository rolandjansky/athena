/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigZVertexHypoTool.h"

using TrigCompositeUtils::addDecisionID;

TrigZVertexHypoTool::TrigZVertexHypoTool(const std::string& type, const std::string& name, const IInterface* parent) :
  ::AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {}


StatusCode TrigZVertexHypoTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigZVertexHypoTool::decide(TrigZVertexHypoTool::ZVertexInfo& info) const {

  if (info.previousDecisionIDs.count(m_decisionId.numeric()) == 0) {
    ATH_MSG_DEBUG("Already rejected");
    return StatusCode::SUCCESS;
  }

  int count = 0;
  for (auto vertex : *(info.vertices)) {
    const float weight = vertex->getDetail<float>("zfinder_vtx_weight");
    if (m_minWeight <= weight and weight <= m_maxWeight) {
      count++;
    }
    ATH_MSG_DEBUG("Found vertex of weight " << weight << " count is now " << count);
  }
  if (m_minNumVertices <= count and count <= m_maxNumVertices) {
    addDecisionID(m_decisionId.numeric(), info.decision);
    ATH_MSG_DEBUG("Event accepted");
  }
  else {
    ATH_MSG_DEBUG("Event rejected");
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigZVertexHypoTool::finalize() {
  return StatusCode::SUCCESS;
}
