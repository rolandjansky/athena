///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TTreeEventSelectorHelperSvc.cxx 
// Implementation file for class TTreeEventSelectorHelperSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "TTreeEventSelectorHelperSvc.h"

// STL includes
#include <sstream>

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// ROOT includes
#include <TChain.h>

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TTreeEventSelectorHelperSvc::TTreeEventSelectorHelperSvc
( const std::string& name, ISvcLocator* pSvcLocator ) : 
  ::AthService( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "InputCollections", 
                   m_inputCollectionsName,
                   "List of input (ROOT) file names" );
  m_inputCollectionsName.declareUpdateHandler
    ( &TTreeEventSelectorHelperSvc::setupInputCollection, this );

  declareProperty( "TupleName",
                   m_tupleName = "",
                   "Name of the TTree to load/read from input file(s)" );

  declareProperty( "ActiveBranches",
                   m_activeBranchNames,
                   "List of branch names to activate" );

}

// Destructor
///////////////
TTreeEventSelectorHelperSvc::~TTreeEventSelectorHelperSvc()
{}

// Athena Service's Hooks
////////////////////////////
StatusCode TTreeEventSelectorHelperSvc::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "..." 
		<< endmsg
		<< "loading chain of trees into ITHistSvc...");

  if ( !configureTupleSvc().isSuccess() ) {
    ATH_MSG_ERROR ("Could not load TChain into ITHistSvc !");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TTreeEventSelectorHelperSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
TTreeEventSelectorHelperSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( TTreeEventSelectorHelperSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<TTreeEventSelectorHelperSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return ::AthService::queryInterface(riid, ppvInterface);
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

/// callback to synchronize the list of input files
void 
TTreeEventSelectorHelperSvc::setupInputCollection( Property& /*inputCollectionsName*/ )
{
  // nothing ?
  return;
}

StatusCode
TTreeEventSelectorHelperSvc::configureTupleSvc()
{
  // retrieve ITHistSvc
  ServiceHandle<ITHistSvc> svc( "THistSvc/THistSvc", this->name() );
  if ( !svc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Could not retrieve [" << svc.typeAndName() << "] !");
    return StatusCode::FAILURE;
  }

  if ( m_tupleName.empty() ) {
    ATH_MSG_ERROR
      ("You have to give a TTree name to read from the ROOT files !");
    return StatusCode::FAILURE;
  }

  setupInputCollection( m_inputCollectionsName );
  const std::size_t nbrInputFiles = m_inputCollectionsName.value().size();
  if ( nbrInputFiles < 1 ) {
    ATH_MSG_ERROR
      ("You need to give at least 1 input file !!" << endmsg
       << "(Got [" << nbrInputFiles << "] file instead !)");
    return StatusCode::FAILURE;
  }

  TChain* tuple = new TChain( m_tupleName.c_str() );
  for ( std::size_t iFile = 0; iFile != nbrInputFiles; ++iFile ) {
    const std::string& fileName = m_inputCollectionsName.value()[iFile];
    ATH_MSG_DEBUG ("adding [" << fileName << "]");
    tuple->Add( fileName.c_str() );
  }
  // fixup TChain index
  tuple->SetTreeIndex( 0 );
  
  // by default, disable all branches: 
  // clients will have to explicitly tell which ones they want to read
  tuple->SetBranchStatus("*",0);

  // and activate the one we have been told to
  for ( std::size_t i = 0, iMax = m_activeBranchNames.size();
	i != iMax;
	++i ) {
    tuple->SetBranchStatus( m_activeBranchNames[i].c_str(), 1 );
  }

  // register our tuple under stream 'temp' (in-memory special stream)
  // ! don't forget to unregister it ! 
  // (should be done in TTreeEventSelector::finalize())
  std::ostringstream tupleName;
  tupleName << "/temp/TTreeStream/" << tuple->GetName();
  if ( !svc->regTree( tupleName.str(), tuple ).isSuccess() ) {
    ATH_MSG_ERROR ("Couldn't register (chain of) tuple(s)");
    delete tuple; tuple =0;
    return StatusCode::FAILURE;
  }

  // we leak the TChain here.
  // ownership is relinquinshed to TTreeEventSelector.
  tuple = 0;

  return StatusCode::SUCCESS;
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


