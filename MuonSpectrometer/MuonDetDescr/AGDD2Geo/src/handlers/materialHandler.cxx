/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/materialHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDMaterial.h"
#include <iostream>


materialHandler::materialHandler(std::string s):XMLHandler(s)
{
}

void materialHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	double a=getAttributeAsDouble("a",res);
	int z=getAttributeAsInt("z",res);
	double density=getAttributeAsDouble("density",res);
	AGDDMaterial *mat __attribute__((__unused__));
	mat=new AGDDMaterial(name,z,a,density);	
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"material name "<<name<<" a "<<a<<" Z "<<z<<" density "<<density<<endreq;
}
