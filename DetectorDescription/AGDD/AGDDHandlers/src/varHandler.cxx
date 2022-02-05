/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/varHandler.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDControl/XMLHandler.h"
#include <iostream>


varHandler::varHandler(const std::string& s,
                       AGDDController& c)
  : XMLHandler(s, c)
{
}

void varHandler::ElementHandle(AGDDController& c,
                               xercesc::DOMNode *t)
{
	bool bres;
	std::string name=getAttributeAsString(c, t, "name",bres);
	double res=getAttributeAsDouble(c, t, "value",bres);
	c.Evaluator().RegisterConstant(name,res);
}
