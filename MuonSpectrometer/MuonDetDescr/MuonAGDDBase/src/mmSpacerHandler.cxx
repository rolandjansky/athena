/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/mmSpacerHandler.h"
#include "MuonAGDDBase/AGDDMMSpacer.h"
#include <iostream>


mmSpacerHandler::mmSpacerHandler(std::string s):XMLHandler(s)
{
}

void mmSpacerHandler::ElementHandle()
{
    
//    std::cout<<" this is mmSpacerHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString("type",ret);
	std::vector<double> vvv;
	vvv.push_back(getAttributeAsDouble("sWidth",ret));
	vvv.push_back(getAttributeAsDouble("lWidth",ret));
	vvv.push_back(getAttributeAsDouble("Length",ret));
	vvv.push_back(getAttributeAsDouble("Tck",ret));
	
	std::string technology=getAttributeAsString("tech",ret);
	
	AGDDMMSpacer *b=new AGDDMMSpacer(name);
	b->SetXYZ(vvv);
	b->tech=technology;
	
}
