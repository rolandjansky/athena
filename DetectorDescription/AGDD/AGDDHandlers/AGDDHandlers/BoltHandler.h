/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BoltHandler_H
#define BoltHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class BoltHandler:public XMLHandler {
public:
	BoltHandler(std::string);
	void ElementHandle();

};

#endif
