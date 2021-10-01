/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/materialHandler.h"
#include "AGDDModel/AGDDMaterial.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


materialHandler::materialHandler(const std::string& s,
                                 AGDDController& c)
  : XMLHandler(s, c)
{
}

void materialHandler::ElementHandle(AGDDController& c,
                                    xercesc::DOMNode *t)
{
	std::string name=getAttributeAsString(c, t, "name");
	double a=getAttributeAsDouble(c, t, "a");
	int z=getAttributeAsInt(c, t, "z");
	double density=getAttributeAsDouble(c, t, "density");
	new AGDDMaterial(c.GetMaterialStore(),name,z,a,density);	
}
