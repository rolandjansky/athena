/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/snake_pointHandler.h"

#include <iostream>

GeoTrf::Vector3D snake_pointHandler::s_point(GeoTrf::Vector3D::Identity());

snake_pointHandler::snake_pointHandler(const std::string& s,
                                       AGDDController& c)
  : XMLHandler(s, c)
{
}

void snake_pointHandler::ElementHandle(AGDDController& c,
                                       xercesc::DOMNode *t)
{
	bool res=false;
	std::vector<double> vvv=getAttributeAsVector(c, t, "X_Y_Z",res);
	s_point = GeoTrf::Vector3D(vvv[0], vvv[1], vvv[2]);
}
