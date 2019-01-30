/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAEMULATIONTOOL_TRIGEGAMMAEMULATIONTOOLALG_H
#define TRIGEGAMMAEMULATIONTOOL_TRIGEGAMMAEMULATIONTOOLALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationTool.h"

#include <string>
#include <map>

namespace Trig {
  class TrigEgammaEmulationToolAlg : public AthAlgorithm {
    public:
      TrigEgammaEmulationToolAlg(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~TrigEgammaEmulationToolAlg();
      virtual StatusCode initialize() override;
      virtual StatusCode execute() override;
      virtual StatusCode finalize() override;
    private:
      //
      std::vector<std::string>   m_triggerList;
      std::string m_perfTrigger;
      std::map<std::string, int> m_trigger_tdt;
      std::map<std::string, int> m_trigger_emu1;
      std::map<std::string, int> m_trigger_emu2;
      std::map<std::string, int> m_trigger_emu3;
    private:
      ToolHandle<Trig::TrigDecisionTool>   m_trigdec;
      ToolHandle<ITrigEgammaMatchingTool>  m_matchTool;
      ToolHandle<ITrigEgammaEmulationTool> m_emulationTool;
      StoreGateSvc              *m_storeGate;
      //
  };
}

#endif
