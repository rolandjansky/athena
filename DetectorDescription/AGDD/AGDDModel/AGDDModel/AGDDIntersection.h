/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDIntersection_H
#define AGDDIntersection_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDIntersection: public AGDDVolume {
public:
	AGDDIntersection(std::string s):AGDDVolume(s) {}
	void CreateVolume();
	void CreateSolid();
private:
};

#endif
