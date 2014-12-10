/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/elcylHandler.h"
#include "AGDDModel/AGDDElcyl.h"
#include <iostream>



elcylHandler::elcylHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for tubs"<<std::endl;
}

void elcylHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name");
	std::string material=getAttributeAsString("material");
	std::vector<double> vvv=getAttributeAsVector("RXio_RYio_Z");

	AGDDElcyl *v=new AGDDElcyl(name);
	v->SetMaterial(material);
	v->SetRXio_RYio_Z(vvv);
		
	std::string col=getAttributeAsString("color",res);
	if (res)
		v->SetColor(col);
}
