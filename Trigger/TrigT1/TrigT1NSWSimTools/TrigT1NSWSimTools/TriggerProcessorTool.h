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

    TriggerProcessorTool(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
    virtual ~TriggerProcessorTool();

    virtual StatusCode initialize() override;  

    StatusCode mergeRDO();
    
  private:

    SG::WriteHandleKey<Muon::NSW_TrigRawDataContainer> m_trigRdoContainerKey{this,"trigRdoContainerKey","NSW_TrigRawData","Write handle key for NSW TrigRawData"};
    SG::ReadHandleKey<Muon::NSW_PadTriggerDataContainer> m_padTriggerContainerKey{this,"padTriggerContainerKey","NSWPADTRGRDO","Name of the pad trigger RDO"};
    
  };

}


#endif    // TRIGGERPROCESSORTOOL_H
