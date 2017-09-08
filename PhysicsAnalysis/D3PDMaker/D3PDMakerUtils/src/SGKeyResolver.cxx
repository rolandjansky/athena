/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/SGKeyResolver.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2010
 * @brief Helper to resolve the SG key to use.
 */


#include "D3PDMakerUtils/SGKeyResolver.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/tokenizer.hpp"
#include "boost/foreach.hpp"


namespace D3PD {


/**
 * @brief Constructor.
 * @param name The name of the tool/alg in which this is used.
 * @param sg The event store service.
 * @param sgkey The property of the requested key.
 */
SGKeyResolver::SGKeyResolver (std::string name,
                              ServiceHandle<StoreGateSvc>& sg,
                              std::string& sgkey)
  : m_name (name),
    m_sg (sg),
    m_clid (CLID_NULL),
    m_sgkey (sgkey),
    m_haveKey (false)
{
}


/**
 * @brief Initialize.
 * @param clid The CLID of the desired object.
 * @param typname The name of the desired type.
 */
StatusCode SGKeyResolver::initialize (CLID clid, std::string typname)
{
  m_clid = clid;
  m_typname = typname;
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the SG key we should use.
 */
std::string SGKeyResolver::key()
{
  if (m_haveKey)
    return m_usedKey;

  // Split the requested string into individual keys.
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(" ,");
  tokenizer tokens(m_sgkey, sep);
  std::vector<std::string> keys (tokens.begin(), tokens.end());

  // First see if any existing proxies match the keys.
  std::vector<const SG::DataProxy*> proxies = m_sg->proxies();
  BOOST_FOREACH (const SG::DataProxy* & p, proxies) {
    if (p->transientID (m_clid) &&
        std::find (keys.begin(), keys.end(), p->name()) != keys.end())
    {
      if (!m_haveKey ||
          std::find (keys.begin(), keys.end(), p->name()) <
          std::find (keys.begin(), keys.end(), m_usedKey))
      {
        m_usedKey = p->name();
        m_haveKey = true;
      }
    }
  }

  // If we haven't found one, try retrieving proxies individually.
  // This can call to the proxy providers.
  if (!m_haveKey) {
    BOOST_FOREACH (std::string& key, keys) {
      m_usedKey = key;
      if (m_sg->proxy (m_clid, m_usedKey)) {
        m_haveKey = true;
        break;
      }
    }
  }

  if (m_haveKey) {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::VERBOSE, m_name)
      << "Using StoreGate object: "
      << m_typname << "(" << m_clid << ")/" << m_usedKey;
  }

  return m_usedKey;
}


} // namespace D3PD
