/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsMixture_H
#define FadsMixture_H

#include <vector>
#include "FadsMaterial/FadsSimpleMaterial.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

enum Type {Elem, Mat};

struct MaterialComponent 
{
	Type type;
	std::string name;
	double fraction;
};

class FadsMixture: public FadsSimpleMaterial {
protected:
	int m_ncomponents;
	int m_nadded;
	std::vector<MaterialComponent> m_components;
public:
	FadsMixture(std::string n,double density,int nc=0,
		    State sta=NotDefined,double temp=CLHEP::STP_Temperature,
		    double pressure=CLHEP::STP_Pressure);
	void CreateG4Material();
	void AddMaterial(std::string nam, double fract);
	void AddElement(std::string nam, double fract);
};

}	// end namespace

#endif
