/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef versionHandler_H
#define versionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class versionHandler:public XMLHandler {
public:
	versionHandler(std::string);
	void ElementHandle();

};

#endif
