/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIJETREC_HISUBTRACTEDCELLMAKERTOOL_H
#define HIJETREC_HISUBTRACTEDCELLMAKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "HIJetRec/IHIUEModulatorTool.h"
#include "AsgTools/ToolHandle.h"

#include <string>
#include <vector>


class HISubtractedCellMakerTool : public AthAlgTool, virtual ICaloCellMakerTool
{

public:

  HISubtractedCellMakerTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;
  

private:
  std::string m_event_shape_key;
  ToolHandle<IHIUEModulatorTool> m_modulator_tool;



};

#endif

