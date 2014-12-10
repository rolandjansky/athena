/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ringHandler_H
#define ringHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class ringHandler:public XMLHandler {
public:
	ringHandler(std::string);
	void ElementHandle();

};

#endif
