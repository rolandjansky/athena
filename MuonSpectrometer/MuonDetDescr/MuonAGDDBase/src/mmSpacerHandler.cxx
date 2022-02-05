/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/mmSpacerHandler.h"
#include "MuonAGDDBase/AGDDMMSpacer.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


mmSpacerHandler::mmSpacerHandler(const std::string& s,
                                 AGDDController& c)
  : XMLHandler(s, c)
{
}

void mmSpacerHandler::ElementHandle(AGDDController& c,
                                    xercesc::DOMNode *t)
{
    
//    std::cout<<" this is mmSpacerHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString(c, t, "type",ret);
	std::vector<double> vvv;
	vvv.push_back(getAttributeAsDouble(c, t, "sWidth",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "lWidth",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "Length",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "Tck",ret));
	
	std::string technology=getAttributeAsString(c, t, "tech",ret);
	
	AGDDMMSpacer *b=new AGDDMMSpacer(name, c.GetVolumeStore(), c.GetSectionStore());
	b->SetXYZ(vvv);
	b->tech=technology;
	
}
