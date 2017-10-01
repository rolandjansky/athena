/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TAURoIsUnpackingTool.h"

TAURoIsUnpackingTool::TAURoIsUnpackingTool( const std::string& type, 
                                            const std::string& name, 
                                            const IInterface* parent ) 
  : RoIsUnpackingToolBase(type, name, parent)
{

}

StatusCode TAURoIsUnpackingTool::initialize()
{
  ATH_CHECK(RoIsUnpackingToolBase::initialize());

  return StatusCode::SUCCESS;
}

