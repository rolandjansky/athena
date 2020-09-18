/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaBaseTool.h"

egammaBaseTool::egammaBaseTool(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IegammaBaseTool>(this);
}

StatusCode
egammaBaseTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode
egammaBaseTool::finalize()
{
  return StatusCode::SUCCESS;
}

