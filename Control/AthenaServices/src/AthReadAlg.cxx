/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
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

  std::string storename;
  const std::string& objkey = m_key.objKey();
  std::string::size_type ppos = objkey.find ('+');
  if (ppos != std::string::npos) {
    storename = objkey.substr (0, ppos+1);
  }
  for (const std::string& a : m_aliases) {
    this->addDependency (DataObjID (m_key.clid(), storename + a),
                         Gaudi::DataHandle::Writer);
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Algorithm execute method.
 * @param ctx Event context.
 */
StatusCode AthReadAlg::execute (const EventContext& ctx) const
{
  // Look up the proxy for the original object.
  SG::DataProxy* proxy = evtStore()->proxy_exact (m_sgkey);
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

  // Any extra aliases?
  std::vector<std::string> extraAliases;
  for (const std::string& a : m_aliases) {
    if (!proxy->hasAlias (a + "_DELETED")) {
      extraAliases.push_back (a);
    }
  }

  // Restore original aliases.
  if (!proxy->alias().empty() || !extraAliases.empty()) {
    SG::DataProxy* newprox = evtStore()->proxy (m_key.clid(), m_key.key());
    if (!newprox) {
      ATH_MSG_ERROR( "Can't find proxy for just-recorded object " << 
                     m_key.clid() << "/" << m_key.key());
      return StatusCode::FAILURE;
    }
    SG::DataObjectSharedPtr<DataObject> dobj (newprox->accessData());

    for (std::string a : proxy->alias()) {
#if __cplusplus >= 201709
      if (a.ends_with ("_DELETED"))
#else
      if (a.compare (a.size() - 8, 8, "_DELETED") == 0)
#endif
      {
        a.erase (a.size() - 8, 8);
        if (!evtStore()->recordObject (dobj, a, false, true)) {
          ATH_MSG_ERROR( "Can't make alias " << a << " for " <<
                         m_key.clid() << "/" << m_key.key());
          return StatusCode::FAILURE;
        }
      }
    }

    for (const std::string& a : extraAliases) {
      if (!evtStore()->recordObject (dobj, a, false, true)) {
        ATH_MSG_ERROR( "Can't make alias " << a << " for " <<
                       m_key.clid() << "/" << m_key.key());
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;
}

