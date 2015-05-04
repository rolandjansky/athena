/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTCALOESD_XAODTRIGEMCLUSTERS_H
#define HLTCALOESD_XAODTRIGEMCLUSTERS_H

#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "GaudiKernel/ToolHandle.h"

//#include "egammaInterfaces/IegammaShowerShape.h"


class HLTCaloESD_xAODTrigEMClusters: public IHLTMonTool {
  
 public:
  
  HLTCaloESD_xAODTrigEMClusters(const std::string & type, const std::string & name, const IInterface* parent);
  
  virtual ~HLTCaloESD_xAODTrigEMClusters();
  
  StatusCode init();
  StatusCode book();
  StatusCode fill(); 
  StatusCode proc();
  
 private:
  
  std::string m_HLT_cont_name; // xAOD::TrigEMClusterContainer
  std::string m_OFF_cont_name; //   xAOD::CaloClusterContainer

  std::string m_mongroup_name;
  
  float m_HLT_min_et;
  
  float m_OFF_min_et;
  std::vector<int> m_OFF_types;

  float m_max_delta_r;
  
  //ToolHandle<IegammaShowerShape> m_ShowerShapeTool;
  
};

#endif
