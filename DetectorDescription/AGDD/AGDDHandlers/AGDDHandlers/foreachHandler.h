/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef foreachHandler_H
#define foreachHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class foreachHandler:public XMLHandler {
public:
	foreachHandler(std::string);
	void ElementHandle();

};

#endif
