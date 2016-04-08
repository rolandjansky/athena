/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxy_pointHandler.h"
#include "AGDDKernel/TwoPoint.h"
#include <iostream>

TwoPoint gvxy_pointHandler::s_point(0.,0.);


gvxy_pointHandler::gvxy_pointHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for gvxy_point"<<std::endl;
}

void gvxy_pointHandler::ElementHandle()
{
	std::vector<double> vvv=getAttributeAsVector("X_Y");
	s_point.x(vvv[0]);
	s_point.y(vvv[1]);
}
