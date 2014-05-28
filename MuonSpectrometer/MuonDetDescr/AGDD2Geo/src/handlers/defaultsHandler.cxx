/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/defaultsHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include <iostream>

defaultsHandler::defaultsHandler(std::string s):XMLHandler(s)
{
}

void defaultsHandler::ElementHandle()
{
	bool res;
//	std::cout<<"handling for defaults";
	std::string uLength=getAttributeAsString("unit_length",res);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"defults unit_length "<<uLength<<endreq;
}
