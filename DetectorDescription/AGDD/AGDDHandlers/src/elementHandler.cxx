/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/elementHandler.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDMaterial.h"
#include <iostream>


elementHandler::elementHandler(std::string s):XMLHandler(s),mat(0)
{
}

void elementHandler::ElementHandle()
{
	std::string name=getAttributeAsString("name");
	std::string symbol=getAttributeAsString("symbol");
	double ca=getAttributeAsInt("aweight");
	int cz=getAttributeAsInt("z");
	double density=getAttributeAsDouble("density");
	AGDDElement *el=new AGDDElement(name,symbol,cz,ca);
	mat=new AGDDMaterial(name,el,density);
}
