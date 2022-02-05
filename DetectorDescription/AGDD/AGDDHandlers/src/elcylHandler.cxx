/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/elcylHandler.h"
#include "AGDDModel/AGDDElcyl.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>



elcylHandler::elcylHandler(const std::string& s,
                           AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for tubs"<<std::endl;
}

void elcylHandler::ElementHandle(AGDDController& c,
                                 xercesc::DOMNode *t)
{
	bool res;
	std::string name=getAttributeAsString(c, t, "name");
	std::string material=getAttributeAsString(c, t, "material");
	std::vector<double> vvv=getAttributeAsVector(c, t, "RXio_RYio_Z");

	AGDDElcyl *v=new AGDDElcyl(name, c.GetVolumeStore(), c.GetSectionStore());
	v->SetMaterial(material);
	v->SetRXio_RYio_Z(vvv);
		
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		v->SetColor(col);
}
