/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Geometry/AFP_GeometryTool.h"

AFP_GeometryTool::AFP_GeometryTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

AFP_GeometryTool::~AFP_GeometryTool()
{
}

StatusCode AFP_GeometryTool::initialize()
{
  //ATH_MSG_DEBUG("Use macros for logging!");
  return StatusCode::SUCCESS;
}

StatusCode AFP_GeometryTool::finalize()
{
  return StatusCode::SUCCESS;
}

