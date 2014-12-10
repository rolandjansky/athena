/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IbeamHandler_H
#define IbeamHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class IbeamHandler:public XMLHandler {
public:
	IbeamHandler(std::string);
	void ElementHandle();

};

#endif
