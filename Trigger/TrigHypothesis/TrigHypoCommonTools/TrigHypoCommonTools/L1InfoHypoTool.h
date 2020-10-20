/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHYPOCOMMONTOOLS_L1INFOHYPOTOOL_H
#define TRIGHYPOCOMMONTOOLS_L1INFOHYPOTOOL_H

#include "Gaudi/Property.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1Decoder/CTPUnpackingTool.h"

#include "IL1InfoHypoTool.h"

#include <vector>


class L1InfoHypoTool : public extends<AthAlgTool, IL1InfoHypoTool> {
    
public:
    L1InfoHypoTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual StatusCode initialize() override;
    StatusCode decide(const std::vector<IL1InfoHypoTool::L1Info>& input) const override;
    const HLT::Identifier& getId() const override; 

private:
    HLT::Identifier m_decisionId;
    Gaudi::Property <std::vector<std::string>> m_l1ItemNames {this, "L1ItemNames"};
    ToolHandle <CTPUnpackingTool> m_ctpUnpackingTool {this, "CTPUnpackingTool", "CTPUnpackingTool"};
};

#endif //TRIGHYPOCOMMONTOOLS_L1INFOHYPOTOOL_H