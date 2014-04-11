/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InnerDetector_H
#define InnerDetector_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>

class InnerDetector: public FADS::DetectorFacility {
public:
	InnerDetector(G4String s): DetectorFacility(s)
	{
	}
	void BuildGeometry();
};
#endif
