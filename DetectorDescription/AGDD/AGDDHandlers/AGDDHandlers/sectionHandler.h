/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sectionHandler_H
#define sectionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDSection;

class sectionHandler:public XMLHandler {
public:
	sectionHandler(std::string);
	void ElementHandle();

	AGDDSection *sect;
};

#endif
