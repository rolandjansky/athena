/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef materialsHandler_H
#define materialsHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class materialsHandler:public XMLHandler {
public:
	materialsHandler(std::string);
	void ElementHandle();

};

#endif
