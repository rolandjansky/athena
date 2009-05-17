/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/ToolUsingTool.h"


/////////////////////////////////////////////////////////////////////////////

ToolUsingTool::ToolUsingTool( const std::string& type, const std::string& name,
		      const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    m_pubTool( "ConcreteTool/PublicConcreteTool" ),         // no parent, making it public
    m_privTool( "ConcreteTool", this ), m_toolArray( this ) // adding 'this' makes it private
{

// declare any properties here

   declareProperty( "Factor", m_factor = 1., "multiplicative factor to perform task" );

   declareProperty( "TheToolPubTool",   m_pubTool,   "global helper to offload tasks to" );
   declareProperty( "TheToolTool",      m_privTool,  "helper to offload some tasks to" );
   declareProperty( "TheToolToolArray", m_toolArray, "list of helpers to offload some tasks to" );
   
   // set default toolArray
   m_toolArray.push_back( "ConcreteTool/CxxDefaultTool" );
}


//___________________________________________________________________________
StatusCode ToolUsingTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if ( riid == IVirtualTool::interfaceID() )  {
      *ppvIf = (IVirtualTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }

   return AthAlgTool::queryInterface( riid, ppvIf );
}

//___________________________________________________________________________
StatusCode ToolUsingTool::initialize()
{

// perform necessary one-off initialization

// verify that our tool handle is pointing to an accessible tool
   if ( m_pubTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL ("Failed to retrieve " << m_pubTool);
      return StatusCode::FAILURE;
   } else {
      ATH_MSG_INFO ("Retrieved " << m_pubTool);
   }

   if ( m_privTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL ("Failed to retrieve " << m_privTool);
      return StatusCode::FAILURE;
   } else {
      ATH_MSG_INFO ("Retrieved " << m_privTool);
   }
   
   if ( m_toolArray.retrieve().isFailure() ) {
     ATH_MSG_ERROR ("Failed to retreive " << m_toolArray);
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_INFO ("Retrieved " << m_toolArray);
   }

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode ToolUsingTool::finalize()
{

// perform work done at shutdown

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode ToolUsingTool::performTask( double& result )
{

// do what needs to be done
  ATH_MSG_INFO ("performing task using tool (factor: " << m_factor << ") ...");

   StatusCode sc = m_pubTool->performTask( result );
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR ("the global, public tool failed it's task ... stop!");
     return sc;
   }

   sc = m_privTool->performTask( result );
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR ("my tool failed it's task ... stop!");
     return sc;
   }

   msg(MSG::INFO) << "... task using tool is done";
   if ( msgLvl(MSG::DEBUG) ) { msg() << ", result: " << result; }
   msg(MSG::INFO) << endreq;

// make list of tools do their work
   double newvalue = 12345.0;
   ToolHandleArray< IVirtualTool >::iterator itPriv = m_toolArray.begin(),
     itPrivEnd = m_toolArray.end();
   for (  ; itPriv != itPrivEnd; ++itPriv ) {
     sc = (*itPriv)->performTask( newvalue );
     if ( sc.isFailure() ) {
       ATH_MSG_ERROR
	 ("tool " << (*itPriv).typeAndName() << " failed it's task ... stop!");
       return sc;
     }
   }
 
   return StatusCode::SUCCESS;
}
