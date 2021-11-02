/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/consHandler.h"
#include "AGDDModel/AGDDCons.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


consHandler::consHandler(const std::string& s,
                         AGDDController& c)
  : XMLHandler(s, c)
{
}

void consHandler::ElementHandle(AGDDController& c,
                                xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name");
	std::string material=getAttributeAsString(c, t, "material");
	std::vector<double> vvv=getAttributeAsVector(c, t, "Rio1_Rio2_Z");
	
	AGDDCons *b=new AGDDCons(name, c.GetVolumeStore(), c.GetSectionStore());
	b->SetMaterial(material);
	b->SetRio_Z(vvv);
	
	vvv.clear();
	vvv=getAttributeAsVector(c, t, "profile",res);
	if (res)
	{
		b->SetProfile(vvv[0],vvv[1]);
	}
	else
		b->SetProfile(0.,360.);

	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		b->SetColor(col);
}
