/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsMenu_H
#define PhysicsMenu_H

#include "G4AtlasControl/SimpleMenu.h"

#include <string>

class PhysicsMenu : public SimpleMenu {
friend class SimControl;
private:
   PhysicsMenu();
public:
   void SetRandomNumberService(const std::string);
   void SetVertexOverrideFile(const std::string);
   void SetVertexOverrideEventFile(const std::string);
};

#endif
