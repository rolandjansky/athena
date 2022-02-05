/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/colorHandler.h"
#include "AGDDModel/AGDDColor.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


colorHandler::colorHandler(const std::string& s,
                           AGDDController& c)
  : XMLHandler(s, c)
{
}

void colorHandler::ElementHandle(AGDDController& c,
                                 xercesc::DOMNode *t)
{
	std::string name=getAttributeAsString(c, t, "name");

	double fred=getAttributeAsDouble(c, t, "red");
	double fgreen=getAttributeAsDouble(c, t, "green");
	double fblue=getAttributeAsDouble(c, t, "blue");
	double falpha __attribute__((__unused__));
	falpha=getAttributeAsDouble(c, t, "alpha",0.);
	new AGDDColor(c.GetColorStore(),name,fred,fgreen,fblue);
}
