/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4EventAnalyzer_H
#define G4EventAnalyzer_H

#include <string>
#include <vector>
#include "FadsKinematics/GeneratorCenter.h"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

namespace FADS {

class G4EventAnalyzer {
protected:
	std::string name;
	int onSwitch;
public:
	G4EventAnalyzer(std::string s) 
	{
		name=s;
		onSwitch=0;  // off by default
		GeneratorCenter::GetGeneratorCenter()->AddEventAnalyzer(this);
	}
	virtual ~G4EventAnalyzer() {}
	std::string GetName() {return name;}
	bool IsOn() {return onSwitch;}
	void TurnOn() {onSwitch=1;}
	void TurnOff() {onSwitch=0;}
	virtual void EventInitialization() {;}
	virtual void EventAnalysis(std::vector<G4PrimaryVertex*>&, std::vector<G4PrimaryParticle*>&) {}
};

}	// end namespace

#endif
