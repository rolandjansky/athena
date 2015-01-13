/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mmSpacerHandler_H
#define mmSpacerHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mmSpacerHandler:public XMLHandler {
public:
	mmSpacerHandler(std::string);
	void ElementHandle();

};

#endif
