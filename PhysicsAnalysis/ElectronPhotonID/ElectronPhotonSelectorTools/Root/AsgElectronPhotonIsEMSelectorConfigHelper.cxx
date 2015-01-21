/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "TEnv.h"
#include <vector>
#include <iostream>
#include <sstream>      // std::istringstream
using namespace std;


  std::vector<double> AsgConfigHelper::HelperDouble(const std::string& input,  TEnv& env){
    std::vector<double> CutVector;    
    std::string env_input(env.GetValue(input.c_str(), ""));
    if (env_input.size() > 0) {
      std::string::size_type end;
      do {
	end = env_input.find(";");
	float myValue(0);
	
	if(AsgConfigHelper::strtof(env_input.substr(0,end),myValue)){
	  CutVector.push_back(myValue);
	}
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
      float myValue(0);
      if(AsgConfigHelper::strtof(env_input.substr(0,end),myValue)){
	CutVector.push_back(myValue);
      }
      if (end != std::string::npos)	env_input= env_input.substr(end+1);
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


bool AsgConfigHelper::strtof(const std::string& input, float& f){
  int diff = 0 ;
  std::string tmp = input;
  std::string::size_type first;
  std::string::size_type last;
  
  first = ( input.find("#") ) ;
  if (first == std::string::npos) {
    f = ( atof (input.c_str() ) ) ;
    return true;
  }  
  else {
    last = (input.find("#",first+1) );
    if (last == std::string::npos) {
      std::cout << " ERROR NOT 2 #  "<< std::endl;  
      return false;
    }  
    diff = last - first ;
    tmp= tmp.erase(first,diff+1);
    std::istringstream buffer (tmp);
    buffer>>f;

    return true;
  }
}


