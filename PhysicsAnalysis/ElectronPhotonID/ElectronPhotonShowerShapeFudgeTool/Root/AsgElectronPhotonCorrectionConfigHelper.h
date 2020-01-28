/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Dear emacs, this is -*-c++-*-
#ifndef __ASGELECTRONISEMSELECTRONCONFIGHELPER__
#define __ASGELECTRONISEMSELECTRONCONFIGHELPER__

/**
   @brief Tool to simplify the configuration of the cut based IsEM based on TEnv input

   @author Christos Anastopoulos
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


#endif
