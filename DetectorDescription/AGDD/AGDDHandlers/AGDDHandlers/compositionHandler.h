/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef compositionHandler_H
#define compositionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class compositionHandler:public XMLHandler {
public:
	compositionHandler(std::string);
	void ElementHandle();

};

#endif
