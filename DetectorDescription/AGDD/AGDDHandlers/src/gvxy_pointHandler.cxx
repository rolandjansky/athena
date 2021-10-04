/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/gvxy_pointHandler.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDKernel/TwoPoint.h"
#include <iostream>

TwoPoint gvxy_pointHandler::s_point(0.,0.);


gvxy_pointHandler::gvxy_pointHandler(const std::string& s,
                                     AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for gvxy_point"<<std::endl;
}

void gvxy_pointHandler::ElementHandle(AGDDController& c,
                                      xercesc::DOMNode *t)
{
	std::vector<double> vvv=getAttributeAsVector(c, t, "X_Y");
	s_point.x(vvv[0]);
	s_point.y(vvv[1]);
}
