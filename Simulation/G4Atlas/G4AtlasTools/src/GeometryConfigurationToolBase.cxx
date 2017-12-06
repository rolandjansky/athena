/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Primary include
#include "G4AtlasTools/GeometryConfigurationToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GeometryConfigurationToolBase
//
// 2016-08-16: Andrea Dell'Acqua
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GeometryConfigurationToolBase::GeometryConfigurationToolBase(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
  : base_class(type,name,parent)
{

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode GeometryConfigurationToolBase::initialize()
{
  ATH_MSG_DEBUG("GeometryyConfigurationToolBase::initialize()");

  return StatusCode::SUCCESS;
}

StatusCode GeometryConfigurationToolBase::preGeometryConfigure() const
{
  ATH_MSG_DEBUG("GeometryyConfigurationToolBase::preGeometryConfigure()");

  return StatusCode::SUCCESS;
}

StatusCode GeometryConfigurationToolBase::postGeometryConfigure() const
{
  ATH_MSG_DEBUG("GeometryyConfigurationToolBase::postGeometryConfigure()");

  return StatusCode::SUCCESS;
}
