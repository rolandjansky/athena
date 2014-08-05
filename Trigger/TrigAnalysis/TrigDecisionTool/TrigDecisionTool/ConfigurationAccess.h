/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_CONFIGURATION_ACCESS_H
#define TRIGGER_DECISION_TOOL_CONFIGURATION_ACCESS_H
/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : DecisionAnalysis
 *
 * @brief access to information about the trigger configuration
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 *
 ***********************************************************************************/
#include <string>
#include <vector>

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/ChainGroup.h"

namespace Trig {

  class ConfigurationAccess : public virtual Logger  {
  public:
    ConfigurationAccess();
    virtual ~ConfigurationAccess();

    std::vector< std::string > getListOfTriggers(const Trig::ChainGroup* chaingroup) const;
    std::vector< std::string > getListOfTriggers(const std::string& triggerNames=".*") const;

    std::vector< std::string > getListOfStreams(const Trig::ChainGroup* chaingroup) const;
    std::vector< std::string > getListOfStreams(const std::string& triggerNames=".*") const;

    std::vector< std::string > getListOfGroups(const Trig::ChainGroup* chaingroup) const;
    std::vector< std::string > getListOfGroups(const std::string& triggerNames=".*") const;

    std::vector< std::vector< std::string > > getListOfTriggerElements(const Trig::ChainGroup* chaingroup) const;
    std::vector< std::vector< std::string > > getListOfTriggerElements(const std::string& triggerNames=".*") const;

    float getPrescale(const Trig::ChainGroup* chaingroup, unsigned int condition=TrigDefs::Physics) const;
    float getPrescale(const std::string& triggerNames="EF._*", unsigned int condition=TrigDefs::Physics) const;

  protected:

    // update methods for IOV awareness etc
    void configurationUpdate(const TrigConf::HLTChainList* confChains,
                             const TrigConf::CTPConfig* confItems);
    

  private:
    
    virtual Trig::CacheGlobalMemory* cgm() const =0; 


    
  };
} // End of namespace

#endif
