/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/arrayHandler.h"
#include <iostream>


arrayHandler::arrayHandler(std::string s):XMLHandler(s)
{
}

void arrayHandler::ElementHandle()
{
	std::string name=getAttributeAsString("name");
	std::vector<double> value=getAttributeAsVector("values");
	Evaluator().RegisterArray(name,value);
}
