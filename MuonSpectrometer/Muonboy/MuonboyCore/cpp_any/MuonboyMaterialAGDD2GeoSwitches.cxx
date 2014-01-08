/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyMaterialAGDD2GeoSwitches.h"

extern "C" void resetmatagdd2geoswitches_();
extern "C" void f1setmatagdd2geoswitches_( int& Long, char* NAMEVAR);

MuonboyMaterialAGDD2GeoSwitches::MuonboyMaterialAGDD2GeoSwitches(){}

MuonboyMaterialAGDD2GeoSwitches::~MuonboyMaterialAGDD2GeoSwitches(){}

void MuonboyMaterialAGDD2GeoSwitches::SetMatiereAGDD2GeoSwitches(std::vector<std::string> AGDD2GeoSwitches)
{

  std::cout << " MuonboyMaterialAGDD2GeoSwitches::SetMatiereAGDD2GeoSwitches 	" 
            << " One gets from AGDD2GeoSwitches " << AGDD2GeoSwitches.size() << " items "
            << std::endl;

  resetmatagdd2geoswitches_();
  for (unsigned int Item = 0; Item < AGDD2GeoSwitches.size(); Item++) {

    std::string TheString  = AGDD2GeoSwitches[Item];
    char NAMEVAR[100];
    int Long = TheString.size();
    if( Long > 100 ){ Long = 100; }
    for(int I=0; I<Long; I++){ NAMEVAR[I] = TheString[I]; }

    f1setmatagdd2geoswitches_( Long, NAMEVAR);

  }

}
