/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasControl_MCTruthMenu_H
#define G4AtlasControl_MCTruthMenu_H

#include "G4AtlasControl/SimpleMenu.h"

/// @todo NEEDS DOCUMENTATION
class MCTruthMenu : public SimpleMenu {
public:
  MCTruthMenu();
  void listStrategies();
  void activateStrategy(const std::string strg_name,const std::string vol_name, int vol_level);
  void disactivateStrategy(const std::string strg_name);
  void listParameters();
  void secondarySaving(const std::string);
  void setTruthStrategiesParameter(const std::string,double);
};
#endif
