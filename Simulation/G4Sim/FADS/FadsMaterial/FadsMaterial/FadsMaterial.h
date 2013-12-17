/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsMaterial_H
#define FadsMaterial_H

#include "FadsMaterial/FadsSimpleMaterial.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

class FadsMaterial: public FadsSimpleMaterial {
protected:
	double z_;
	double a_;
public:
	FadsMaterial(std::string n,double z,double a,double density,
		    State sta=NotDefined,double temp=CLHEP::STP_Temperature,
		    double pressure=CLHEP::STP_Pressure);
	void CreateG4Material();
};

}	// end namespace

#endif
