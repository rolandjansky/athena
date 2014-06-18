/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_CLIDREGISTRY_H
# define SGTOOLS_CLIDREGISTRY_H
/** @file CLIDRegistry.h
 * @brief  a static registry of CLID->typeName entries. NOT for general use.
 * Use ClassIDSvc instead.
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 *$Id: CLIDRegistry.h,v 1.2 2009-01-15 19:07:29 binet Exp $
 */

#include <vector>
#include <string>
#include <utility> /* pair */
#include <boost/tuple/tuple.hpp>
#include <boost/static_assert.hpp>

/* #define CLIDREG_DEBUG 1 */
#ifdef CLIDREG_DEBUG
 #include <iostream> 
#endif

#include "AthenaKernel/tools/AthenaPackageInfo.h"
#ifndef BOOST_HAS_STATIC_ASSERT
# include "CxxUtils/unused.h"
#endif

namespace detail {
  /// @name allowed class id range
  //@{	
  const unsigned long MINCLID = 256;
  const unsigned long MAXCLID = 2147483647; ///< 2**31 - 1
  //@}	
}

/** @class CLIDRegistry
 * @brief  a static registry of CLID->typeName entries. NOT for general use.
 * Use ClassIDSvc instead.
 */
class CLIDRegistry {
private:
  typedef boost::tuple <unsigned long, 
			std::string, 
			Athena::PackageInfo,
			std::string> tuple_t;
  typedef std::vector< tuple_t > CLIDRegistryImpl;
public:
  typedef CLIDRegistryImpl::const_iterator const_iterator;

  ///to be called by the CLASS_DEFS
  template <unsigned long CLID>
  static bool addEntry(const std::string& typeName, 
		       const Athena::PackageInfo& pkgInfo,
		       const std::string& typeInfoName); 
  //    static bool addEntry(unsigned long id, const std::string& typeName);

  /// allowed class id range (backward compatibility)
  //@{	
  static const unsigned long MINCLID;
  static const unsigned long MAXCLID;
  //@}	
 
  /// registry accessors (used by ClassIDSvc)
  //@{
  static const_iterator begin();
  static const_iterator end(); 
  /// are there new entries since last call? Does not move the entries ptr
  static bool hasNewEntries();
  /// returns an iterator range over the entries added since last time
  /// newEntries was called
  static std::pair<const_iterator, const_iterator> newEntries();
  //@}

private:
  static CLIDRegistryImpl& registry();
};


//the drudgery below is to issue a compilation error when CLID is
//out of range
template <bool x> struct ERROR_CLID_out_of_CLIDRegistry_range;
template <>       struct ERROR_CLID_out_of_CLIDRegistry_range<true>{};

#ifndef __GCCXML__

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
template <unsigned long CLID>
bool CLIDRegistry::addEntry(const std::string& typeName, 
			    const Athena::PackageInfo& pkgInfo,
			    const std::string& typeInfoName) {
  //more drudgery
#ifdef BOOST_HAS_STATIC_ASSERT
  static_assert (detail::MINCLID <= CLID && CLID <= detail::MAXCLID,
                 "CLID out of CLIDRegistry range");
#else
  UNUSED(typedef ::boost::static_assert_test<
    sizeof(ERROR_CLID_out_of_CLIDRegistry_range< (bool)(detail::MINCLID <= CLID && CLID <= detail::MAXCLID) >)
         > BOOST_JOIN(boost_static_assert_typedef_, __LINE__));
#endif

  registry().push_back(tuple_t(CLID, typeName, pkgInfo, typeInfoName));
#ifdef CLIDREG_DEBUG
  std::cerr << "CLIDRegistry::addEntry: for CLID/type " 
	    << CLID << '/' << typeName << " to registry " <<&registry() 
	    <<std::endl;
#endif		
  return true;
}

#endif

#endif // SGTOOLS_CLIDREGISTRY_H
