/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/CLIDRegistry.h"
#include "CxxUtils/make_unique.h"
#include <unordered_map>
#include <memory>
/* #include <algorithm> */
using namespace std;

const unsigned long CLIDRegistry::MINCLID = CLIDdetail::MINCLID;
const unsigned long CLIDRegistry::MAXCLID = CLIDdetail::MAXCLID;

namespace {
unsigned int alreadyDone (0);
}


//   bool 
//   CLIDRegistry::addEntry(unsigned long id, const string& typeName) {
//     assert( MINCLID <= id && id <= MAXCLID );
//     registry().push_back(pair<unsigned long, string>(id,typeName));
//     return true;
//   }


CLIDRegistry::const_iterator CLIDRegistry::begin() {
#ifdef CLIDREG_DEBUG
  cerr << "CLIDRegistry::begin: returns " 
	    << &*(registry().begin()) << " for registry " <<&registry() 
	    <<endl;
#endif	
  return registry().begin();
}


CLIDRegistry::const_iterator CLIDRegistry::end() {
#ifdef CLIDREG_DEBUG
  cerr << "CLIDRegistry::end: returns " 
	    << &*(registry().end()) << " for registry " <<&registry() 
	    <<endl;
#endif	
  return registry().end();
  //return unique(registry().begin(), registry().end());  //FIXME O(N)!
}

bool
CLIDRegistry::hasNewEntries() {
  return (alreadyDone < CLIDRegistry::registry().size());
}

std::pair<CLIDRegistry::const_iterator, CLIDRegistry::const_iterator> 
CLIDRegistry::newEntries() {
  std::pair<CLIDRegistry::const_iterator, CLIDRegistry::const_iterator> ret =
     std::make_pair(CLIDRegistry::begin()+alreadyDone, 
                    CLIDRegistry::end());
  alreadyDone = CLIDRegistry::registry().size();
  return ret;
}


CLIDRegistry::CLIDRegistryImpl& CLIDRegistry::registry() {
  static CLIDRegistryImpl reg;
  return reg;
}


namespace {
typedef std::unordered_map<unsigned long, const std::type_info*> clid_ti_map_t;
std::unique_ptr<clid_ti_map_t> clid_ti_map;

typedef std::unordered_map<const std::type_info*, unsigned long> ti_clid_map_t;
std::unique_ptr<ti_clid_map_t> ti_clid_map;
}


/**
 * @brief Add a new CLID <> type_info mapping.
 * @param clid The CLID of the class.
 * @param ti The @c type_info of the class.
 */
void CLIDRegistry::addCLIDMapping (unsigned long clid, const std::type_info& ti)
{
  if (!clid_ti_map)
    clid_ti_map = CxxUtils::make_unique<clid_ti_map_t>();
  if (!ti_clid_map)
    ti_clid_map = CxxUtils::make_unique<ti_clid_map_t>();

  (*clid_ti_map)[clid] = &ti;
  (*ti_clid_map)[&ti] = clid;
}


/**
 * @brief Return the @c type_info corresponding to a CLID.
 * @param clid The CLID to find.
 *
 * Returns the corresponding @c type_info or nullptr.
 */
const std::type_info* CLIDRegistry::CLIDToTypeinfo (unsigned long clid)
{
  if (clid_ti_map) {
    auto i = clid_ti_map->find (clid);
    if (i != clid_ti_map->end())
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
unsigned long CLIDRegistry::typeinfoToCLID (const std::type_info& ti)
{
  if (ti_clid_map) {
    auto i = ti_clid_map->find (&ti);
    if (i != ti_clid_map->end())
      return i->second;
  }
  return 0;
}


/// Out-of-line part of addEntry().
bool CLIDRegistry::addEntry (unsigned long clid,
                             const std::type_info& ti,
                             const char* typeName, 
                             const Athena::PackageInfo& pkgInfo,
                             const std::string& typeInfoName)
{
  registry().push_back(tuple_t(clid, std::string(typeName), pkgInfo, typeInfoName));
#ifdef CLIDREG_DEBUG
  std::cerr << "CLIDRegistry::addEntry: for CLID/type " 
	    << clid << '/' << typeName << " to registry " <<&registry() 
	    <<std::endl;
#endif		
  addCLIDMapping (clid, ti);
  return true;
}

