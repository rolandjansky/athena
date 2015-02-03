/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGC_readoutHandler_H
#define sTGC_readoutHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class sTGC_readoutHandler:public XMLHandler {
public:
	sTGC_readoutHandler(std::string);
	void ElementHandle();

};

#endif
