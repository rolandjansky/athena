/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoModelTool.h"

/**
 ** Constructor(s)
 **/
GeoModelTool::GeoModelTool( const std::string& type, const std::string& name, const IInterface* parent )
  : AthAlgTool( type, name, parent )
  , m_detector(0)
{
  declareInterface<IGeoModelTool>( this );
}

/**
 ** Destructor
 **/
GeoModelTool::~GeoModelTool()  {
}

/**
 ** The Detector Node corresponding to this tool
 **/
GeoVDetectorManager* GeoModelTool::manager() const {
  return m_detector;
}

StatusCode GeoModelTool::clear()
{
  return StatusCode::SUCCESS;
}

StatusCode GeoModelTool::align(IOVSVC_CALLBACK_ARGS)
{
  return StatusCode::SUCCESS;
}

StatusCode GeoModelTool::registerCallback()
{
  // Return Failure since no function has been registered
  return StatusCode::FAILURE;
}
