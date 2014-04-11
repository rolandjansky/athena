/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Calorimeter_H
#define Calorimeter_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>

class Calorimeter: public FADS::DetectorFacility {
public:
	Calorimeter(G4String s): DetectorFacility(s)
	{
	}
	void BuildGeometry();
};
#endif
