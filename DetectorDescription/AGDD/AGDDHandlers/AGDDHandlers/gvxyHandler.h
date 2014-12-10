/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef gvxyHandler_H
#define gvxyHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class gvxyHandler:public XMLHandler {
public:
	gvxyHandler(std::string);
	void ElementHandle();

};

#endif
