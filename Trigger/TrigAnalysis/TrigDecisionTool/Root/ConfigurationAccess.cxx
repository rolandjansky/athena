/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

#include "TrigDecisionTool/ConfigurationAccess.h"
#include "TrigDecisionTool/TDTUtilities.h"

Trig::ConfigurationAccess::ConfigurationAccess() {}

Trig::ConfigurationAccess::~ConfigurationAccess() {}


void Trig::ConfigurationAccess::configurationUpdate( const TrigConf::HLTChainList* confChains,
                                                     const TrigConf::CTPConfig* ctp) {
  // this is the update method which is called whenever a lower level element has changed
  // or needs to be changed
  cgm()->update(confChains, ctp);
}

//
// Triggers
//

std::vector< std::string > Trig::ConfigurationAccess::getListOfTriggers(const Trig::ChainGroup* chaingroup) const {
  return chaingroup->getListOfTriggers();
}

std::vector< std::string > Trig::ConfigurationAccess::getListOfTriggers(const std::string& triggerNames) const {
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(triggerNames));
  return getListOfTriggers(g);
}

//
// Streams
//

std::vector< std::string > Trig::ConfigurationAccess::getListOfStreams(const Trig::ChainGroup* chaingroup) const {
  return chaingroup->getListOfStreams();
}

std::vector< std::string > Trig::ConfigurationAccess::getListOfStreams(const std::string& triggerNames) const {
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(triggerNames));
  return getListOfStreams(g);
}

//
// Groups
//

std::vector< std::string > Trig::ConfigurationAccess::getListOfGroups(const Trig::ChainGroup* chaingroup) const {
  return chaingroup->getListOfGroups();
}

std::vector< std::string > Trig::ConfigurationAccess::getListOfGroups(const std::string& triggerNames) const {
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(triggerNames));
  return getListOfGroups(g);
}

//
// Trigger Elements
//

std::vector< std::vector< std::string > > Trig::ConfigurationAccess::getListOfTriggerElements(const Trig::ChainGroup* chaingroup) const {
  return chaingroup->getListOfTriggerElements();
}

std::vector< std::vector< std::string > > Trig::ConfigurationAccess::getListOfTriggerElements(const std::string& triggerNames) const {
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(triggerNames));
  return getListOfTriggerElements(g);
}
//
// Prescale
//

float Trig::ConfigurationAccess::getPrescale(const Trig::ChainGroup* chainGroup, 
                                             unsigned int condition) const
{
  return chainGroup->getPrescale(condition);
}

float Trig::ConfigurationAccess::getPrescale(const std::string& chain, 
                                             unsigned int condition ) const
   
{
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(chain));
  return getPrescale(g,condition);
}
