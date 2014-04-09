/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Atlas_H
#define Atlas_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>

class Atlas: public FADS::DetectorFacility {
public:
	Atlas(G4String s): DetectorFacility(s)
	{
	}
	void BuildGeometry();
};
#endif // Atlas_H
