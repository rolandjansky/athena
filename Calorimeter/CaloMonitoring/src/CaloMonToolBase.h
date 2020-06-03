/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOMONITORING_CALOMONTOOLSBASE
#define CALOMONITORING_CALOMONTOOLSBASE

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaMonitoring/IDQFilterTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "LArRecEvent/LArCollisionTime.h"
#include "RecBackgroundEvent/BeamBackgroundData.h"

class TH1F;

class CaloMonToolBase : public ManagedMonitorToolBase {
 public:
  
   CaloMonToolBase(const std::string & type, const std::string& name, 
		   const IInterface* parent);

   ~CaloMonToolBase();   

   StatusCode initialize();

   virtual StatusCode bookBaseHists(MonGroup* group);
   StatusCode checkFilters(bool& ifPass); 
   
 protected:
   int m_lb;
   bool m_passBeamBackgroundRemoval;
   TH1I* m_h_EvtRejSumm;

 private:
   // Handles on filtering tools
  bool m_useBadLBTool;
  ToolHandle<IDQFilterTool> m_BadLBTool;
  bool m_useReadyFilterTool;
  ToolHandle<IDQFilterTool> m_ReadyFilterTool;

  bool m_useLArNoisyAlg;
  //bool m_useTriggerFilter;
  bool m_useCollisionFilterTool;
  bool m_useBeamBackgroundRemoval;
 
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey{this, "EventInfoKey", "EventInfo"};
  SG::ReadHandleKey<LArCollisionTime> m_LArCollisionTimeKey{this, "LArCollisionTimeKey", "LArCollisionTime"};
  SG::ReadHandleKey<BeamBackgroundData> m_beamBackgroundKey{this, "BeamBackgroundKey", "CSCBackgroundForCaloMon"};

};


#endif 
