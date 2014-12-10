/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef fractionmassHandler_H
#define fractionmassHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class fractionmassHandler:public XMLHandler {
public:
	fractionmassHandler(std::string);
	void ElementHandle();

};

#endif
