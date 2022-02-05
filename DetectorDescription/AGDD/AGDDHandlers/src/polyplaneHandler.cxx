/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/polyplaneHandler.h"
#include "AGDDHandlers/Polyplane.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


polyplaneHandler::polyplaneHandler(const std::string& s,
                                   AGDDController& c)
  : XMLHandler(s, c),
    m_pPlane (0, 0, 0)
{
}

void polyplaneHandler::ElementHandle(AGDDController& c,
                                     xercesc::DOMNode *t)
{
	bool res;
	std::vector<double> vvv=getAttributeAsVector(c, t, "Rio_Z",res);
	m_pPlane.rin(vvv[0]);
	m_pPlane.rou(vvv[1]);
	m_pPlane.z(vvv[2]);
}
