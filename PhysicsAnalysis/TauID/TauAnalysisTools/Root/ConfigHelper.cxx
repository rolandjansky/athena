/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "TauAnalysisTools/ConfigHelper.h"

using namespace TauAnalysisTools;

void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<std::string>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(sItem);
}

void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<size_t>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(size_t(stoi(sItem)));
}

void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<int>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(stoi(sItem));
}

void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<float>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(stof(sItem));
}

void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<double>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(stod(sItem));
}
