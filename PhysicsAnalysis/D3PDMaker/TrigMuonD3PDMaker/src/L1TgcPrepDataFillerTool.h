/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcPrepDataFillerTool_h
#define D3PD_L1TgcPrepDataFillerTool_h

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"

namespace Muon { class MuonIdHelperTool; }
namespace Muon { class TgcPrepData; }



namespace D3PD {
  class L1TgcPrepDataFillerTool : public BlockFillerTool<void>
  {
    public:
      L1TgcPrepDataFillerTool(const std::string& type,
                              const std::string& name,
                              const IInterface* parent);

      // Standard Gaudi initialize method.
      virtual StatusCode initialize();

      virtual StatusCode book();
      virtual StatusCode fill();

    private:
      StatusCode fillTgcPrepData(const Muon::TgcPrepData* data,
                                 const int bunch);
      StatusCode fillPreviousBc();
      StatusCode fillCurrentBc();
      StatusCode fillNextBc();

      ToolHandle<Muon::MuonIdHelperTool> m_idHelper;

      std::string m_tgcPrepDataPrv;
      std::string m_tgcPrepDataCur;
      std::string m_tgcPrepDataNxt;

      std::vector<float> *m_prd_x;
      std::vector<float> *m_prd_y;
      std::vector<float> *m_prd_z;
      std::vector<float> *m_prd_shortWidth;
      std::vector<float> *m_prd_longWidth;
      std::vector<float> *m_prd_length;
      std::vector<int> *m_prd_isStrip;
      std::vector<int> *m_prd_gasGap;
      std::vector<int> *m_prd_channel;
      std::vector<int> *m_prd_eta;
      std::vector<int> *m_prd_phi;
      std::vector<int> *m_prd_station;
      std::vector<int> *m_prd_bunch;
  };
}
#endif
/* eof */
