/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// Dear emacs, this is -*-c++-*-
#ifndef __ASGELECTRONPHOTONCORRECTIONCONFIGHELPER__
#define __ASGELECTRONPHOTONCORRECTIONCONFIGHELPER__

/**
   @brief Tool to simplify the configuration of the ElectronPhotonVariableCorrectionTool based on TEnv input
   @author Nils Gillwald (DESY) nils.gillwald@desy.de
   @date February 2020
*/
#include <string>
#include <vector>
#include <map>
class TEnv;

namespace AsgConfigHelper{
  std::string  findConfigFile (std::string input, const std::map<std::string,std::string>& configmap);
  unsigned int  findMask (std::string input, const std::map<std::string,unsigned int>& maskmap);
  std::vector<double> HelperDouble(const std::string& input,  TEnv& env);  
  std::vector<float> HelperFloat(const std::string& input,  TEnv& env);  
  std::vector<int> HelperInt(const std::string& input, TEnv& env);
}


#endif //__ASGELECTRONPHOTONCORRECTIONCONFIGHELPER__
