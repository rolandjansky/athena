/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMI_H
#define CMI_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {

class CMI: public Technology {
public:
	inline CMI(std::string s);
	double largeness;
	double height;
	double excent;
};

CMI::CMI(std::string s): Technology(s)
{
}
} // namespace MuonGM

#endif
