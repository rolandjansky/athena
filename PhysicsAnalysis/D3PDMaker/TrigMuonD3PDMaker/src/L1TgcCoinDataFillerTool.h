/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcCoinDataFillerTool_h
#define D3PD_L1TgcCoinDataFillerTool_h

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"

namespace Muon { class TgcCoinData; }



namespace D3PD {
  class L1TgcCoinDataFillerTool : public BlockFillerTool<void>
  {
    public:
      L1TgcCoinDataFillerTool(const std::string& type,
                              const std::string& name,
                              const IInterface* parent);
      virtual StatusCode book();
      virtual StatusCode fill();

    private:
      StatusCode fillTgcCoinData(const Muon::TgcCoinData* data,
                                 const int bunch);
      StatusCode fillPreviousBc();
      StatusCode fillCurrentBc();
      StatusCode fillNextBc();

      std::string m_tgcCoinDataPrv;
      std::string m_tgcCoinDataCur;
      std::string m_tgcCoinDataNxt;

      std::vector<float> *m_coin_x_In;
      std::vector<float> *m_coin_y_In;
      std::vector<float> *m_coin_z_In;
      std::vector<float> *m_coin_x_Out;
      std::vector<float> *m_coin_y_Out;
      std::vector<float> *m_coin_z_Out;
      std::vector<float> *m_coin_width_In;
      std::vector<float> *m_coin_width_Out;
      std::vector<float> *m_coin_width_R;
      std::vector<float> *m_coin_width_Phi;

      std::vector<int> *m_coin_isAside;
      std::vector<int> *m_coin_isForward;
      std::vector<int> *m_coin_isStrip;
      std::vector<int> *m_coin_isPositiveDeltaR;

      std::vector<int> *m_coin_type;
      std::vector<int> *m_coin_trackletId;
      std::vector<int> *m_coin_trackletIdStrip;
      std::vector<int> *m_coin_phi;
      std::vector<int> *m_coin_roi;
      std::vector<int> *m_coin_pt;
      std::vector<int> *m_coin_delta;
      std::vector<int> *m_coin_sub;
      std::vector<int> *m_coin_bunch;
  };
}
#endif
/* eof */
