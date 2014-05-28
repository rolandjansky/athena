/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/trdHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDTrd.h"
#include <iostream>


trdHandler::trdHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for trd"<<std::endl;
}

void trdHandler::ElementHandle()
{
	bool res;
	std::string name=getAttributeAsString("name",res);
	std::string material=getAttributeAsString("material",res);
	std::vector<double> vvv=getAttributeAsVector("Xmp_Ymp_Z",res);
	if (msgLog().level()<=MSG::DEBUG)
	msgLog()<<MSG::DEBUG<<"trd name "<<name<<" material="<<material<<" Xmp_Ymp_Z"<<endreq;
	AGDDTrd *b=new AGDDTrd(name);
	b->SetMaterial(material);
	b->SetXYZ(vvv);
	
	std::string col=getAttributeAsString("color",res);
	if (res)
		b->SetColor(col);
}
