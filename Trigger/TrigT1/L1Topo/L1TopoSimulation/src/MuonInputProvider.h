/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_MuonInputProvider
#define L1TopoSimulation_MuonInputProvider

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IInputTOBConverter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/LockedHandle.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/MuCTPIToRoIBSLink.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"

#include "TH1.h"
#include "TH2.h"

#include <vector>

class ITHistSvc;

namespace TrigConf
{
   class L1Menu;
} // namespace TrigConf

namespace TCS {
   class MuonTOB;
   class LateMuonTOB;
}

namespace LVL1 {

   class MuCTPIL1TopoCandidate;

   class MuonInputProvider : public extends2<AthAlgTool, IInputTOBConverter, IIncidentListener> {
   public:
      MuonInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual StatusCode initialize() override;

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const override;

      virtual void handle(const Incident&) override;

   private:
      TCS::MuonTOB createMuonTOB(uint32_t roiword, const TrigConf::L1Menu *l1menu) const;
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
      /* 
         @brief calculate the eta and phi L1Topo indices

         The exact eta and phi coordinates are rounded according to a particular L1Topo granularity
         Using product instead of division avoids unexpected rounding errors due to precision
         Also, LUTs for the firmware are built using Python 3.x numpy.round(), which is different from std::round()
         Input: x = eta/phi float values, g = inverse of eta/phi granularity
         Output: integer eta/phi L1Topo coordinates
      */
      int topoIndex(float x, int g) const;
      /* 
         @brief use L1Topo convention for muon flags (1 = true/positive, -1 = false/negative, 0 = undefined)
      */
      int topoFlag(bool flag) const;

      ServiceHandle<ITHistSvc> m_histSvc;

      ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recRPCRoiTool{this, "RecRpcRoiTool", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool", "RPC RoI reconstruction tool"};
      ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recTGCRoiTool{this, "RecTgcRoiTool", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool", "TGC RoI reconstruction tool"};

      SG::ReadHandleKey<L1MUINT::MuCTPIToRoIBSLink> m_muonROILocation { this, "MuonROILocation", LVL1MUCTPI::DEFAULT_MuonRoIBLocation, "Storegate key for the Muon ROIs" };
      SG::ReadHandleKey<ROIB::RoIBResult> m_roibLocation{ this, "ROIBResultLocation", ROIB::DEFAULT_RoIBRDOLocation, "Storegate key for the reading the ROIBResult" };
      SG::ReadHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPItoL1TopoLocation { this, "locationMuCTPItoL1Topo", LVL1MUCTPI::DEFAULT_MuonL1TopoLocation, "Storegate key for MuCTPItoL1Topo "};
      SG::ReadHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPItoL1TopoLocationPlusOne { this, "BCPlusOneLocation", "", "Storegate key for MuCTPItoL1TopoPlusOne"};
      Gaudi::Property<uint16_t> m_MuonEncoding {this, "MuonEncoding", 0, "0=full granularity Mu ROIs, 1=MuCTPiToTopo granularity"};

      mutable LockedHandle<TH1> m_hPt ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hPtTGC ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH1> m_hPtRPC ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hPtEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hPhiEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hBW2or3Eta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hInnerCoinEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hGoodMFEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hChargeEta ATLAS_THREAD_SAFE;
      mutable LockedHandle<TH2> m_hIs2candEta ATLAS_THREAD_SAFE; 
      mutable LockedHandle<TH2> m_hIsTGCEta ATLAS_THREAD_SAFE; 
   };
}

#endif
