/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef gvxysxHandler_H
#define gvxysxHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class gvxysxHandler:public XMLHandler {
public:
	gvxysxHandler(std::string);
	void ElementHandle();

};

#endif
