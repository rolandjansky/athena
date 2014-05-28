/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/tubsHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDTubs.h"
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
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"tubs name "<<name<<" material="<<material<<" Rio_Z "<<endreq;
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
