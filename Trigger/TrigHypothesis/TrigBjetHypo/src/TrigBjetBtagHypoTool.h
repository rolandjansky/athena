/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetBtagHypoTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Carlo Varni
// EMAIL:    carlo.varni@cern.ch
// 
// ************************************************

#ifndef TRIGBJETHYPO_TRIGBJETHYPOTOOL_H
#define TRIGBJETHYPO_TRIGBJETHYPOTOOL_H 1

#include "IBJetHypoDiscriminantCheck.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"

class TrigBjetBtagHypoTool : virtual public ::AthAlgTool {

 public:
  struct TrigBjetBtagHypoToolInfo {
    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    ElementLink< xAOD::BTaggingContainer > btaggingEL;
    ElementLink< xAOD::VertexContainer > vertexEL;
    TrigCompositeUtils::Decision* decision;
    const InDet::BeamSpotData* beamSpot;
  };


 public:

  /** @brief Constructor. */
  TrigBjetBtagHypoTool(
    const std::string& type,
		const std::string& name,
		const IInterface* parent
    );

  virtual StatusCode initialize() override;

  TrigCompositeUtils::DecisionID decisionId() const;
  const HLT::Identifier getId() const;

  StatusCode decide( std::vector< TrigBjetBtagHypoToolInfo >& ) const;

 private:
  HLT::Identifier m_decisionId;

  ToolHandleArray<IBJetHypoDiscriminantCheck> m_checks {this, "checks", {},
      "List of checks to run for one chain"};

  /** @brief DeclareProperty: to monitor method used to perform the cut. */
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "",
    "Monitoring tool" };

  Gaudi::Property<std::map<std::string, std::string>> m_monMap {this,
      "monitoredFloats", {}, "(EDM name, monitoring name) pairs"};
  std::vector<std::pair<SG::AuxElement::ConstAccessor<float>, std::string>> m_monPairs;

  Gaudi::Property<bool> m_vetoBadBeamspot {this, "vetoBadBeamspot", true,
    "Veto btagging if the beamspot is bad"};

};

#endif // !TRIGBJETHYPO_TRIGBJETHYPOTOOL_H
