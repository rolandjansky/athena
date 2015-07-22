/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef importHandler_H
#define importHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class importHandler:public XMLHandler {
public:
	importHandler(std::string);
	void ElementHandle();

};

#endif
