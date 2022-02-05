/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/mm_TechHandler.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDControl/AGDDController.h"
#include "MuonAGDDDescription/MM_Technology.h"
#include <iostream>



mm_TechHandler::mm_TechHandler(const std::string& s,
                               AGDDController& c)
  : XMLHandler(s, c)
{
}

void mm_TechHandler::ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t)
{
    
//    std::cout<<" this is mm_TechHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString(c, t, "type",ret);
	
	MuonGM::MM_Technology *tech=new MuonGM::MM_Technology(name, c.GetDetectorStore());
	
	tech->nlayers=getAttributeAsInt(c, t, "nLayers",ret);
	tech->thickness=getAttributeAsDouble(c, t, "Tck",ret);
	tech->gasThickness=getAttributeAsDouble(c, t, "gasTck",ret);
	tech->pcbThickness=getAttributeAsDouble(c, t, "driftPcbTck",ret);
	tech->roThickness=getAttributeAsDouble(c, t, "ROPcbTck",ret);
        
	tech->f1Thickness=getAttributeAsDouble(c, t, "f1",ret);
        tech->f2Thickness=getAttributeAsDouble(c, t, "f2",ret);
	tech->f3Thickness=getAttributeAsDouble(c, t, "f3",ret);

       
	
	tech->geoLevel=getAttributeAsInt(c, t, "geometryLevel",ret);

        AGDDParameterStore& prs = c.GetParameterStore();
	if(prs.Exist(name)) {
		std::cout << " parameters for technology " << name << " already registered" << std::endl;
	}
	else {
		AGDDParameterBagMMTech* paraBag = new AGDDParameterBagMMTech();
		paraBag->thickness = tech->gasThickness=getAttributeAsDouble(c, t, "gasTck",ret);
		paraBag->pitchSS = 0.425; //small sector
		paraBag->pitchLS = 0.445; //large sector
		prs.RegisterParameterBag(name, paraBag);
	}
}
