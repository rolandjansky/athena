/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/materialHandler.h"
#include "AGDDModel/AGDDMaterial.h"
#include <iostream>


materialHandler::materialHandler(std::string s):XMLHandler(s),mat(0)
{
}

void materialHandler::ElementHandle()
{
	std::string name=getAttributeAsString("name");
	double a=getAttributeAsDouble("a");
	int z=getAttributeAsInt("z");
	double density=getAttributeAsDouble("density");
	mat=new AGDDMaterial(name,z,a,density);	
}
