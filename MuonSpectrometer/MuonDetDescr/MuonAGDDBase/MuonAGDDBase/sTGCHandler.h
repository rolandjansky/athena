/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCHandler_H
#define sTGCHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class sTGCHandler:public XMLHandler {
public:
	sTGCHandler(std::string);
	void ElementHandle();

};

#endif
