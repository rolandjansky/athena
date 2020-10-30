/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHYPOCOMMONTOOLS_L1INFOHYPO_H
#define TRIGHYPOCOMMONTOOLS_L1INFOHYPO_H

#include "GaudiKernel/ToolHandle.h"

#include "DecisionHandling/HypoBase.h"

#include <string>
#include <vector>

#include "IL1InfoHypoTool.h"

class L1InfoHypo : public ::HypoBase {

  public:
    L1InfoHypo( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& context) const override;

  private:
    ToolHandleArray <IL1InfoHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools that perform actual selection"};

    SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey {this, "RoIBResult", "RoIBResult", "Name of RoIBResult"};

};

#endif // TRIGHYPOCOMMONTOOLS_L1INFOHYPO_H
