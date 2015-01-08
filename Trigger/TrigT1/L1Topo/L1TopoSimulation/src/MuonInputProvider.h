/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_MuonInputProvider
#define L1TopoSimulation_MuonInputProvider

#include "L1TopoSimulation/IInputTOBConverter.h"
#include <vector>

namespace TrigConf {
   class ILVL1ConfigSvc;
   class TriggerThreshold;
}

namespace LVL1 {

   class RecMuonRoiSvc;

   class MuonInputProvider : public extends1<AthAlgTool, IInputTOBConverter> {
   public:
      MuonInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~MuonInputProvider();

      virtual StatusCode initialize() override;

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const; 

   private:

      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

      ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc;
      ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc;

      std::vector< TrigConf::TriggerThreshold* > m_MuonThresholds;

      StringProperty m_muonROILocation;    //!<  Muon ROIs SG key

   };
}

#endif
