///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CallGraphAuditor.cxx 
// Implementation file for class PerfMon::CallGraphAuditor
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/INamedInterface.h"

// PerfMonKernel includes
#include "PerfMonKernel/ICallGraphBuilderSvc.h"

// PerfMonComps includes
#include "CallGraphAuditor.h"

namespace PerfMon {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
CallGraphAuditor::CallGraphAuditor( const std::string& name, 
				    ISvcLocator* pSvcLocator ) : 
  Auditor       ( name, pSvcLocator ),
  m_callGraphSvc( "PerfMon::CallGraphBuilderSvc/CallGraphSvc", name )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty, "descr" );

  // for AuditorHandle ? someday ?
  //declareInterface<ICallGraphAuditor>(this);
}

// Destructor
///////////////
CallGraphAuditor::~CallGraphAuditor()
{ 
  //m_msg << MSG::DEBUG << "Calling destructor" << endreq;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode
CallGraphAuditor::initialize()
{
  // retrieve the callgraph service
  if ( !m_callGraphSvc.retrieve().isSuccess() ) {
    // FIXME: use a datamember MsgStream !
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR
	<< "Could not retrieve [" << m_callGraphSvc.typeAndName() << "] !!"
	<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void CallGraphAuditor::beforeInitialize( INamedInterface* alg )
{
  m_callGraphSvc->openNode( alg->name() + ":initialize" );
  return;
}

void CallGraphAuditor::afterInitialize( INamedInterface* alg )
{
  m_callGraphSvc->closeNode( alg->name() + ":initialize" );
  return;
}

void CallGraphAuditor::beforeReinitialize( INamedInterface* alg )
{
  m_callGraphSvc->openNode( alg->name() + ":reinitialize" );
  return;
}

void CallGraphAuditor::afterReinitialize( INamedInterface* alg )
{
  m_callGraphSvc->closeNode( alg->name() + ":reinitialize" );
  return;
}

void CallGraphAuditor::beforeExecute( INamedInterface* alg )
{
  m_callGraphSvc->openNode( alg->name() + ":execute" );
  return;
}

void CallGraphAuditor::afterExecute( INamedInterface* alg, const StatusCode& ) 
{
  m_callGraphSvc->closeNode( alg->name() + ":execute" );
  return;
}

void CallGraphAuditor::beforeBeginRun( INamedInterface* alg )
{
  m_callGraphSvc->openNode( alg->name() + ":beginRun" );
  return;
}

void CallGraphAuditor::afterBeginRun( INamedInterface* alg )
{
  m_callGraphSvc->closeNode( alg->name() + ":beginRun" );
  return;
}

void CallGraphAuditor::beforeEndRun( INamedInterface* alg )
{
  m_callGraphSvc->openNode( alg->name() + ":endRun" );
  return;
}

void CallGraphAuditor::afterEndRun( INamedInterface* alg )
{
  m_callGraphSvc->closeNode( alg->name() + ":endRun" );
  return;
}

void CallGraphAuditor::beforeFinalize( INamedInterface* alg )
{
  m_callGraphSvc->openNode( alg->name() + ":finalize" );
  return;
}

void CallGraphAuditor::afterFinalize( INamedInterface* alg )
{
  m_callGraphSvc->closeNode( alg->name() + ":finalize" );
  return;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} // end namespace PerfMon
