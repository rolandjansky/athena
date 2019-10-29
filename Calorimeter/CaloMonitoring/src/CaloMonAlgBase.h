/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOMONITORING_CALOMONALGBASE
#define CALOMONITORING_CALOMONALGBASE

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/IDQFilterTool.h"
#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "LArRecEvent/LArCollisionTime.h"

class CaloMonAlgBase : public AthMonitorAlgorithm {
 public:
  
   CaloMonAlgBase(const std::string& name, ISvcLocator* pSvcLocator);

   virtual ~CaloMonAlgBase(){};   

   virtual StatusCode initialize();

   StatusCode checkFilters(bool &ifPass, bool &passBeamBackgroundRemoval, const std::string &MonGroupName, const EventContext &ctx) const; 
   
 private:
  // LArCollisionTime name
  SG::ReadHandleKey<LArCollisionTime> m_LArCollisionTimeKey{this,"LArCollisionTimeKey","LArCollisionTime"};
  SG::ReadHandleKey<BeamBackgroundData>  m_beamBackgroundKey{this,"BeamBackgroundKey","CSCBackgroundForCaloMon"};

  // Handles on filtering tools
  bool m_useBadLBTool;
  ToolHandle<IDQFilterTool> m_BadLBTool;
  bool m_useReadyFilterTool;
  ToolHandle<IDQFilterTool> m_ReadyFilterTool;

  bool m_useLArNoisyAlg;
  //bool m_useTriggerFilter;
  bool m_useCollisionFilterTool;
  bool m_useBeamBackgroundRemoval;
 

};


#endif 
