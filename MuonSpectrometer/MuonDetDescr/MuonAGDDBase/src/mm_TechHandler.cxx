/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/mm_TechHandler.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "MuonAGDDDescription/MM_Technology.h"
#include <iostream>



mm_TechHandler::mm_TechHandler(std::string s):XMLHandler(s)
{
}

void mm_TechHandler::ElementHandle()
{
    
//    std::cout<<" this is mm_TechHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString("type",ret);
	
	MuonGM::MM_Technology *tech=new MuonGM::MM_Technology(name);
	
	tech->nlayers=getAttributeAsInt("nLayers",ret);
	tech->thickness=getAttributeAsDouble("Tck",ret);
	tech->gasThickness=getAttributeAsDouble("gasTck",ret);
	tech->pcbThickness=getAttributeAsDouble("driftPcbTck",ret);
	tech->roThickness=getAttributeAsDouble("ROPcbTck",ret);
        
	tech->f1Thickness=getAttributeAsDouble("f1",ret);
        tech->f2Thickness=getAttributeAsDouble("f2",ret);
	tech->f3Thickness=getAttributeAsDouble("f3",ret);

       
	
	tech->geoLevel=getAttributeAsInt("geometryLevel",ret);

	if(AGDDParameterStore::GetParameterStore()->Exist(name)) {
		std::cout << " parameters for technology " << name << " already registered" << std::endl;
	}
	else {
		AGDDParameterBagMMTech* paraBag = new AGDDParameterBagMMTech();
		paraBag->thickness = tech->gasThickness=getAttributeAsDouble("gasTck",ret);
		paraBag->pitchSS = 0.425; //small sector
		paraBag->pitchLS = 0.445; //large sector
		AGDDParameterStore::GetParameterStore()->RegisterParameterBag(name, paraBag);
	}
}
