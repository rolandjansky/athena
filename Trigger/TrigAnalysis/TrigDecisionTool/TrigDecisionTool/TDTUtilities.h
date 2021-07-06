/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_UTILITIES_H
#define TRIGGER_DECISION_TOOL_UTILITIES_H
/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 *
 * @brief utility functions
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 *
 ***********************************************************************************/
#include <string>
#include <vector>

class TTree;

namespace HLT {
  class TriggerElement;
}

namespace Trig {

  /**
   * @brief makes a split of list of chains into the vector of chains
   **/
  std::vector< std::string > convertStringToVector(const std::string& triggerNames);

  /**
   * @brief normalizes the list of triggers (patterns) by sorting and uniquing them
   **/
  std::vector< std::string > keyWrap(const std::vector< std::string >& triggerNames);  



  /**
   * @brief converts TEid to TE name (this may not always work, it depends on the availability of config)
   **/
  std::string getTEName(const HLT::TriggerElement& te);  
}
#endif
