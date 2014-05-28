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
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string symbol=getAttributeAsString("symbol",res);
	double ca=getAttributeAsInt("aweight",res);
	int cz=getAttributeAsInt("z",res);
	double density=getAttributeAsDouble("density",res);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"element name "<<name<<" symbol "<<symbol<<" a "<<ca<<" z "<<cz<<endreq;
	AGDDElement *el=new AGDDElement(name,symbol,cz,ca);
	if (res)
	{
		AGDDMaterial *mat __attribute__((__unused__));
		mat=new AGDDMaterial(name,el,density);
	}
}
