/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaServices/src/ThinningCacheTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Create ThinningCache for a given stream.
 */


#include "ThinningCacheTool.h"
#include "AthContainers/ThinningDecision.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "SelectionVetoes.h"
#include "CompressionInfo.h"

namespace Athena {


/**
 * @brief Gaudi initialize method.
 */
StatusCode ThinningCacheTool::initialize()
{
  if (!m_trigNavigationThinningSvc.empty()) {
    ATH_CHECK( m_trigNavigationThinningSvc.retrieve() );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Called at the end of initialize.  A no-op here.
 */
StatusCode ThinningCacheTool::postInitialize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Called at the beginning of execute.  A no-op for now.
 */
StatusCode ThinningCacheTool::preExecute()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Called before actually streaming objects.
 *        Find all thinning requests for this stream, build the @c ThinningCache,
 *        and install it in the current @c EventContext.
 */
StatusCode ThinningCacheTool::preStream()
{
  m_cache.clear();

  // Nothing to do if we don't have an extended event context.
  EventContext ctx = Gaudi::Hive::currentContext();
  if (!Atlas::hasExtendedEventContext (ctx)) {
    return StatusCode::SUCCESS;
  }

  // Examine all ThinningDecision objects in the store.
  SG::ConstIterator<SG::ThinningDecision> beg;
  SG::ConstIterator<SG::ThinningDecision> end;
  // Will return FAILURE if no matches, so ignore.
  evtStore()->retrieve<SG::ThinningDecision>(beg, end).ignore();
  for (; beg != end; ++beg)
  {
    // See if this is for this stream.
    std::string key = wantedKey (beg.key());
    if (!key.empty()) {

      // Get the set of hashed SG keys for this object.
      std::vector<SG::sgkey_t> sgkeys;
      SG::DataProxy* proxy = beg->link().proxy (true);
      if (proxy) {
        sgkeys.push_back (proxy->sgkey());
        CLID pCLID = proxy->clID();
        IProxyDict* store = proxy->store();
        for (CLID clid : proxy->transientID()) {
          if (clid != pCLID) {
            sgkeys.push_back (store->stringToKey (key, clid));
          }
        }
      }

      // Add it to the cache.
      m_cache.addThinning (key, sgkeys, &*beg);
    }
  }

  // Look for any selection vetoes.
  const std::string selVetoesKey = "SelectionVetoes_" + m_streamName;
  const SG::SelectionVetoes* vetoes = nullptr;
  if (evtStore()->contains<SG::SelectionVetoes> (selVetoesKey)) {
    ATH_CHECK( evtStore()->retrieve (vetoes, selVetoesKey) );
    for (const auto& p : *vetoes) {
      m_cache.setVetoed (p.first, p.second);
    }
  }

  // Look for any compression info
  const std::string compInfoKey = "CompressionInfo_" + m_streamName;
  const SG::CompressionInfo* compInfo = nullptr;
  if (evtStore()->contains<SG::CompressionInfo> (compInfoKey)) {
    ATH_CHECK( evtStore()->retrieve (compInfo, compInfoKey) );
    for (const auto& p : *compInfo) {
      m_cache.setCompression (p.first, p.second);
    }
  }

  // Set the TrigNavigation thinning tool if needed.
  if (!m_trigNavigationThinningSvc.empty()) {
    m_cache.setTrigNavigationThinningSvc (m_trigNavigationThinningSvc.get());
  }

  // If there was any thinning for this stream, then install the cache
  // in the EventContext.
  if (!m_cache.empty() || m_cache.trigNavigationThinningSvc()) {
    m_cache.lockOwned();
    Atlas::getExtendedEventContext (ctx).setThinningCache (&m_cache);
    Gaudi::Hive::setCurrentContext (ctx);
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Called at the end of execute.
 *        Remove ThinningCache from the EventContext and clear the cache instance.
 */
StatusCode ThinningCacheTool::postExecute()
{
  if (!m_cache.empty() || m_cache.trigNavigationThinningSvc()) {
    EventContext ctx = Gaudi::Hive::currentContext();
    Atlas::getExtendedEventContext (ctx).setThinningCache (nullptr);
    Gaudi::Hive::setCurrentContext (ctx);
  }

  m_cache.clear();
  return StatusCode::SUCCESS;
}


/**
 * @brief Called at the beginning of finalize.  A no-op here.
 */
StatusCode ThinningCacheTool::preFinalize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Test if a thinning decision object is for our stream.
 * @param key The SG key of the @c ThinningDecision object.
 *
 * If this thinning decision is for our stream, then return the SG key
 * of the object being thinned.  Otherwise, return an empty string.
 */
std::string ThinningCacheTool::wantedKey (const std::string& key) const
{
  std::string suffix = "_THINNED_" + m_streamName;
  std::string::size_type spos = key.find (suffix);
  if (spos != std::string::npos &&
      (spos + suffix.size() == key.size() ||
       key[spos + suffix.size()] == '.'))
  {
    return key.substr (0, spos);
  }
  return "";
}


} // namespace Athena

