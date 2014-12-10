/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/colorHandler.h"
#include "AGDDModel/AGDDColor.h"
#include <iostream>


colorHandler::colorHandler(std::string s):XMLHandler(s),color(0)
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
	color=new AGDDColor(name,fred,fgreen,fblue);
}
