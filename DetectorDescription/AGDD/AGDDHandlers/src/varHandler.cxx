/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/varHandler.h"
#include "AGDDControl/XMLHandler.h"
#include <iostream>


varHandler::varHandler(std::string s):XMLHandler(s)
{
}

void varHandler::ElementHandle()
{
	bool bres;
	std::string name=getAttributeAsString("name",bres);
	double res=getAttributeAsDouble("value",bres);
	Evaluator().RegisterConstant(name,res);
}
