/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSubtraction_H
#define AGDDSubtraction_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDSubtraction: public AGDDVolume {
public:
	AGDDSubtraction(std::string s):AGDDVolume(s) {}
	void CreateVolume();
	void CreateSolid();
private:
};

#endif
