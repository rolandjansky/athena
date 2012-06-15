/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoModelTool.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GaudiKernel/MsgStream.h"

/**
 ** Constructor(s)
 **/
GeoModelTool::GeoModelTool( const std::string& type, const std::string& name, const IInterface* parent )
  : AthAlgTool( type, name, parent ),
    m_detector(0)
  
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
GeoVDetectorManager*
GeoModelTool::manager() const {
	return m_detector;
}

StatusCode GeoModelTool::clear(StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

StatusCode 
GeoModelTool::align(IOVSVC_CALLBACK_ARGS)
{
  return StatusCode::SUCCESS;
}

StatusCode 
GeoModelTool::registerCallback( StoreGateSvc* )
{
  // Return Failure since no function has been registered
  return StatusCode::FAILURE;
}
