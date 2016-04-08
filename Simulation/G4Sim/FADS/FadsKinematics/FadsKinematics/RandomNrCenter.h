/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RandomNrCenter_H
#define RandomNrCenter_H

#include <CLHEP/Random/Random.h>
#include <string>

class RandomNrCenter {
private:
	static CLHEP::HepRandomEngine *theDefaultEngine;
	static CLHEP::HepRandomEngine *theCurrentEngine;
public:
	void SaveEngineStatus(std::string);
	void RestoreEngineStatus(std::string);
	void SetEngineSeed(long int);
	long int GetEngineSeed();
	void SetEngine();
	void ResetEngine();
	CLHEP::HepRandomEngine *GetEngine();
	
	static double Rngaus(double mean, double sigma);
};

#endif
