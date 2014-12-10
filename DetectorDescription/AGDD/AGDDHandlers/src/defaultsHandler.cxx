/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/defaultsHandler.h"

#include <iostream>

defaultsHandler::defaultsHandler(std::string s):XMLHandler(s)
{
}

void defaultsHandler::ElementHandle()
{
	bool res;
//	std::cout<<"handling for defaults";
	std::string uLength=getAttributeAsString("unit_length",res);
}
