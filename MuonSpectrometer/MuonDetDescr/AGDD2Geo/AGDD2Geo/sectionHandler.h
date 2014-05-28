/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sectionHandler_H
#define sectionHandler_H

#include "AGDD2Geo/XMLHandler.h"
#include <string>

class sectionHandler:public XMLHandler {
public:
	sectionHandler(std::string);
	void ElementHandle();

};

#endif
