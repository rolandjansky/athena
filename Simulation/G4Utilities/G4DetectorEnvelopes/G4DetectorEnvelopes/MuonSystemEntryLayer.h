/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSystemEntryLayer_H
#define MuonSystemEntryLayer_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>

class MuonSystemEntryLayer: public FADS::DetectorFacility {
public:
	MuonSystemEntryLayer(G4String s): DetectorFacility(s)
	{
	}
	void BuildGeometry();
};
#endif
