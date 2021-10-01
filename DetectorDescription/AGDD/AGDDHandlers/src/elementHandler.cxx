/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/elementHandler.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDMaterial.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


elementHandler::elementHandler(const std::string& s,
                               AGDDController& c)
  : XMLHandler(s, c)
{
}

void elementHandler::ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t)
{
	std::string name=getAttributeAsString(c, t, "name");
	std::string symbol=getAttributeAsString(c, t, "symbol");
	double ca=getAttributeAsInt(c, t, "aweight");
	int cz=getAttributeAsInt(c, t, "z");
	double density=getAttributeAsDouble(c, t, "density");
	AGDDElement *el=new AGDDElement(c.GetMaterialStore(),name,symbol,cz,ca);
	new AGDDMaterial(c.GetMaterialStore(),name,el,density);
}
