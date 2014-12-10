/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/BoltHandler.h"
#include "AGDDModel/AGDDBolt.h"
#include <iostream>

BoltHandler::BoltHandler(std::string s):XMLHandler(s)
{
}

void BoltHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string material=getAttributeAsString("material",res);
	double l=getAttributeAsDouble("length",res);
	double d=getAttributeAsDouble("diameter",res);
	double hd=getAttributeAsDouble("head_diameter",res);
	double hl=getAttributeAsDouble("head_length",res);

	AGDDBolt *b=new AGDDBolt(name);
	b->SetMaterial(material);
	b->SetLength(l);
	b->SetHeadLength(hl);
	b->SetDiameter(d);
	b->SetHeadDiameter(hd);
	
	std::string col=getAttributeAsString("color",res);
	if (res)
		b->SetColor(col);
}
