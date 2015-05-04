/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTCALOESD_XAODCALOCLUSTERS_H
#define HLTCALOESD_XAODCALOCLUSTERS_H

#include "TrigHLTMonitoring/IHLTMonTool.h"


class HLTCaloESD_xAODCaloClusters: public IHLTMonTool {
  
 public:
  
  HLTCaloESD_xAODCaloClusters(const std::string & type, const std::string & name, const IInterface* parent);
  
  virtual ~HLTCaloESD_xAODCaloClusters();
  
  StatusCode init();
  StatusCode book();
  StatusCode fill(); 
  StatusCode proc();
  
 private:
  
  std::string m_HLT_cont_name; // xAOD::CaloClusterContainer
  std::string m_OFF_cont_name; // xAOD::CaloClusterContainer

  std::string m_mongroup_name;
  
  float            m_HLT_min_et;
  std::vector<int> m_HLT_types;
  
  float            m_OFF_min_et;
  std::vector<int> m_OFF_types;
  
  bool  m_match_types;
  float m_max_delta_r;

};

#endif
