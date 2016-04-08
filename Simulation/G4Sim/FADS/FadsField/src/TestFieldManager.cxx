/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/TestFieldManager.h"

#include "FadsField/FieldManagerFactoryT.h"

FADS::FieldManagerFactoryT<TestFieldManager> a("TestFieldManager");
FADS::FieldManagerFactoryT<TestFieldManager> b("AnotherFieldManager");


void TestFieldManager::ConfigureForTrack(const G4Track* )
{
	std::cout<<"This is TestFieldManager::ConfigureForTrack"<<std::endl;
}
