///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CallGraphBuilderSvc.cxx 
// Implementation file for class CallGraphBuilderSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

// PerfMonComps includes
#include "CallGraphBuilderSvc.h"

namespace {
  typedef PerfMon::CallGraphBuilderSvc::NodeId_t NodeId_t;
  static const std::string s_rootName = "<root-node>";
  static const NodeId_t    s_rootId   = 0;
}

namespace PerfMon {

// initialize the static "unique identifier"
// FIXME: what happens when we wrap around ??
NodeId_t CallGraphBuilderSvc::m_uuid = 0;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
CallGraphBuilderSvc::CallGraphBuilderSvc( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  AthService( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty, "descr" );

  // install the root of the nodes for our callgraph modelisation
  // (would be so simple with a boost::bimap ... (boost 1.35 ?)
  m_nameToId[s_rootName] = s_rootId;
  m_idToName[s_rootId  ] = s_rootName;
}

// Destructor
///////////////
CallGraphBuilderSvc::~CallGraphBuilderSvc()
{ 
  msg() << MSG::DEBUG << "Calling destructor" << endmsg;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode CallGraphBuilderSvc::initialize()
{
  // initialize MsgStream
  msg().setLevel( m_outputLevel.value() );

  msg() << MSG::INFO 
      << "Initializing " << name() << "..." 
      << endmsg;

  msg() << MSG::DEBUG << "Initializing base class..." << endmsg;
  if ( AthService::initialize().isFailure() ) {
    msg() << MSG::ERROR
        << "Could not initialize base class !!"
        << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg() << MSG::VERBOSE << "Base class initialized" << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode CallGraphBuilderSvc::finalize()
{
  msg() << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endmsg;

  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
CallGraphBuilderSvc::queryInterface( const InterfaceID& riid, 
				     void** ppvInterface )
{
  if ( ICallGraphBuilderSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ICallGraphBuilderSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void CallGraphBuilderSvc::openNode( const std::string& nodeName )
{
  if ( m_nameToId.find( nodeName ) == m_nameToId.end() ) {
    m_nameToId[nodeName] = ++m_uuid;
    m_idToName[m_uuid  ] = nodeName;
  }

  const NodeId_t nodeId = m_nameToId[nodeName];
  const NodeId_t parentId = m_stack.empty() ? 0 : m_stack.top();
  m_stack.push( nodeId );

  boost::add_edge( parentId, nodeId, m_graph );

  msg() << MSG::VERBOSE 
	<< "--> [" << nodeName << "] = " << nodeId << "\t"
	<< "(parent = " << m_idToName[parentId] << ")"
	<< endmsg;
  return;
}

void CallGraphBuilderSvc::closeNode( const std::string& nodeName )
{
  const NodeId_t nodeId = m_nameToId[nodeName];
  //boost::add_edge( nodeId, 1, m_graph);
  msg() << MSG::VERBOSE
	<< "<-- [" << nodeName << "] = " << nodeId 
	<< endmsg;

  if ( !m_stack.empty() ) { 
    m_stack.pop();
  }
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

