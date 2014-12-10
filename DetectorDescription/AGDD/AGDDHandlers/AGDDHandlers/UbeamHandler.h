/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UbeamHandler_H
#define UbeamHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class UbeamHandler:public XMLHandler {
public:
	UbeamHandler(std::string);
	void ElementHandle();

};

#endif
