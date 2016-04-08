/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Package includes
#include "BeginRunAlg.h"
#include "Helpers.h"

// Gaudi inludes
#include "GaudiKernel/IAlgTool.h"

// Athena includes
#include "StoreGate/StoreGate.h"

// Test conditions
#include "IOVDbTestConditions/IOVDbTestAmdbCorrection.h"

// STL includes
#include <iostream>
#include <list>
#include <iterator>


////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////
BeginRunAlg::BeginRunAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_myAlgTool("MyAlgTool")
{
  declareProperty("MyAlgTool", m_myAlgTool, "public shared tool");
  declareProperty("Key", m_key = "/AthExBeginRun/AthExSimpleCond", "StoreGate key");
}


////////////////////////////////////////////////////////////////////////////////
// Initialize algorithm
////////////////////////////////////////////////////////////////////////////////
StatusCode BeginRunAlg::initialize()
{
  ATH_MSG_INFO ("initialize()");

  // Initialize our AlgTool
  if ( m_myAlgTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR (m_myAlgTool.propertyName()
		   << ": Failed to retrieve tool " << m_myAlgTool.type());
  }

  /*******************************************************************
   * Register a DataHandle with the detector store for direct
   * access to the conditions object. This is obsolete if you register
   * a callback anyways (see below).
   *******************************************************************/
  /*
  StatusCode sc = detStore()->regHandle(m_dataHandle, m_key.value());
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Could not register DataHandle with detector store");
    return StatusCode::FAILURE;
  }
  */
  
  /*******************************************************************
   * Register the DataHandle and a callback.
   * Will get triggered whenever the object changes
   *******************************************************************/
  ATH_MSG_DEBUG("Registering callback");
  StatusCode sc = detStore()->regFcn(&BeginRunAlg::iovCallback, this,
                                     m_dataHandle, m_key.value());
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Could not register IOV callback with detector store");
    return StatusCode::FAILURE;
  }
        
  return StatusCode::SUCCESS;
}



////////////////////////////////////////////////////////////////////////////////
// Event loop
////////////////////////////////////////////////////////////////////////////////
StatusCode BeginRunAlg::execute()
{
 /*******************************************************************
  * Dereferencing the DataHandle
  * The IOVDbSvc takes care that you get the currently valid object.
  * This is faster than StoreGate::retrieve and therefore preferred.
  *******************************************************************/
  ATH_MSG_INFO ("execute: Conditions via DataHandle: " << (*m_dataHandle));
      
  return StatusCode::SUCCESS;
}



////////////////////////////////////////////////////////////////////////////////
// Start of run
////////////////////////////////////////////////////////////////////////////////
StatusCode BeginRunAlg::beginRun()
{
  ATH_MSG_INFO ("beginRun()");

  /*******************************************************************
  * Get the conditions object from the detector store using
  * a regular StoreGate retrieve. This is fine for beginRun().
  *******************************************************************/
  
  const IOVDbTestAmdbCorrection* corr;
  StatusCode sc = detStore()->retrieve(corr, m_key.value());
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Cannot retrieve conditions object from detector store.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO ("beginRun: Conditions from detector store: " << (*corr));

  // ...
  // Typically you would perform some additional actions with this
  // object here (update of internal caches, re-initializations, ...)
  // ...
  
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// End of run
////////////////////////////////////////////////////////////////////////////////
StatusCode BeginRunAlg::endRun()
{
  ATH_MSG_INFO ("endRun()");

  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// IOV callback
////////////////////////////////////////////////////////////////////////////////
StatusCode BeginRunAlg::iovCallback( IOVSVC_CALLBACK_ARGS_P(/*idx*/,keys) )
{
  msg(MSG::INFO) << "IOV callback triggered by ";
  
  /*******************************************************************
   * Print out all keys of DataHandles that this function
   * was registered against, which were reset. You can then do any
   * necessary re-initializations.
   *******************************************************************/

  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    msg() << *itr << " ";
  }
  msg() << endreq;
  
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// Finalize
////////////////////////////////////////////////////////////////////////////////
StatusCode BeginRunAlg::finalize()
{
  ATH_MSG_INFO ("finalize()");

  return StatusCode::SUCCESS;
}
