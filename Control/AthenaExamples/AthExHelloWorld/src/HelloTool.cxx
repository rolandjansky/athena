/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HelloTool.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

HelloTool::HelloTool( const std::string& type, const std::string& name,
		      const IInterface* parent ) 
  : base_class( type, name, parent )
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloTool::saySomething()
{
  ATH_MSG_INFO ("my message to the world: " << (std::string)m_myMessage);

   return StatusCode::SUCCESS;
}
