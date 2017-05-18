/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// L1Decoder includes
#include "JRoIsUnpackingTool.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
JRoIsUnpackingTool::JRoIsUnpackingTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
JRoIsUnpackingTool::~JRoIsUnpackingTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode JRoIsUnpackingTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  
  return StatusCode::SUCCESS;
}

StatusCode JRoIsUnpackingTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}



