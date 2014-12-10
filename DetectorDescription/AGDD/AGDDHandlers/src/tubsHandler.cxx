/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/tubsHandler.h"
#include "AGDDModel/AGDDTubs.h"
#include <iostream>



tubsHandler::tubsHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for tubs"<<std::endl;
}

void tubsHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string material=getAttributeAsString("material",res);
	std::vector<double> vvv=getAttributeAsVector("Rio_Z",res);
	AGDDTubs *v=new AGDDTubs(name);
	v->SetMaterial(material);
	v->SetRio_Z(vvv);
	
	vvv=getAttributeAsVector("profile",res);
	if (res)
		v->SetProfile(vvv[0],vvv[1]);
	else
		v->SetProfile(0.,360.);
		
	std::string col=getAttributeAsString("color",res);

	if (res)
		v->SetColor(col);
}
