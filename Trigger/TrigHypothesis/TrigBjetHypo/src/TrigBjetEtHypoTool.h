/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetEtHypoTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetEtHypo
//
// AUTHOR:   Carlo Varni
// EMAIL:    Carlo.Varni@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETHYPO_TRIGBJETETHYPOTOOL_H
#define TRIGBJETHYPO_TRIGBJETETHYPOTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h" 

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

class TrigBjetEtHypoTool : virtual public ::AthAlgTool {

 public:
  struct TrigBjetEtHypoToolInfo {
    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    ElementLink< xAOD::JetContainer > jetEL;
    ElementLink< xAOD::VertexContainer > vertexEL;
    TrigCompositeUtils::Decision* decision;
  };

  
 public:
  /** @brief Constructor. */
  TrigBjetEtHypoTool (const std::string& type,
		      const std::string& name,
		      const IInterface* parent );

  virtual StatusCode initialize() override;

  TrigCompositeUtils::DecisionID decisionId() const;
  const HLT::Identifier getId() const;

  StatusCode decide( std::vector< TrigBjetEtHypoToolInfo >& ) const;

 private:
  HLT::Identifier m_decisionId;

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  Gaudi::Property< bool > m_acceptAll {this,"AcceptAll",false,"if acceptAll flag is set to true, every event is taken"};
  /** @brief DeclareProperty: Et threshold cut. */
  Gaudi::Property< float > m_etThreshold {this,"EtThreshold",0.0,"Et threshold cut"};
  /** @brief DeclareProperty: min eta threshold cut. */
  Gaudi::Property< float > m_minEtaThreshold {this,"MinEtaThreshold",0.0,"Min Eta threshold cut"};
  /** @brief DeclareProperty: max eta threshold cut. */
  Gaudi::Property< float > m_maxEtaThreshold {this,"MaxEtaThreshold",0.0,"Max Eta threshold cut"};
};

#endif  // !TRIGBJETHYPO_TRIGBJETETHYPOTOOL_H
