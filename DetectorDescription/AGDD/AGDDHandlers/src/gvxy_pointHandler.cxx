/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxy_pointHandler.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDKernel/TwoPoint.h"
#include <iostream>


gvxy_pointHandler::gvxy_pointHandler(const std::string& s,
                                     AGDDController& c)
  : XMLHandler(s, c),
    m_point (0, 0)
{
//	std::cout<<"Creating handler for gvxy_point"<<std::endl;
}

void gvxy_pointHandler::ElementHandle(AGDDController& c,
                                      xercesc::DOMNode *t)
{
	std::vector<double> vvv=getAttributeAsVector(c, t, "X_Y");
	m_point.x(vvv[0]);
	m_point.y(vvv[1]);
}
