/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/arrayHandler.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


arrayHandler::arrayHandler(const std::string& s,
                           AGDDController& c)
  : XMLHandler(s, c)
{
}

void arrayHandler::ElementHandle(AGDDController& c,
                                 xercesc::DOMNode *t)
{
	std::string name=getAttributeAsString(c, t, "name");
	std::vector<double> value=getAttributeAsVector(c, t, "values");
	c.Evaluator().RegisterArray(name,value);
}
