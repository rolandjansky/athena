/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/colorHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDColor.h"
#include <iostream>


colorHandler::colorHandler(std::string s):XMLHandler(s)
{
}

void colorHandler::ElementHandle()
{
	std::string name=getAttributeAsString("name");

	double fred=getAttributeAsDouble("red");
	double fgreen=getAttributeAsDouble("green");
	double fblue=getAttributeAsDouble("blue");
	double falpha __attribute__((__unused__));
	falpha=getAttributeAsDouble("alpha",0.);
	AGDDColor *col __attribute__((__unused__));
	col=new AGDDColor(name,fred,fgreen,fblue);
}
