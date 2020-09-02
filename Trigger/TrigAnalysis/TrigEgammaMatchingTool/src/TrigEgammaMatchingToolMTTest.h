/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrigEgammaMatchingToolMTTest_H
#define TrigEgammaMatchingToolMTTest_H


#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/TrigEgammaMatchingToolMT.h"
#include "xAODEgamma/ElectronContainer.h"
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include <vector>
#include <string>


class TrigEgammaMatchingToolMTTest : public AthMonitorAlgorithm {
  public:

    TrigEgammaMatchingToolMTTest( const std::string& name, ISvcLocator* pSvcLocator );
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:

            
    void inspect(const std::string, const xAOD::Egamma *) const ;

    SG::ReadHandleKey<xAOD::ElectronContainer> m_electronKey{this , "ElectronKey", "Electrons", ""};

    ToolHandle<Trig::TrigDecisionTool> m_trigdec;
    ToolHandle<TrigEgammaMatchingToolMT> m_matchTool;


    std::vector<std::string> m_triggerList;
};

#endif
