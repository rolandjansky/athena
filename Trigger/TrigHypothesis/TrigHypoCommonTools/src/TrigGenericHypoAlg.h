/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHYPOCOMMONTOOLS_TRIGGENERICHYPOALG_H
#define TRIGHYPOCOMMONTOOLS_TRIGGENERICHYPOALG_H

#include "GaudiKernel/ToolHandle.h"
#include "DecisionHandling/HypoBase.h"
#include "TrigGenericHypoTool.h"
#include "StoreGate/ReadDecorHandleKey.h"

class TrigGenericHypoAlg : public ::HypoBase {
  public:
    TrigGenericHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& context) const override;
  private:
    ToolHandleArray <TrigGenericHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools that perform actual selection"};
    SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_trigCompKey {this, "TrigCompositeContainer", "TrigComposite", "Name of TrigCompositeContainer"};
};

#endif 
