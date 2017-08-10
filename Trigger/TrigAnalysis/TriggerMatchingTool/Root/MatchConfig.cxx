/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/MatchConfig.h"
namespace Trig{

  MatchConfig::MatchConfig() : m_thresholds_by_type{
    {xAOD::Type::Muon, 0.1},
    {xAOD::Type::Electron, 0.1},
    {xAOD::Type::Photon, 0.1},
    {xAOD::Type::Tau, 0.1},
  } {
  }

  MatchConfig::MatchConfig(const std::map<xAOD::Type::ObjectType, double>& thresholds) :  MatchConfig() {
    for(auto type_thr : thresholds){
      m_thresholds_by_type[type_thr.first] = type_thr.second;
    }
  }

  MatchConfig::~MatchConfig() {
  }
  
  double MatchConfig::matchThreshold(xAOD::Type::ObjectType type) const {
    auto pos = m_thresholds_by_type.find(type);
    if(pos == m_thresholds_by_type.end()){
      return 0;
    }
    auto x =  pos->second;
    return x;
  }

}//end Trig namespace
