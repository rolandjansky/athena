/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef aliasHandler_H
#define aliasHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class aliasHandler:public XMLHandler {
public:
	aliasHandler(std::string);
	void ElementHandle();

};

#endif
