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
	static FadsSimpleMaterial* s_currentMaterial;
	std::string m_name;
	double m_density;
	G4Material * m_theMaterial;
	State m_state;
	double m_temperature;
	double m_pressure;
	virtual void CreateG4Material() {}
	MaterialManager* m_matManager;
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
