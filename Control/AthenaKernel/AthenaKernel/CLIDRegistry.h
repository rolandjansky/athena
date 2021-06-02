/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef ATHENAKERNEL_CLIDREGISTRY_H
# define ATHENAKERNEL_CLIDREGISTRY_H
/** @file AthenaKernel/CLIDRegistry.h
 * @brief  a static registry of CLID->typeName entries. NOT for general use.
 * Use ClassIDSvc instead.
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 */

#include "GaudiKernel/ClassID.h"

#include <vector>
#include <string>
#include <tuple>
#include <typeinfo>

namespace CLIDdetail {
  /// @name allowed class id range
  //@{	
  const unsigned long MINCLID = 256;
  const unsigned long MAXCLID = 2147483647; ///< 2**31 - 1
  //@}	
}

class CLIDRegistryImpl;

/** @class CLIDRegistry
 * @brief  a static registry of CLID->typeName entries. NOT for general use.
 * Use ClassIDSvc instead.
 */
class CLIDRegistry {
public:
  typedef std::tuple <unsigned long, 
                      std::string, 
                      std::string> tuple_t;
  typedef std::vector< tuple_t > CLIDVector_t;

  ///to be called by the CLASS_DEFS
  template <unsigned long CLID>
  static bool addEntry(const std::type_info& ti,
                       const char* typeName,
                       const std::string& typeInfoName);

  /// registry accessors (used by ClassIDSvc)
  //@{
  /// are there new entries since last call? Does not move the entries ptr
  static bool hasNewEntries();
  /// returns an iterator range over the entries added since last time
  /// newEntries was called
  static CLIDVector_t newEntries();
  //@}

  /// Translate between CLID and type_info.
  static const std::type_info* CLIDToTypeinfo (CLID clid);
  static CLID typeinfoToCLID (const std::type_info& ti);


  /// Out-of-line part of addEntry().
  static bool addEntry (unsigned long clid,
                        const std::type_info& ti,
                        const char* typeName,
                        const std::string& typeInfoName);

private:
  static CLIDRegistryImpl& impl();
};


template <unsigned long CLID>
bool CLIDRegistry::addEntry(const std::type_info& ti,
                            const char* typeName, 
                            const std::string& typeInfoName) {
  static_assert (CLIDdetail::MINCLID <= CLID && CLID <= CLIDdetail::MAXCLID,
                 "CLID out of CLIDRegistry range");

  addEntry (CLID, ti, typeName, typeInfoName);
  return true;
}


#endif // ATHENAKERNEL_CLIDREGISTRY_H
