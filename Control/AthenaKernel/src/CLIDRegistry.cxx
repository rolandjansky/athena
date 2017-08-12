/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

#include "AthenaKernel/CLIDRegistry.h"
#include "CxxUtils/checker_macros.h"
#include <unordered_map>
#include <mutex>


class CLIDRegistryImpl
{
public:
  bool hasNewEntries() const;

  CLIDRegistry::CLIDVector_t newEntries();

  const std::type_info* CLIDToTypeinfo (CLID clid) const;
  CLID typeinfoToCLID (const std::type_info& ti) const;

  bool addEntry (unsigned long clid,
                 const std::type_info& ti,
                 const char* typeName, 
                 const Athena::PackageInfo& pkgInfo,
                 const std::string& typeInfoName);


private:
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutable mutex_t m_mutex;

  CLIDRegistry::CLIDVector_t m_vec;
  size_t m_alreadyDone = 0;
  
  typedef std::unordered_map<CLID, const std::type_info*> clid_ti_map_t;
  clid_ti_map_t m_clid_ti_map;

  typedef std::unordered_map<const std::type_info*, CLID> ti_clid_map_t;
  ti_clid_map_t m_ti_clid_map;
};


bool CLIDRegistryImpl::hasNewEntries() const
{
  lock_t lock (m_mutex);
  return m_alreadyDone < m_vec.size();
}


CLIDRegistry::CLIDVector_t
CLIDRegistryImpl::newEntries()
{
  lock_t lock (m_mutex);
  size_t pos = m_alreadyDone;
  m_alreadyDone = m_vec.size();
  return CLIDRegistry::CLIDVector_t (m_vec.begin()+pos, m_vec.end());
}


/**
 * @brief Return the @c type_info corresponding to a CLID.
 * @param clid The CLID to find.
 *
 * Returns the corresponding @c type_info or nullptr.
 */
const std::type_info* CLIDRegistryImpl::CLIDToTypeinfo (CLID clid) const
{
  lock_t lock (m_mutex);
  auto i = m_clid_ti_map.find (clid);
  if (i != m_clid_ti_map.end()) {
    return i->second;
  }
  return nullptr;
}


/**
 * @brief Return the CLID corresponding to a @c type_info.
 * @param ti The @c type_info to find.
 *
 * Returns the corresponding @c CLID or CLID_NULL.
 */
CLID CLIDRegistryImpl::typeinfoToCLID (const std::type_info& ti) const
{
  lock_t lock (m_mutex);
  auto i = m_ti_clid_map.find (&ti);
  if (i != m_ti_clid_map.end()) {
    return i->second;
  }
  return CLID_NULL;
}


bool CLIDRegistryImpl::addEntry (unsigned long clid,
                                 const std::type_info& ti,
                                 const char* typeName, 
                                 const Athena::PackageInfo& pkgInfo,
                                 const std::string& typeInfoName)
{
  lock_t lock (m_mutex);
  m_vec.emplace_back (clid, std::string(typeName), pkgInfo, typeInfoName);
  m_clid_ti_map[clid] = &ti;
  m_ti_clid_map[&ti] = clid;
  return true;
}


//***************************************************************************


bool
CLIDRegistry::hasNewEntries()
{
  return impl().hasNewEntries();
}

CLIDRegistry::CLIDVector_t
CLIDRegistry::newEntries()
{
  return impl().newEntries();
}


CLIDRegistryImpl& CLIDRegistry::impl()
{
  static CLIDRegistryImpl reg ATLAS_THREAD_SAFE;
  return reg;
}


/**
 * @brief Return the @c type_info corresponding to a CLID.
 * @param clid The CLID to find.
 *
 * Returns the corresponding @c type_info or nullptr.
 */
const std::type_info* CLIDRegistry::CLIDToTypeinfo (CLID clid)
{
  return impl().CLIDToTypeinfo (clid);
}


/**
 * @brief Return the CLID corresponding to a @c type_info.
 * @param ti The @c type_info to find.
 *
 * Returns the corresponding @c CLID or CLID_NULL.
 */
CLID CLIDRegistry::typeinfoToCLID (const std::type_info& ti)
{
  return impl().typeinfoToCLID (ti);
}


/// Out-of-line part of addEntry().
bool CLIDRegistry::addEntry (unsigned long clid,
                             const std::type_info& ti,
                             const char* typeName, 
                             const Athena::PackageInfo& pkgInfo,
                             const std::string& typeInfoName)
{
  return impl().addEntry (clid, ti, typeName, pkgInfo, typeInfoName);
}

