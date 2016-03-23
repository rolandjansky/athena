/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCoudes_H
#define LArCoudes_H

#include "LArG4Barrel/PhysicalVolumeAccessor.h"

class LArCoudes {
private:
	static PhysicalVolumeAccessor* s_theCoudes;
public:
	LArCoudes(std::string strDetector="") ;
	double XCentCoude(int stackid, int cellid) ;
	double YCentCoude(int stackid, int cellid) ;
};

#endif
