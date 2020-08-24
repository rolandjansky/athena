/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIJETREC_HISUBTRACTEDCELLMAKERTOOL_H
#define HIJETREC_HISUBTRACTEDCELLMAKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "HIJetRec/IHIUEModulatorTool.h"
#include "HIEventUtils/HIEventShapeMapTool.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "AsgTools/ToolHandle.h"

#include <string>
#include <vector>

#include "StoreGate/ReadHandleKey.h"

class HISubtractedCellMakerTool : public AthAlgTool, virtual ICaloCellMakerTool
{

public:

  HISubtractedCellMakerTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

private:

  SG::ReadHandleKey<xAOD::HIEventShapeContainer>  m_eventShapeKey     { this, "EventShapeKey", "HIEventShapeContainer", "Event Shape Key"};
  ToolHandle<IHIUEModulatorTool> m_modulatorTool { this, "Modulator", "HIUEModulatorTool", "Handle to HIUEModulatorTool"};
  ToolHandle<IHIEventShapeMapTool> m_eventShapeMapTool { this, "EventShapeMapTool", "HIEventShapeMapTool", "Handle to Event Shape Map Tool"};

};

#endif
