/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef trdHandler_H
#define trdHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class trdHandler:public XMLHandler {
public:
	trdHandler(std::string);
	void ElementHandle();

};

#endif
