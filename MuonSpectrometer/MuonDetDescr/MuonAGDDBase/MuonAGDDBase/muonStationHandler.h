/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef muonStationHandler_H
#define muonStationHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class muonStationHandler:public XMLHandler {
public:
	muonStationHandler(std::string);
	void ElementHandle();

};

#endif
