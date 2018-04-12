/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_HIEVENTSHAPEAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_HIEVENTSHAPEAUGMENTATIONTOOL_H

#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

class IHIEventShapeSummaryTool;

namespace DerivationFramework 
{
class HIEventShapeAugmentationTool: public AthAlgTool, public IAugmentationTool 
{
public: 
  HIEventShapeAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);
  StatusCode initialize() {return StatusCode::SUCCESS;};
  StatusCode finalize() {return StatusCode::SUCCESS;};
  virtual StatusCode addBranches() const;
  
private:
  std::string m_input_container;
  std::string m_output_container;
  ToolHandle<IHIEventShapeSummaryTool> m_summary_tool;
};
}

#endif


