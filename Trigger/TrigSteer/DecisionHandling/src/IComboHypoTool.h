/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_ICOMBOHYPOTOOL_H
#define DECISIONHANDLING_ICOMBOHYPOTOOL_H 1

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "GaudiKernel/IAlgTool.h"
#include "ComboHypoCombination.h"

/**
 * @class $(klass)s
 * @brief 
 **/

typedef std::map<TrigCompositeUtils::DecisionID, ComboHypoCombination> CombinationMap;

class IComboHypoTool: virtual public ::IAlgTool { 
  
 public: 
  DeclareInterfaceID(IComboHypoTool, 1, 0);
  
  virtual ~IComboHypoTool() {};
 IComboHypoTool(const std::string& name)   : m_decisionId(  HLT::Identifier::fromToolName( name ) ) {}

  // vector of decision that satisfy the mult. requirement

  virtual StatusCode decide(std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
			    const CombinationMap IDCombMap, std::map<TrigCompositeUtils::DecisionID, std::vector< std::pair<uint32_t,uint16_t> >> & passingComb ) const = 0;//TrigCompositeUtils::DecisionIDContainer& passingIds ) const = 0;
  virtual HLT::Identifier decisionId() const { return m_decisionId; }
  
 protected:
  HLT::Identifier m_decisionId;
  
}; 



#endif //> !DECISIONHANDLING_ICOMBOHYPOTOOL_H
