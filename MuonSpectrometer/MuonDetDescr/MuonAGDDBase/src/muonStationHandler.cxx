/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/muonStationHandler.h"
#include "MuonAGDDBase/AGDDMuonStation.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


muonStationHandler::muonStationHandler(const std::string& s,
                                       AGDDController& c)
  : XMLHandler(s, c)
{
//	std::cout<<"Creating handler for trd"<<std::endl;
}

void muonStationHandler::ElementHandle(AGDDController& c,
                                       xercesc::DOMNode *t)
{
    
//    std::cout<<" this is muonStationHandler::Handle"<<std::endl;
	bool res;
	std::string name=getAttributeAsString(c, t, "type",res);
	std::vector<double> vvv=getAttributeAsVector(c, t, "sWidth_lWidth_Tck_Length",res);
	AGDDMuonStation *b=new AGDDMuonStation(name, c.GetVolumeStore(), c.GetSectionStore());
	b->SetXYZ(vvv);
	
	std::string col=getAttributeAsString(c, t, "color",res);
	if (res)
		b->SetColor(col);
}
