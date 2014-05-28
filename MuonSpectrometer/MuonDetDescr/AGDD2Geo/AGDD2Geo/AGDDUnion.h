/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDUnion_H
#define AGDDUnion_H

#include "AGDD2Geo/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDUnion: public AGDDVolume {
public:
	AGDDUnion(std::string s):AGDDVolume(s) {}
	void CreateVolume();
	void CreateSolid();
private:
};

#endif
