/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHYPOCOMMONTOOLS_TRIGGENERICHYPOTOOL_H
#define TRIGHYPOCOMMONTOOLS_TRIGGENERICHYPOTOOL_H

#include "Gaudi/Property.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigGenericHypoTool : public :: AthAlgTool {
    
 public:

  struct HypoToolInfo {
  HypoToolInfo( TrigCompositeUtils::Decision* d, 
		const xAOD::TrigComposite*  tc,
		const TrigCompositeUtils::Decision* previousDecision )
  : decision( d ), 
      trigComp( tc ),
      previousDecisionIDs(TrigCompositeUtils::decisionIDs( previousDecision ).begin(), 
			  TrigCompositeUtils::decisionIDs( previousDecision ).end() )
    {}
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigComposite* trigComp;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };

  TrigGenericHypoTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize() override;
  StatusCode decide(const std::vector<HypoToolInfo>& input) const;
    
 private:
  HLT::Identifier m_decisionId;
  SG::ReadDecorHandleKey<xAOD::TrigCompositeContainer> m_passKey{this, "PassString", "TrigComposite.pass", "TrigComposite should pass"};

};

#endif 
