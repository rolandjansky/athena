/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_TRIGZVERTEXHYPOTOOL_H
#define TRIGMINBIAS_TRIGZVERTEXHYPOTOOL_H

// Package includes

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
// STL includes
#include <string>

/**
 * @class TrigZVertexHypoTool
 * @brief Applies selection on the number of tracks poting to vertex (actually on the vertex weight)
 **/
class TrigZVertexHypoTool : public ::AthAlgTool {
public:
  TrigZVertexHypoTool(const std::string& type, const std::string& name, const IInterface* parent);

  struct ZVertexInfo {
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigCompositeContainer* vertices;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };

  StatusCode decide(ZVertexInfo& decisions) const;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

private:
  HLT::Identifier m_decisionId;
  Gaudi::Property<int> m_minNumVertices{ this, "minNumVertices", 1, "Require at least that number of vertices (inclusive)" };
  Gaudi::Property<int> m_maxNumVertices{ this, "maxNumVertices", 100, "Require at most that number of vertices (inclusive)" };
  Gaudi::Property<int> m_minWeight{ this, "minWeight", 1, "Count vertices that at least have weight higher than this threshold (inclusive)" };
  Gaudi::Property<int> m_maxWeight{ this, "maxWeight", 1e6, "Count vertices that have weight smaller than this threshold (inclusive)" };
};

#endif // TRIGMINBIAS_TRIGZVERTEXHYPOTOOL_H
