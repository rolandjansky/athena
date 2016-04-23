/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AsgTools/AsgToolsConf.h"
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)

#include <iostream>
#include <boost/assign/std/vector.hpp>

#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/ChainGroupManager.h"
#include "TrigDecisionTool/DecisionAccess.h"
#include "TrigDecisionTool/ConfigurationAccess.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

using namespace boost::assign;

int main() {

  Trig::TrigDecisionTool TDT ("name", "type");

  // this should happen inside Athena but must be called by ARA
  TDT.initialize();

  // get links to the primary pieces
  Trig::ChainGroupManager* manager = TDT.getChainGroupManager();
  const Trig::ConfigurationAccess& CA = TDT.config();
  const Trig::DecisionAccess& DA = TDT.decision();

  // build a chain group on the fly as if we were users
  std::cout << CA.testFunction("EF_foo,EF_bar") << std::endl;

  // build a chain group on the fly and use the reference
  const Trig::ChainGroup* happy=manager->get("EF_foo");
  std::cout << CA.testFunction(happy) << std::endl;
  std::cout << happy->dumpFixedNames() << std::endl;

  std::cout << "Passed: " << std::endl;
  const Trig::ChainGroup* foo=TDT.createChainGroup("EF_happy, EF_sad");
  std::cout << DA.isPassed(*foo) << std::endl;
  std::cout << DA.isPassed("EF_happy", TrigDefs::ignoreIOV) << std::endl;


  // dump the registry
  manager->dumpRegistry();

  // conditions
  std::cout << DA.isHLT("EF_foo") << std::endl;


  std::cout << "Now updating" << std::endl;
  TDT.update();
  manager->setValidTriggers("EF_foo, EF_bar, EF_contented, EF_happy");

  manager->dumpRegistry();
  std::cout << DA.isPassed(*foo) << std::endl;
  std::cout << DA.isPassed(*foo, TrigDefs::Physics) << std::endl;
  std::cout << DA.isPassed(*foo, TrigDefs::ignoreIOV) << std::endl;

}
#else
int main() {
}
#endif // full Athena env
