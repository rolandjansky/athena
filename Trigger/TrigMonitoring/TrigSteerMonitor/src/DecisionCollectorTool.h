/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_DECISIONCOLLECTORTOOL_H
#define TRIGSTEERMONITOR_DECISIONCOLLECTORTOOL_H

#include "./IDecisionCollector.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"

// STL includes
#include <string>

/**
 * @class DecisionCollectorTool
 * @brief
 **/
class DecisionCollectorTool : public extends<AthAlgTool, IDecisionCollector>  {
public:
  DecisionCollectorTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~DecisionCollectorTool() override;
    
  virtual StatusCode getDecisions( std::vector<TrigCompositeUtils::DecisionID>& ) const override;
  virtual StatusCode getSequencesPerEvent( std::set<std::string>& ) const;
  virtual StatusCode getSequencesNames( std::set<std::string>& ) const;
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_decisionsKey{ this, "Decisions", {}, "Containers from which the decisions need to be red" };
  // in future we will also need a property to filter only the desired decision for combined chains (partial decisions should not be accounted)
  
private:

};

#endif // TRIGSTEERMONITOR_DECISIONCOLLECTORTOOL_H
