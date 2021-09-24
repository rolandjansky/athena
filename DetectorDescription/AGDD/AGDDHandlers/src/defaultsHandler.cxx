/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/defaultsHandler.h"

#include <iostream>

defaultsHandler::defaultsHandler(const std::string& s,
                                 AGDDController& c)
  : XMLHandler(s, c)
{
}

void defaultsHandler::ElementHandle(AGDDController& c,
                                    xercesc::DOMNode *t)
{
	bool res;
//	std::cout<<"handling for defaults";
	std::string uLength=getAttributeAsString(c, t, "unit_length",res);
}
