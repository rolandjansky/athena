/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTCALOESD_CALOCELLS_H
#define HLTCALOESD_CALOCELLS_H

#include "TrigHLTMonitoring/IHLTMonTool.h"


class HLTCaloESD_CaloCells: public IHLTMonTool {
  
 public:
  
  HLTCaloESD_CaloCells(const std::string & type, const std::string & name, const IInterface* parent);
  
  virtual ~HLTCaloESD_CaloCells();
  
  StatusCode init();
  StatusCode book();
  StatusCode fill(); 
  StatusCode proc();
  
 private:
  
  std::string m_HLT_cont_name; // CaloCellContainer
  std::string m_OFF_cont_name; // CaloCellContainer
  
  std::string m_mongroup_name;

  float    m_HLT_min_et;
  float    m_OFF_min_et;
  
};

#endif

