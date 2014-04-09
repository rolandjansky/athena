/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Muon_H
#define Muon_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>

class Muon: public FADS::DetectorFacility {
public:
	Muon(G4String s): DetectorFacility(s)
	{
	}
	void BuildGeometry();
};
#endif
