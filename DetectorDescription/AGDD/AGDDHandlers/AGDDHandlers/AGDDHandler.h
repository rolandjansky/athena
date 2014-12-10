/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDHandler_H
#define AGDDHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDHandler:public XMLHandler {
public:
	AGDDHandler(std::string);
	void ElementHandle();

};

#endif
