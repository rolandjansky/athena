/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcTriggerInfoFillerTool_h
#define D3PD_L1TgcTriggerInfoFillerTool_h

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"

namespace Trig { class TrigDecisionTool; }



namespace D3PD {
  class L1TgcTriggerInfoFillerTool : public BlockFillerTool<void>
  {
    public:
      L1TgcTriggerInfoFillerTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);

      // Standard Gaudi initialize method.
      virtual StatusCode initialize();

      virtual StatusCode book();
      virtual StatusCode fill();

    private:
      enum {
        COMBINED = (0x1 << 16),
        STANDALONE = (0x1 << 17),
        EXTRAPOLATED = (0x1 << 18)
      };

      ToolHandle<Trig::TrigDecisionTool> m_tdt;
      std::string m_pattern;

      std::vector<std::string> *m_chain;
      std::vector<int> *m_isPassed;
      std::vector<int> *m_nTracks;

      std::vector<std::vector<int> > *m_typeVec;
      std::vector<std::vector<float> > *m_ptVec;
      std::vector<std::vector<float> > *m_etaVec;
      std::vector<std::vector<float> > *m_phiVec;
      std::vector<std::vector<int> > *m_qVec;
      std::vector<std::vector<int> > *m_l1RoiWordVec;
  };
}
#endif
/* eof */
