/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/elementHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDElement.h"
#include "AGDD2Geo/AGDDMaterial.h"
#include <iostream>


elementHandler::elementHandler(std::string s):XMLHandler(s)
{
}

void elementHandler::ElementHandle()
{
	std::string name=getAttributeAsString("name");
	std::string symbol=getAttributeAsString("symbol");
	double ca=getAttributeAsInt("aweight");
	int cz=getAttributeAsInt("z");
	double density=getAttributeAsDouble("density");
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"element name "<<name<<" symbol "<<symbol<<" a "<<ca<<" z "<<cz<<endreq;
	AGDDElement *el=new AGDDElement(name,symbol,cz,ca);
	AGDDMaterial *mat __attribute__((__unused__));
	mat=new AGDDMaterial(name,el,density);
}
