/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef elcylHandler_H
#define elcylHandler_H

#include "AGDD2Geo/XMLHandler.h"
#include <string>

class elcylHandler:public XMLHandler {
public:
	elcylHandler(std::string);
	void ElementHandle();

};

#endif
