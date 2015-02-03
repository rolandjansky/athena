/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mm_TechHandler_H
#define mm_TechHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class mm_TechHandler:public XMLHandler {
public:
	mm_TechHandler(std::string);
	void ElementHandle();

};

#endif
