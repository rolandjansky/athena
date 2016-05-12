/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsMolecule_H
#define FadsMolecule_H

#include <vector>
#include "FadsMaterial/FadsSimpleMaterial.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

struct Element 
{
	std::string m_name;
	int m_natoms;
};

class FadsMolecule: public FadsSimpleMaterial {
protected:
	int m_ncomponents;
	int m_nadded;
	std::vector<Element> m_elements;
public:
	FadsMolecule(std::string n,double density,int nc=0,
		    State sta=NotDefined,double temp=CLHEP::STP_Temperature,
		    double pressure=CLHEP::STP_Pressure);
	void CreateG4Material();
	void AddElement(std::string nam, int natoms);
};

}	// end namespace

#endif
