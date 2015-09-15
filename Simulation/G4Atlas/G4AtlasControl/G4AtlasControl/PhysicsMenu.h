/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsMenu_H
#define PhysicsMenu_H

#include <string>

#include "G4AtlasControl/PhysicsRegionProxy.h"
#include "G4AtlasControl/SimpleMenu.h"

class PhysicsMenu : public SimpleMenu {
friend class SimControl;
private:
   PhysicsMenu();
public:
   void PrintLists();
   void UsePhysicsList(const std::string);
   void ConstructPhysicsList();
   void SetGeneralCut(double);
	
   void SetNeutronTimeCut(double);
   void SetNeutronEnergyCut(double);
   void SetApplyCuts(bool);

   PhysicsRegionProxy CreateNewRegion(const std::string);
   void ListRegions();

   void SetRandomNumberService(const std::string);
   void SetVertexOverrideFile(const std::string);
   void SetVertexOverrideEventFile(const std::string);
};

#endif
