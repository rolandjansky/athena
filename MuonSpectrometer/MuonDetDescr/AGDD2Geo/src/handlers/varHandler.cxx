/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/varHandler.h"
#include "AGDD2Geo/XMLHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>


varHandler::varHandler(std::string s):XMLHandler(s)
{
}

void varHandler::ElementHandle()
{
	bool bres;
	std::string name=getAttributeAsString("name",bres);
	double res=getAttributeAsDouble("value",bres);
	XercesParser::Evaluator().RegisterConstant(name,res);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<" var name "<<name<<" calculated value="<<res<<endreq;
}
