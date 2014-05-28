/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/sTGCHandler.h"
#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDsTGC.h"
#include <iostream>


sTGCHandler::sTGCHandler(std::string s):XMLHandler(s)
{
}

void sTGCHandler::ElementHandle()
{
    
//    std::cout<<" this is sTGCHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString("type",ret);
	std::string sType=getAttributeAsString("subType",ret);
	std::vector<double> vvv;
	vvv.push_back(getAttributeAsDouble("sWidth",ret));
	vvv.push_back(getAttributeAsDouble("lWidth",ret));
	vvv.push_back(getAttributeAsDouble("Length",ret));
	vvv.push_back(getAttributeAsDouble("Tck",ret));
	vvv.push_back(getAttributeAsDouble("yCutout",0.));
	
	std::string technology=getAttributeAsString("tech",ret);
	
	AGDDsTGC *b=new AGDDsTGC(name);
	b->SetXYZ(vvv);
	b->subType(sType);
	b->tech=technology;
	
}
