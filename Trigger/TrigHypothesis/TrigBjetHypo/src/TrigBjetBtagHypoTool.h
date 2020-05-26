/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

// This is in current hypo, not sure if needed
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

// Are these new?
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgTool.h" 
#include "BeamSpotConditionsData/BeamSpotData.h"

class TrigBjetBtagHypoTool : virtual public ::AthAlgTool {

 public:
  struct TrigBjetBtagHypoToolInfo {
    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    ElementLink< xAOD::BTaggingContainer > btaggingEL;
    ElementLink< xAOD::VertexContainer > vertexEL;
    TrigCompositeUtils::Decision* decision;
  };


 public:

  /** @brief Constructor. */
  TrigBjetBtagHypoTool (const std::string& type,
		const std::string& name,
		const IInterface* parent );

  virtual StatusCode initialize() override;

  TrigCompositeUtils::DecisionID decisionId() const;
  const HLT::Identifier getId() const;

  StatusCode decide( std::vector< TrigBjetBtagHypoToolInfo >& ) const;

 private:
  HLT::Identifier m_decisionId;

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  Gaudi::Property< bool > m_acceptAll {this,"AcceptAll",false,"if acceptAll flag is set to true, every event is taken"};
  /** @brief DeclareProperty: Tagger used to perform the selection. */
  Gaudi::Property< std::string > m_methodTag {this,"MethodTag","","Tagger used to perform the selection"};    
  /** @brief DeclareProperty: lower bound of the discriminant variable to be selected (if flag acceptAll is set to false) for MV2 tagger. */
  Gaudi::Property< double > m_bTaggingCut {this,"BTaggingCut",-20.,"lower bound of the discriminant variable to be selected for b-tagging"};

  /** @brief DeclareProperty: to monitor method used to perform the cut. */
  //  float m_monitorMethod;
  //  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" }; Temporary commenting this out
};

#endif // !TRIGBJETHYPO_TRIGBJETHYPOTOOL_H
