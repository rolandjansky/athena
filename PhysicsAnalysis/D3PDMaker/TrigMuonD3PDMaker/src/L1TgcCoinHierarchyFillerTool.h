/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcCoinHierarchyFillerTool_h
#define D3PD_L1TgcCoinHierarchyFillerTool_h

#include "GaudiKernel/ServiceHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace Trigger { class IMuonRoiChainSvc; }
namespace Trigger { class ITgcCoinHierarchySvc; }



namespace D3PD {
  class L1TgcCoinHierarchyFillerTool : public BlockFillerTool<void>
  {
    public:
      L1TgcCoinHierarchyFillerTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);

      // Standard Gaudi initialize method.
      virtual StatusCode initialize();

      virtual StatusCode book();
      virtual StatusCode fill();

    private:
      ServiceHandle<Trigger::IMuonRoiChainSvc> m_muonRoiChainSvc;
      ServiceHandle<Trigger::ITgcCoinHierarchySvc> m_tgcCoinHierarchySvc;

      std::string m_l1RoiKey;

      std::vector<int> *m_index;
      std::vector<int> *m_deltaR_hiPt;
      std::vector<int> *m_deltaPhi_hiPt;
      std::vector<int> *m_deltaR_tracklet;
      std::vector<int> *m_deltaPhi_tracklet;
      std::vector<int> *m_isInEndcapChamberBoundary;
  };
}
#endif
/* eof */
