/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/MuonCosmicSetup.h"

bool chambersCosmicSetup(const std::string& f_hardware_name, int f_cosmicstation)
{
  bool pass=false;
  	

  ///Sector 13 cosmic setup///
  if (f_cosmicstation == 13) {
    //vector holding sector 13 cosmic setup chamber list
    std::vector<std::string> sector13;
    //Filling chambers list, user could change that accordingly
    sector13.push_back("BIL1A13");
    sector13.push_back("BIL2A13");
    sector13.push_back("BIL3A13");
    sector13.push_back("BML1A13");
    sector13.push_back("BML2A13");
    sector13.push_back("BML3A13");
    sector13.push_back("BOL1A13");
    sector13.push_back("BOL2A13");
    sector13.push_back("BOL3A13");  
    sector13.push_back("BOF1A12");
    sector13.push_back("BOF1A14");    
    sector13.push_back("BOF3A12");
    sector13.push_back("BOF3A14");  
    for (std::vector<std::string>::const_iterator iter=sector13.begin(); iter!=sector13.end(); iter++){
      if (f_hardware_name==*iter){pass=true;}     
    }
  }

  if (f_cosmicstation==0){pass=true;}
  	
  return pass;
}
