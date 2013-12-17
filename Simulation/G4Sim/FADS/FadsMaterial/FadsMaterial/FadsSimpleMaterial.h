/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsSimpleMaterial_H
#define FadsSimpleMaterial_H

#include <string>
#include "G4Material.hh"
#include "FadsMaterial/MaterialManager.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

enum State {NotDefined=kStateUndefined,Gas=kStateGas,Liquid=kStateLiquid,
	    Solid=kStateSolid};

class FadsSimpleMaterial {
protected:
	static FadsSimpleMaterial* currentMaterial;
	std::string name_;
	double density_;
	G4Material * theMaterial;
	State state;
	double temperature_;
	double pressure_;
	virtual void CreateG4Material() {}
	MaterialManager* matManager;
public:
	FadsSimpleMaterial(std::string n,double d,State sta=NotDefined,
				double temp=CLHEP::STP_Temperature,
				double pressure=CLHEP::STP_Pressure);
	static FadsSimpleMaterial* GetCurrentMaterial();
	virtual ~FadsSimpleMaterial();
	G4Material* GetG4Material();
	std::string name();
	double density();
};

}	// end namespace

#endif 	// FadsSimpleMaterial_H
