/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/mmSpacer_TechHandler.h"
#include "MuonGeoModel/MMSpacer_Technology.h"
#include "MuonGeoModel/MYSQL.h"
#include <iostream>


mmSpacer_TechHandler::mmSpacer_TechHandler(const std::string& s,
                                           AGDDController& c)
  : XMLHandler(s, c)
{
}

void mmSpacer_TechHandler::ElementHandle(AGDDController& c,
                                         xercesc::DOMNode *t)
{
    
//    std::cout<<" this is mmSpacer_TechHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString(c, t, "type",ret);
	
	MuonGM::MMSpacer_Technology *tech=new MuonGM::MMSpacer_Technology(*MuonGM::MYSQL::GetPointer(), name);
	
	tech->thickness=getAttributeAsDouble(c, t, "Tck",ret);
	
	tech->lowZCutOuts=getAttributeAsInt(c, t, "lowZCutOuts",ret);
	if (ret) 
	{	
		tech->lowZCutOutWidth=getAttributeAsDouble(c, t, "lowZCutOutWidth",ret);
		tech->lowZCutOutDZ=getAttributeAsDouble(c, t, "lowZCutOutDZ",ret);
	}
	tech->highZCutOuts=getAttributeAsInt(c, t, "highZCutOuts",ret);
	if (ret) 
	{	
		tech->highZCutOutWidth=getAttributeAsDouble(c, t, "highZCutOutWidth",ret);	
		tech->highZCutOutDZ=getAttributeAsDouble(c, t, "highZCutOutDZ",ret);
	}

}
