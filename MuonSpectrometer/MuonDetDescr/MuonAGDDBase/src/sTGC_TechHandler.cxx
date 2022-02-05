/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/sTGC_TechHandler.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDControl/AGDDController.h"
#include "MuonAGDDDescription/sTGC_Technology.h"
#include <iostream>


sTGC_TechHandler::sTGC_TechHandler(const std::string& s,
                                   AGDDController& c)
  : XMLHandler(s, c)
{
}

void sTGC_TechHandler::ElementHandle(AGDDController& c,
                                     xercesc::DOMNode *t)
{
    
//    std::cout<<" this is sTGC_TechHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString(c, t, "type",ret);
	
	MuonGM::sTGC_Technology *tech=new MuonGM::sTGC_Technology(name, c.GetDetectorStore());
	
	tech->nlayers=getAttributeAsInt(c, t, "nLayers",ret);
	tech->thickness=getAttributeAsDouble(c, t, "Tck",ret);
	tech->gasThickness=getAttributeAsDouble(c, t, "gasTck",ret);
    tech->pcbThickness=getAttributeAsDouble(c, t, "pcbTck",ret);

    tech->f4Thickness=getAttributeAsDouble(c, t, "f4",ret);
    tech->f5Thickness=getAttributeAsDouble(c, t, "f5",ret);
    tech->f6Thickness=getAttributeAsDouble(c, t, "f6",ret);
		
	// std::cout<<" sTGC_Tech "<<name<<tech->nlayers<<" "<<tech->thickness<<" "<<tech->gasThickness<<" "<<tech->pcbThickness<<std::endl;

	
	tech->geoLevel=getAttributeAsInt(c, t, "geometryLevel",ret);
		
        AGDDParameterStore& prs = c.GetParameterStore();
	if(prs.Exist(name)) {
		std::cout << " parameters for technology " << name << " already registered" << std::endl;
	}
	else {
		AGDDParameterBagsTGCTech* paraBag = new AGDDParameterBagsTGCTech();
		paraBag->thickness = tech->gasThickness=getAttributeAsDouble(c, t, "gasTck",ret);
		paraBag->stripPitch = 3.2; //eta
		paraBag->wirePitch = 1.8; //phi
		paraBag->stripWidth = 2.7; //eta
		paraBag->wireWidth = 0.015; //phi
		prs.RegisterParameterBag(name, paraBag);
	}
}
