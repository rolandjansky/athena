// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVLInfo.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthContainers/src/DVLInfo.cxx
 * @author scott snyder
 * @date Mar, 2008
 * @brief Holder to implement conversion copies for @c DataVector/@c DataList.
 *
 * Out-of-line implementations.
 */


#include "AthContainers/tools/DVLInfo.h"
#include "AthContainers/tools/threading.h"
#ifndef XAOD_STANDALONE
#include "SGTools/CLIDRegistry.h"
#endif
#include <typeinfo>
#include <string>
#include <memory>
#include <unordered_map>


using AthContainers_detail::mutex;
using AthContainers_detail::lock_guard;


namespace DataModel_detail {


/// Maps from tinfo pointers and CLID to @c DVLInfo instances.
/// Note that we don't rely on static initialization here,
/// to avoid initialization ordering problems.
typedef std::unordered_map<const std::type_info*, DVLInfoBase*>
   dvl_tinfo_map_t;
dvl_tinfo_map_t* s_dvl_tinfo_map ATLAS_THREAD_SAFE = 0;
mutex s_dvl_tinfo_mutex;

struct dvlmapdel {
  ~dvlmapdel() { delete s_dvl_tinfo_map; }
} s_dvlmapdel;




/**
 * @brief Constructor.
 * @param tinfo Type info object for the container being described.
 * @param elt_tinfo Type info object for the element type of the container
 *                  being described (with pointer and const's stripped.)
 *
 * Note: these objects should only be allocated statically.
 */
DVLInfoBase::DVLInfoBase (const std::type_info& tinfo,
                          const std::type_info& elt_tinfo)
  : m_tinfo (tinfo),
    m_elt_tinfo (elt_tinfo)
{
  lock_guard<mutex> lock (s_dvl_tinfo_mutex);
  if (s_dvl_tinfo_map == 0)
    s_dvl_tinfo_map = new dvl_tinfo_map_t;
  (*s_dvl_tinfo_map)[&tinfo] = this;
}


/**
 * @brief Find the @c DVLInfo for the container @a tinfo.
 * @param tinfo @c type_info of the desired container.
 * @returns Pointer to the @c DVLInfo, or 0 if not found.
 */
DVLInfoBase* DVLInfoBase::find (const std::type_info& tinfo)
{
  lock_guard<mutex> lock (s_dvl_tinfo_mutex);
  if (!s_dvl_tinfo_map)
    return 0;
  dvl_tinfo_map_t::iterator i = s_dvl_tinfo_map->find (&tinfo);
  if (i != s_dvl_tinfo_map->end())
    return i->second;
  return 0;
}


#ifdef XAOD_STANDALONE
DVLInfoBase* DVLInfoBase::find (CLID /*clid*/)
{
  return 0;
}

CLID DVLInfoBase::clid() const
{
  return CLID_NULL;
}
#else
/**
 * @brief Find the @c DVLInfo for the container @a tinfo.
 * @param clid @c CLID of the desired container.
 * @param tinfo @c type_info of the desired container.
 * @returns Pointer to the @c DVLInfo, or 0 if not found.
 */
DVLInfoBase* DVLInfoBase::find (CLID clid)
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return DVLInfoBase::find (*ti);
  return 0;
}


/**
 * @brief Return the CLID for the container.
 */
CLID DVLInfoBase::clid() const
{
  return CLIDRegistry::typeinfoToCLID (m_tinfo);
}
#endif


} // namespace DataModel_detail
