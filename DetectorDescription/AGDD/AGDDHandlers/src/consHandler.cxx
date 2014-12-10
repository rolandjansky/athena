/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/consHandler.h"
#include "AGDDModel/AGDDCons.h"
#include <iostream>


consHandler::consHandler(std::string s):XMLHandler(s)
{
}

void consHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name");
	std::string material=getAttributeAsString("material");
	std::vector<double> vvv=getAttributeAsVector("Rio1_Rio2_Z");
	
	AGDDCons *b=new AGDDCons(name);
	b->SetMaterial(material);
	b->SetRio_Z(vvv);
	
	vvv.clear();
	vvv=getAttributeAsVector("profile",res);
	if (res)
	{
		b->SetProfile(vvv[0],vvv[1]);
	}
	else
		b->SetProfile(0.,360.);

	std::string col=getAttributeAsString("color",res);
	if (res)
		b->SetColor(col);
}
