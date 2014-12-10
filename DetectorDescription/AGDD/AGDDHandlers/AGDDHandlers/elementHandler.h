/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef elementHandler_H
#define elementHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDMaterial;

class elementHandler:public XMLHandler {
public:
	elementHandler(std::string);
	void ElementHandle();

	AGDDMaterial *mat;
};

#endif
