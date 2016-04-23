/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : DecisionAnalysis
 *
 * @brief access to information about the trigger decision
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 *
 ***********************************************************************************/

#include <vector>
#include <exception>
#include "TrigDecisionTool/DecisionAccess.h"

#include "TrigSteeringEvent/Chain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"


Trig::DecisionAccess::DecisionAccess() {}

Trig::DecisionAccess::~DecisionAccess() {}


bool Trig::DecisionAccess::isPassed(const Trig::ChainGroup* chainGroup, 
				    unsigned int condition) const
{
  return chainGroup->isPassed(condition);
}

bool Trig::DecisionAccess::isPassed(const std::string& chain, 
				    unsigned int condition ) const

{
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(chain));
  return isPassed(g,condition);
}

bool Trig::DecisionAccess::isPassed(const std::string& chain) const {
  return isPassed(chain, TrigDefs::Physics);
}

unsigned int Trig::DecisionAccess::isPassedBits(const Trig::ChainGroup* chainGroup) const
{
  return chainGroup->isPassedBits();
}

unsigned int Trig::DecisionAccess::isPassedBits(const std::string& chain) const

{
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(chain));
  return isPassedBits(g);
}


Trig::FeatureContainer
Trig::DecisionAccess::features(const Trig::ChainGroup* chain, unsigned int condition) const {
  return chain->features(condition);
}

Trig::FeatureContainer
Trig::DecisionAccess::features(const std::string& chain, unsigned int condition) const {
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(chain));
  return features(g,condition);
}

char
Trig::DecisionAccess::getBGCode() const {
  if( ! const_cast< Trig::CacheGlobalMemory* >( cgm() )->assert_decision() ) {
    ATH_MSG_ERROR("Something went wrong in decoding the trigger decision");
    return 0;
  }
  return cgm()->m_bgCode;
}

