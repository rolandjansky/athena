/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/RandomNrMenu.h"
#include "FadsRun/RandomNrCenter.h"

RandomNrMenu::RandomNrMenu():SimpleMenu("RandomNrMenu")
{
  // std::cout << "Creating the Random Nr. Menu "<<std::endl;
}

void RandomNrMenu::setSeed(long seed)
{
		RandomNrCenter rc;
		rc.SetEngineSeed(seed);
}

void RandomNrMenu::saveStatus(const std::string fileName)
{
		RandomNrCenter rc;
		rc.SaveEngineStatus(fileName);
}

void RandomNrMenu::retrieveStatus(const std::string fileName)
{
		RandomNrCenter rc;
		rc.RestoreEngineStatus(fileName);
}

void RandomNrMenu::setEngine(const std::string)
{
		log() << MSG::ERROR << " method not implemented " << endreq;
}

void RandomNrMenu::resetEngine()
{
		log() << MSG::ERROR << " method not implemented " << endreq;
}
