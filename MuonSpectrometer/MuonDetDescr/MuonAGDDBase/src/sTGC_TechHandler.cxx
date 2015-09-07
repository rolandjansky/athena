/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/sTGC_TechHandler.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "MuonAGDDDescription/sTGC_Technology.h"
#include <iostream>


sTGC_TechHandler::sTGC_TechHandler(std::string s):XMLHandler(s)
{
}

void sTGC_TechHandler::ElementHandle()
{
    
//    std::cout<<" this is sTGC_TechHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString("type",ret);
	
	MuonGM::sTGC_Technology *tech=new MuonGM::sTGC_Technology(name);
	
	tech->nlayers=getAttributeAsInt("nLayers",ret);
	tech->thickness=getAttributeAsDouble("Tck",ret);
	tech->gasThickness=getAttributeAsDouble("gasTck",ret);
    tech->pcbThickness=getAttributeAsDouble("pcbTck",ret);

    tech->f4Thickness=getAttributeAsDouble("f4",ret);
    tech->f5Thickness=getAttributeAsDouble("f5",ret);
    tech->f6Thickness=getAttributeAsDouble("f6",ret);
		
	// std::cout<<" sTGC_Tech "<<name<<tech->nlayers<<" "<<tech->thickness<<" "<<tech->gasThickness<<" "<<tech->pcbThickness<<std::endl;

	
	tech->geoLevel=getAttributeAsInt("geometryLevel",ret);
		
	if(AGDDParameterStore::GetParameterStore()->Exist(name)) {
		std::cout << " parameters for technology " << name << " already registered" << std::endl;
	}
	else {
		AGDDParameterBagsTGCTech* paraBag = new AGDDParameterBagsTGCTech();
		paraBag->thickness = tech->gasThickness=getAttributeAsDouble("gasTck",ret);
		paraBag->stripPitch = 3.2; //eta
		paraBag->wirePitch = 1.8; //phi
		paraBag->stripWidth = 2.7; //eta
		paraBag->wireWidth = 0.015; //phi
		AGDDParameterStore::GetParameterStore()->RegisterParameterBag(name, paraBag);
	}
}
