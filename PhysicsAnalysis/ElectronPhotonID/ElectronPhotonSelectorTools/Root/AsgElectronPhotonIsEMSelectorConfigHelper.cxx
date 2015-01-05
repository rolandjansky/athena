/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "TEnv.h"
#include <vector>

  std::vector<double> AsgConfigHelper::HelperDouble(const std::string& input,  TEnv& env){
    std::vector<double> CutVector;    
    std::string env_input(env.GetValue(input.c_str(), ""));
    if (env_input.size() > 0) {
      std::string::size_type end;
      do {
	end = env_input.find(";");
	CutVector.push_back( atof ( env_input.substr(0,end).c_str() ) ) ;
	if (end != std::string::npos) env_input= env_input.substr(end+1);
      } while (end != std::string::npos);     
    }
    return CutVector;
  }

  std::vector<float> AsgConfigHelper::HelperFloat(const std::string& input,  TEnv& env){
    std::vector<float> CutVector;    
    std::string env_input(env.GetValue(input.c_str(), ""));
    if (env_input.size() > 0) {
      std::string::size_type end;
      do {
	end = env_input.find(";");
	CutVector.push_back( atof ( env_input.substr(0,end).c_str() ) ) ;
	if (end != std::string::npos) env_input= env_input.substr(end+1);
      } while (end != std::string::npos);     
    }
    return CutVector;
  }

  std::vector<int> AsgConfigHelper::HelperInt(const std::string& input, TEnv& env){
    
    std::vector<int> CutVector;
    std::string env_input(env.GetValue(input.c_str(), ""));
    if (env_input.size() > 0) {
      std::string::size_type end;
      do {
	end = env_input.find(";");
	CutVector.push_back( atoi ( env_input.substr(0,end).c_str() ) ) ;
	if (end != std::string::npos) env_input= env_input.substr(end+1);
      } while (end != std::string::npos);     
    }
    return CutVector;  
  }


