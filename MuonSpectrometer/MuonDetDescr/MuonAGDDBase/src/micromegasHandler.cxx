/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/micromegasHandler.h"
#include "MuonAGDDBase/AGDDMicromegas.h"
#include <iostream>


micromegasHandler::micromegasHandler(std::string s):XMLHandler(s)
{
}

void micromegasHandler::ElementHandle()
{
    
//    std::cout<<" this is micromegasHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString("type",ret);
	std::string sType=getAttributeAsString("subType",ret);
	std::vector<double> vvv;
	vvv.push_back(getAttributeAsDouble("sWidth",ret));
	vvv.push_back(getAttributeAsDouble("lWidth",ret));
	vvv.push_back(getAttributeAsDouble("Length",ret));
	vvv.push_back(getAttributeAsDouble("Tck",ret));
	
	std::string technology=getAttributeAsString("tech",ret);
	
	AGDDMicromegas *b=new AGDDMicromegas(name);
	b->SetXYZ(vvv);
	b->subType(sType);
	b->tech=technology;

	double xf=getAttributeAsDouble("xFrame",0.);
	b->xFrame(xf);
	double ysf=getAttributeAsDouble("ysFrame",0.);
	b->ysFrame(ysf);
	double ylf=getAttributeAsDouble("ylFrame",0.);
	b->ylFrame(ylf);
	
//	std::string col=getAttributeAsString("color",ret);
//	if (ret)
//		b->SetColor(col);
}
