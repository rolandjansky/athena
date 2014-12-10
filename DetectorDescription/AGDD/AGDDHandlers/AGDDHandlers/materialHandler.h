/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef materialHandler_H
#define materialHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDMaterial;
class materialHandler:public XMLHandler {
public:
	materialHandler(std::string);
	void ElementHandle();

	AGDDMaterial *mat;
};

#endif
