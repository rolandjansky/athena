/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/consHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDCons.h"
#include <iostream>


consHandler::consHandler(std::string s):XMLHandler(s)
{
}

void consHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string material=getAttributeAsString("material",res);
	std::vector<double> vvv=getAttributeAsVector("Rio1_Rio2_Z",res);
	
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
