/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef micromegasHandler_H
#define micromegasHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class micromegasHandler:public XMLHandler {
public:
	micromegasHandler(std::string);
	void ElementHandle();

};

#endif
