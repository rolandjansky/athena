/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcRpcPrepDataFillerTool_h
#define D3PD_L1TgcRpcPrepDataFillerTool_h

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"

namespace Muon { class MuonIdHelperTool; }
namespace Muon { class RpcPrepData; }



namespace D3PD {
  class L1TgcRpcPrepDataFillerTool : public BlockFillerTool<void>
  {
    public:
      L1TgcRpcPrepDataFillerTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);

      // Standard Gaudi initialize method.
      virtual StatusCode initialize();

      virtual StatusCode book();
      virtual StatusCode fill();

    private:
      ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
      std::string m_sgKey;

      std::vector<float> *m_prd_x;
      std::vector<float> *m_prd_y;
      std::vector<float> *m_prd_z;
      std::vector<float> *m_prd_time;
      std::vector<int> *m_prd_triggerInfo;
      std::vector<int> *m_prd_ambiguityFlag;
      std::vector<int> *m_prd_measuresPhi;
      std::vector<int> *m_prd_inRibs;
      std::vector<int> *m_prd_station;
      std::vector<int> *m_prd_stationEta;
      std::vector<int> *m_prd_stationPhi;
      std::vector<int> *m_prd_doubletR;
      std::vector<int> *m_prd_doubletZ;
      std::vector<float> *m_prd_stripWidth;
      std::vector<float> *m_prd_stripLength;
      std::vector<float> *m_prd_stripPitch;
  };
}
#endif
/* eof */
