/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaServices/src/AthReadAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Generic algorithm wrapping an input conversion.
 */


#include "AthReadAlg.h"
#include "StoreGate/TypelessWriteHandle.h"


/**
 * @brief Standard initialize method.
 */
StatusCode AthReadAlg::initialize()
{
  // Initialize the output key.
  ATH_CHECK( m_key.initialize() );

  // AddressRemappingSvc will rename the proxy for the original object
  // so that it ends with _DELETED.  Cache the sgkey for this proxy.
  m_sgkey = evtStore()->stringToKey (m_key.key() + "_DELETED",
                                     m_key.clid());
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Algorithm execute method.
 * @param ctx Event context.
 */
StatusCode AthReadAlg::execute (const EventContext& ctx) const
{
  // Look up the proxy for the orignal object.
  const SG::DataProxy* proxy = evtStore()->proxy_exact (m_sgkey);
  if (!proxy) {
    ATH_MSG_ERROR( "Cannot find proxy for original object: " <<
                   m_key.clid() << "/" << m_key.key() << "_DELETED [" <<
                   m_sgkey << "]" );
    return StatusCode::FAILURE;
  }

  // Read the object.
  std::unique_ptr<DataObject> ptr (proxy->readData());
  if (!ptr) {
    ATH_MSG_ERROR( "Object read failed: " <<
                   m_key.clid() << "/" << m_key.key() << "_DELETED [" <<
                   m_sgkey << "]" );
    return StatusCode::FAILURE;
  }

  // Record the result in the event store.
  SG::TypelessWriteHandle h (m_key, ctx);
  ATH_CHECK( h.typelessPut (std::move (ptr) ) );

  return StatusCode::SUCCESS;
}

