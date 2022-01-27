/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGGERPROCESSORTOOL_H
#define TRIGGERPROCESSORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRDO/NSW_PadTriggerDataContainer.h"
#include "MuonRDO/NSW_TrigRawDataContainer.h"

namespace NSWL1 {

  class TriggerProcessorTool : public AthAlgTool {

  public:

    TriggerProcessorTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TriggerProcessorTool();

    virtual StatusCode initialize() override;

    StatusCode mergeRDO(const Muon::NSW_PadTriggerDataContainer* padTriggerContainer,
                        const Muon::NSW_TrigRawDataContainer* MMTriggerContainer,
                        Muon::NSW_TrigRawDataContainer* trigRdoContainer);

  private:

  };

}
#endif    // TRIGGERPROCESSORTOOL_H
