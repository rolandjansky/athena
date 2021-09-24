/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/polyplaneHandler.h"
#include "AGDDHandlers/Polyplane.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>

Polyplane polyplaneHandler::s_pPlane(0.,0.,0.);


polyplaneHandler::polyplaneHandler(const std::string& s,
                                   AGDDController& c)
  : XMLHandler(s, c)
{
}

void polyplaneHandler::ElementHandle(AGDDController& c,
                                     xercesc::DOMNode *t)
{
	bool res;
	std::vector<double> vvv=getAttributeAsVector(c, t, "Rio_Z",res);
	s_pPlane.rin(vvv[0]);
	s_pPlane.rou(vvv[1]);
	s_pPlane.z(vvv[2]);
}
