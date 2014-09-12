/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/arrayHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>


arrayHandler::arrayHandler(std::string s):XMLHandler(s)
{
}

void arrayHandler::ElementHandle()
{
	std::string name=getAttributeAsString("name");
	std::vector<double> value=getAttributeAsVector("values");
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<" array name "<<name<<" value="<<value[0]<<endreq;
	XercesParser::Evaluator().RegisterArray(name,value);
}
