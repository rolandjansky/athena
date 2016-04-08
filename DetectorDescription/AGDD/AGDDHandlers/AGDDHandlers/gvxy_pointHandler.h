/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef gvxy_pointHandler_H
#define gvxy_pointHandler_H

#include "AGDDControl/XMLHandler.h"
#include "AGDDKernel/TwoPoint.h"
#include <string>

class gvxy_pointHandler:public XMLHandler {
public:
	gvxy_pointHandler(std::string);
	void ElementHandle();
	static TwoPoint CurrentTwoPoint() {return s_point;}
private:
	static TwoPoint s_point;
};

#endif
