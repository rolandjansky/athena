/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDComposition_H
#define AGDDComposition_H

#include "AGDD2Geo/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDComposition: public AGDDVolume {
public:
	AGDDComposition(std::string s):AGDDVolume(s) {}
	void CreateVolume();
private:
};

#endif
