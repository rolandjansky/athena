/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_IDECISIONCOLLECTOR_H
#define TRIGSTEERMONITOR_IDECISIONCOLLECTOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

/**
 * @class IDecisionCollector
 * @brief Tools that collect decisions 
 **/
class IDecisionCollector : virtual public IAlgTool {
public: 
  DeclareInterfaceID(IDecisionCollector, 1, 0);
  
  virtual void getDecisions( std::vector<TrigCompositeUtils::DecisionID>& ) const = 0;
  
  virtual ~IDecisionCollector() override {}
}; 

#endif // TRIGSTEERMONITOR_IDECISIONCOLLECTOR_H
