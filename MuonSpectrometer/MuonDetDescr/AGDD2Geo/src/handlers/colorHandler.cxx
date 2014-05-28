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
	bool res;
	std::string name=getAttributeAsString("name",res);

	double fred=getAttributeAsDouble("red",res);
	double fgreen=getAttributeAsDouble("green",res);
	double fblue=getAttributeAsDouble("blue",res);
	double falpha __attribute__((__unused__));
	falpha=getAttributeAsDouble("alpha",res);
	AGDDColor *col __attribute__((__unused__));
	col=new AGDDColor(name,fred,fgreen,fblue);
}
