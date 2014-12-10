/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef pgonHandler_H
#define pgonHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class pgonHandler:public XMLHandler {
public:
	pgonHandler(std::string);
	void ElementHandle();

};

#endif
