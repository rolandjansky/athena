/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mm_readoutHandler_H
#define mm_readoutHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mm_readoutHandler:public XMLHandler {
public:
	mm_readoutHandler(std::string);
	void ElementHandle();

};

#endif
