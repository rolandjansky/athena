/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthExStoreGateExample/src/ExampleAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Example of using handles for data access.
 */


#include "ExampleAlg.h"


ExampleAlg::ExampleAlg (const std::string& name, ISvcLocator* svcLocator)
  : AthReentrantAlgorithm (name, svcLocator)
{
  // Declare the keys as properties.
  // You can set defaults here that can be overridden during job configuration.
  declareProperty ("ReadKey", m_readKey = "in");
  declareProperty ("WriteKey", m_writeKey = "out");
}


StatusCode ExampleAlg::initialize()
{
  // This will check that the properties were initialized properly
  // by job condifiguration.
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode ExampleAlg::execute (const EventContext& ctx) const
{
  // Construct handles from the keys.
  // Since this is a reentrant algorithm, we have an explicit event context,
  // which we pass to the handles.
  SG::ReadHandle<MyDataObj> h_read (m_readKey, ctx);
  SG::WriteHandle<MyDataObj> h_write (m_writeKey, ctx);

  // Now we can dereference the read handle to access input data.
  int newval = h_read->val()+1;

  // We make a new object, held by a unique_ptr, and record it
  // in the store using the record method of the handle.
  ATH_CHECK( h_write.record (std::make_unique<MyDataObj> (newval)) );
  return StatusCode::SUCCESS;
}


