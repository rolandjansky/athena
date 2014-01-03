/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcMdtPrepDataFillerTool_h
#define D3PD_L1TgcMdtPrepDataFillerTool_h

#include "D3PDMakerUtils/VoidBlockFillerTool.h"

namespace Muon { class MdtPrepData; }



namespace D3PD {
  class L1TgcMdtPrepDataFillerTool : public BlockFillerTool<void>
  {
    public:
      L1TgcMdtPrepDataFillerTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);
      virtual StatusCode book();
      virtual StatusCode fill();

    private:
      std::string m_sgKey;

      std::vector<float> *m_prd_x;
      std::vector<float> *m_prd_y;
      std::vector<float> *m_prd_z;
      std::vector<int> *m_prd_adc;
      std::vector<int> *m_prd_tdc;
      std::vector<int> *m_prd_status;
      std::vector<float> *m_prd_drift_radius;
      std::vector<float> *m_prd_drift_radius_error;
  };
}
#endif
/* eof */
