/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/TemplatedTool.h"


/////////////////////////////////////////////////////////////////////////////

template< class T >
ToolSpace::TemplatedTool< T >::TemplatedTool( const std::string& type, const std::string& name,
		      const IInterface* parent ) 
   : AthAlgTool( type, name, parent )
{

// declare any properties here

   declareProperty( "Factor", m_factor = 1., "multiplicative factor to perform task" );

}

//___________________________________________________________________________
template< class T >
StatusCode ToolSpace::TemplatedTool< T >::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if ( riid == IVirtualTool::interfaceID() )  {
      *ppvIf = (IVirtualTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }

   return AthAlgTool::queryInterface( riid, ppvIf );
}

//___________________________________________________________________________
template< class T>
StatusCode ToolSpace::TemplatedTool< T >::initialize()
{

// perform necessary one-off initialization

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
template< class T >
StatusCode ToolSpace::TemplatedTool< T >::finalize()
{

// perform work done at shutdown

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
template< class T >
StatusCode ToolSpace::TemplatedTool< T >::performTask( double& result )
{

// do what needs to be done
   ATH_MSG_INFO ("performing templated task factor ( " << m_factor << ") ...");

   result = result * 789.;

   msg(MSG::INFO) << "... templated task is done";
   if ( msgLvl(MSG::DEBUG) ) { msg() << ", result: " << result; }
   msg(MSG::INFO) << endreq;

   return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
template class ToolSpace::TemplatedTool< double >;
