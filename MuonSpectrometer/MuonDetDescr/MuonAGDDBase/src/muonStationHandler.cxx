/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/muonStationHandler.h"
#include "MuonAGDDBase/AGDDMuonStation.h"
#include <iostream>


muonStationHandler::muonStationHandler(std::string s):XMLHandler(s)
{
//	std::cout<<"Creating handler for trd"<<std::endl;
}

void muonStationHandler::ElementHandle()
{
    
//    std::cout<<" this is muonStationHandler::Handle"<<std::endl;
	bool res;
	std::string name=getAttributeAsString("type",res);
	std::vector<double> vvv=getAttributeAsVector("sWidth_lWidth_Tck_Length",res);
	AGDDMuonStation *b=new AGDDMuonStation(name);
	b->SetXYZ(vvv);
	
	std::string col=getAttributeAsString("color",res);
	if (res)
		b->SetColor(col);
}
