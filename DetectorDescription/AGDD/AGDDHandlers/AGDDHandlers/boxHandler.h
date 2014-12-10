/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef boxHandler_H
#define boxHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class boxHandler:public XMLHandler {
public:
	boxHandler(std::string);
	void ElementHandle();

};

#endif
