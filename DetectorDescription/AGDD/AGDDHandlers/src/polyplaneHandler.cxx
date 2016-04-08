/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/polyplaneHandler.h"
#include "AGDDHandlers/Polyplane.h"
#include <iostream>

Polyplane polyplaneHandler::s_pPlane(0.,0.,0.);


polyplaneHandler::polyplaneHandler(std::string s):XMLHandler(s)
{
}

void polyplaneHandler::ElementHandle()
{
	bool res;
	std::vector<double> vvv=getAttributeAsVector("Rio_Z",res);
	s_pPlane.rin(vvv[0]);
	s_pPlane.rou(vvv[1]);
	s_pPlane.z(vvv[2]);
}
