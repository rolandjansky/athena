/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_EVENTINFOPIXELMODULESTATUSMONITORING_H
#define DERIVATIONFRAMEWORK_EVENTINFOPIXELMODULESTATUSMONITORING_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthLinks/ElementLink.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "StoreGate/ReadHandleKey.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelConditionsData/PixelDCSHVData.h"
#include "PixelConditionsData/PixelDCSTempData.h"
#include "PixelConditionsData/PixelDeadMapCondData.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

namespace DerivationFramework {

  class EventInfoPixelModuleStatusMonitoring : public AthAlgTool, public IAugmentationTool {
    public: 
      EventInfoPixelModuleStatusMonitoring(const std::string& type, const std::string& name, const IInterface* parent);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:

      Gaudi::Property<std::string> m_prefix
      { this,"DecorationPrefix", "", "" };

      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey
      { this, "ContainerName", "EventInfo", ""};

      SG::ReadCondHandleKey<PixelDCSTempData> m_readKeyTemp
      {this, "ReadKeyeTemp", "PixelDCSTempCondData", "Key of input sensor temperature conditions folder"};

      SG::ReadCondHandleKey<PixelDCSHVData> m_readKeyHV
      {this, "ReadKeyHV",    "PixelDCSHVCondData", "Key of input bias voltage conditions folder"};

      SG::ReadCondHandleKey<PixelDeadMapCondData> m_condDeadMapKey
      {this, "PixelDeadMapCondData", "PixelDeadMapCondData", "Pixel deadmap conditions key"};

      ToolHandle<IInDetConditionsTool> m_pixelSummary
      {this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool for PixelConditionsSummaryTool"};

      const PixelID* m_pixelID;
      mutable std::atomic_uint m_lbCounter{0};

      std::vector<SG::WriteDecorHandleKey<xAOD::EventInfo> > m_moduleConditionKeys;
      std::vector<SG::WriteDecorHandleKey<xAOD::EventInfo> > m_moduleFEmaskKeys;
      std::vector<SG::WriteDecorHandleKey<xAOD::EventInfo> > m_moduleBSErrKeys;

  };
}

#endif
