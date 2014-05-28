/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mposPhiHandler_H
#define mposPhiHandler_H

#include "AGDD2Geo/XMLHandler.h"
#include <string>

class mposPhiHandler:public XMLHandler {
public:
	mposPhiHandler(std::string);
	void ElementHandle();

};

#endif
