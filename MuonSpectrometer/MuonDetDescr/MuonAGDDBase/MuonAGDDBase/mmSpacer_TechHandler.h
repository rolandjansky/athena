/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mmSpacer_TechHandler_H
#define mmSpacer_TechHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mmSpacer_TechHandler:public XMLHandler {
public:
	mmSpacer_TechHandler(std::string);
	void ElementHandle();

};

#endif
