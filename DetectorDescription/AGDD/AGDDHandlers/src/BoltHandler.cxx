/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/BoltHandler.h"
#include "AGDDModel/AGDDBolt.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>

BoltHandler::BoltHandler(const std::string& s,
                         AGDDController& c)
  : XMLHandler(s, c)
{
}

void BoltHandler::ElementHandle(AGDDController& c,
                                xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name",res);
	std::string material=getAttributeAsString(c, t, "material",res);
	double l=getAttributeAsDouble(c, t, "length",res);
	double d=getAttributeAsDouble(c, t, "diameter",res);
	double hd=getAttributeAsDouble(c, t, "head_diameter",res);
	double hl=getAttributeAsDouble(c, t, "head_length",res);

	AGDDBolt *b=new AGDDBolt(name, c.GetVolumeStore(), c.GetSectionStore());
	b->SetMaterial(material);
	b->SetLength(l);
	b->SetHeadLength(hl);
	b->SetDiameter(d);
	b->SetHeadDiameter(hd);
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		b->SetColor(col);
}
