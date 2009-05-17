/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/ConcreteTool.h"


/////////////////////////////////////////////////////////////////////////////

ConcreteTool::ConcreteTool( const std::string& type, const std::string& name,
		      const IInterface* parent ) 
   : AthAlgTool( type, name, parent )
{

// declare any properties here

   declareProperty( "Factor", m_factor = 1., "multiplicative factor to perform task" );

}

//___________________________________________________________________________
StatusCode ConcreteTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if ( riid == IVirtualTool::interfaceID() )  {
      *ppvIf = (IVirtualTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }

   return AthAlgTool::queryInterface( riid, ppvIf );
}

//___________________________________________________________________________
StatusCode ConcreteTool::initialize()
{

// perform necessary one-off initialization

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode ConcreteTool::finalize()
{

// perform work done at shutdown

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode ConcreteTool::performTask( double& result )
{

// do what needs to be done
   ATH_MSG_INFO ("performing concrete task (factor: " << m_factor << ") ...");

   result = result * 123.;

   msg(MSG::INFO) << "... concrete task is done";
   if ( msgLvl(MSG::DEBUG) ) { msg() << ", result: " << result; }
   msg(MSG::INFO) << endreq;

   return StatusCode::SUCCESS;
}
