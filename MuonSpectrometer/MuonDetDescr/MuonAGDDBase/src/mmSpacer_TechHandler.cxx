/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/mmSpacer_TechHandler.h"
#include "MuonGeoModel/MMSpacer_Technology.h"
#include <iostream>


mmSpacer_TechHandler::mmSpacer_TechHandler(std::string s):XMLHandler(s)
{
}

void mmSpacer_TechHandler::ElementHandle()
{
    
//    std::cout<<" this is mmSpacer_TechHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString("type",ret);
	
	MuonGM::MMSpacer_Technology *tech=new MuonGM::MMSpacer_Technology(name);
	
	tech->thickness=getAttributeAsDouble("Tck",ret);
	
	tech->lowZCutOuts=getAttributeAsInt("lowZCutOuts",ret);
	if (ret) 
	{	
		tech->lowZCutOutWidth=getAttributeAsDouble("lowZCutOutWidth",ret);
		tech->lowZCutOutDZ=getAttributeAsDouble("lowZCutOutDZ",ret);
	}
	tech->highZCutOuts=getAttributeAsInt("highZCutOuts",ret);
	if (ret) 
	{	
		tech->highZCutOutWidth=getAttributeAsDouble("highZCutOutWidth",ret);	
		tech->highZCutOutDZ=getAttributeAsDouble("highZCutOutDZ",ret);
	}

}
