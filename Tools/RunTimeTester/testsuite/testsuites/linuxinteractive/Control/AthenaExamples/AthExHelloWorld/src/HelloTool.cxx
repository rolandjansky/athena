/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HelloTool.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

HelloTool::HelloTool( const std::string& type, const std::string& name,
		      const IInterface* parent ) 
  : AthAlgTool( type, name, parent ), m_myMessage("Default message set in HelloTool.cxx")
{
  declareProperty( "MyMessage", m_myMessage, "something to say" );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if ( riid == IHelloTool::interfaceID() )  {
      *ppvIf = (IHelloTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }

   return AthAlgTool::queryInterface( riid, ppvIf );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloTool::saySomething()
{
  ATH_MSG_INFO ("my message to the world: " << m_myMessage);

   return StatusCode::SUCCESS;
}
