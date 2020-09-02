/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef snake_pointHandler_H
#define snake_pointHandler_H

#include "AGDDControl/XMLHandler.h"

#include "GeoModelKernel/GeoDefinitions.h"

#include <string>

class snake_pointHandler:public XMLHandler {
public:
	snake_pointHandler(std::string);
	void ElementHandle();
	static GeoTrf::Vector3D CurrentPoint() {return s_point;}
private:
	static GeoTrf::Vector3D s_point;
};

#endif
