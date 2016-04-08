/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/PhysicsMenu.h"

#include "FadsKinematics/GeneratorCenter.h"

PhysicsMenu::PhysicsMenu():SimpleMenu("PhysicsMenu")
{
  // std::cout <<" creating the PhysicsMenu "<<std::endl;
}

void PhysicsMenu::SetRandomNumberService(const std::string name)
{
  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->RandomNumberService(name);
}
void PhysicsMenu::SetVertexOverrideFile(const std::string path)
{
  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->VertexOverrideFile(path);
}
void PhysicsMenu::SetVertexOverrideEventFile(const std::string path)
{
  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->VertexOverrideEventFile(path);
}

