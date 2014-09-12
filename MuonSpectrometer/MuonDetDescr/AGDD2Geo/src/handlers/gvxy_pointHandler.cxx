/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/gvxy_pointHandler.h"
#include "AGDD2Geo/TwoPoint.h"
#include "AGDD2Geo/XercesParser.h"
#include <iostream>

TwoPoint gvxy_pointHandler::point(0.,0.);


gvxy_pointHandler::gvxy_pointHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for gvxy_point"<<std::endl;
}

void gvxy_pointHandler::ElementHandle()
{
	std::vector<double> vvv=getAttributeAsVector("X_Y");
	point.x(vvv[0]);
	point.y(vvv[1]);
}
