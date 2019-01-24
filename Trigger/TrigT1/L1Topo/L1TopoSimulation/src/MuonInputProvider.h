/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_MuonInputProvider
#define L1TopoSimulation_MuonInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include <vector>
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/MuCTPIToRoIBSLink.h"

class TH1I;
class TH2I;

class ITHistSvc;

namespace TrigConf {
   class ILVL1ConfigSvc;
   class TriggerThreshold;
}

namespace TCS {
   class MuonTOB;
   class LateMuonTOB;
}

namespace LVL1MUCTPI {
  class IMuctpiSimTool;
}

namespace LVL1 {

   class RecMuonRoiSvc;
   class MuCTPIL1TopoCandidate;

   class MuonInputProvider : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      MuonInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~MuonInputProvider();

      virtual StatusCode initialize() override;

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const override;

      virtual void handle(const Incident&) override;

   private:

      TCS::MuonTOB createMuonTOB(uint32_t roiword) const;
      TCS::MuonTOB createMuonTOB(const MuCTPIL1TopoCandidate & roi) const;
      TCS::LateMuonTOB createLateMuonTOB(const MuCTPIL1TopoCandidate & roi) const;
      /**
         @brief convert the 2-bit value from MuCTPIL1TopoCandidate::getptL1TopoCode() to an actual pt

         The muon TOB encodes pt values in 2 bits.
         A MuCTPIL1TopoCandidate provides the encoded 2-bit value with
         the function getptL1TopoCode().
         This function uses the information from the l1 trigger menu
         configuration to convert the threshold to an actual pt value.
         For more details, see ATR-16781.
      */
      unsigned int topoMuonPtThreshold(const MuCTPIL1TopoCandidate &mctpiCand) const;

      SG::ReadHandleKey<ROIB::RoIBResult> m_roibLocation;

      ServiceHandle<ITHistSvc> m_histSvc;

      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

      ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc;
      ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc;

      ToolHandle<LVL1MUCTPI::IMuctpiSimTool> m_MuctpiSimTool;

      std::vector< TrigConf::TriggerThreshold* > m_MuonThresholds;

      SG::ReadHandleKey<L1MUINT::MuCTPIToRoIBSLink> m_muonROILocation;    //!<  Muon ROIs SG key

      TH1I * m_hPt {nullptr};
      TH2I * m_hEtaPhi {nullptr};

      int m_MuonEncoding; //!< Use 0 for full granularity; 1 for MuCTPiToTopo muon granularity
      SG::ReadHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPItoL1TopoLocation;
      SG::ReadHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPItoL1TopoLocationPlusOne;
   };
}

#endif
