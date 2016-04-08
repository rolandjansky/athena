/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/snake_pointHandler.h"

#include "CLHEP/Vector/ThreeVector.h"

#include <iostream>

CLHEP::Hep3Vector snake_pointHandler::s_point(0.,0.);

snake_pointHandler::snake_pointHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for snake_point"<<std::endl;
}

void snake_pointHandler::ElementHandle()
{
	bool res;
	std::vector<double> vvv=getAttributeAsVector("X_Y_Z",res);
	s_point.setX(vvv[0]);
	s_point.setY(vvv[1]);
	s_point.setZ(vvv[2]);
}
