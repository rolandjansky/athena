/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef snake_pointHandler_H
#define snake_pointHandler_H

#include "AGDDControl/XMLHandler.h"

#include "CLHEP/Vector/ThreeVector.h"

#include <string>

class snake_pointHandler:public XMLHandler {
public:
	snake_pointHandler(std::string);
	void ElementHandle();
	static CLHEP::Hep3Vector CurrentPoint() {return s_point;}
private:
	static CLHEP::Hep3Vector s_point;
};

#endif
