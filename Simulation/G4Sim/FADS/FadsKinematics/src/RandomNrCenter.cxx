/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsKinematics/RandomNrCenter.h"
#include <CLHEP/Random/Random.h>

CLHEP::HepRandomEngine* RandomNrCenter::theDefaultEngine=CLHEP::HepRandom::getTheEngine();
CLHEP::HepRandomEngine* RandomNrCenter::theCurrentEngine=CLHEP::HepRandom::getTheEngine();

void RandomNrCenter::SaveEngineStatus(std::string s)
{
	CLHEP::HepRandom::saveEngineStatus(s.c_str());
}
void RandomNrCenter::RestoreEngineStatus(std::string s)
{
	CLHEP::HepRandom::restoreEngineStatus(s.c_str());
}
void RandomNrCenter::SetEngineSeed(long int i)
{
	CLHEP::HepRandom::setTheSeed(i,3);
}
long int RandomNrCenter::GetEngineSeed()
{
	return CLHEP::HepRandom::getTheSeed();
}
void RandomNrCenter::SetEngine()
{
}

void RandomNrCenter::ResetEngine()
{
	theCurrentEngine=theDefaultEngine;
}

CLHEP::HepRandomEngine* RandomNrCenter::GetEngine()
{
	return theCurrentEngine;
}

#include <CLHEP/Random/RandGauss.h>
double RandomNrCenter::Rngaus(double mean, double sigma)
{
        return CLHEP::RandGauss::shoot(mean,sigma); // FIXME: should be using RandGaussZiggurat
}
