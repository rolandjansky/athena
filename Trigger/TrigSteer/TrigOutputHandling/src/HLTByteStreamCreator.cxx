/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// OutputHandling includes
#include "HLTByteStreamCreator.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"




HLTByteStreamCreator::HLTByteStreamCreator( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   )
{
  //declareProperty( "Property", m_nProperty );
}

HLTByteStreamCreator::~HLTByteStreamCreator()
{}

StatusCode HLTByteStreamCreator::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  
  return StatusCode::SUCCESS;
}

StatusCode HLTByteStreamCreator::createOutput() const {

  return StatusCode::SUCCESS;
}

StatusCode HLTByteStreamCreator::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}


