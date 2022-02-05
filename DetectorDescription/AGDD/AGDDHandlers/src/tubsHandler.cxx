/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/tubsHandler.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDModel/AGDDTubs.h"
#include <iostream>



tubsHandler::tubsHandler(const std::string& s,
                         AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for tubs"<<std::endl;
}

void tubsHandler::ElementHandle(AGDDController& c,
                                xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name",res);
	std::string material=getAttributeAsString(c, t, "material",res);
	std::vector<double> vvv=getAttributeAsVector(c, t, "Rio_Z",res);
	AGDDTubs *v=new AGDDTubs(name, c.GetVolumeStore(), c.GetSectionStore());
	v->SetMaterial(material);
	v->SetRio_Z(vvv);
	
	vvv=getAttributeAsVector(c, t, "profile",res);
	if (res)
		v->SetProfile(vvv[0],vvv[1]);
	else
		v->SetProfile(0.,360.);
		
	std::string col=getAttributeAsString(c, t, "color",res);

	if (res)
		v->SetColor(col);
}
