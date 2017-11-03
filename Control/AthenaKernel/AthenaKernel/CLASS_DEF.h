/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_CLASS_DEF_H
#define ATHENAKERNEL_CLASS_DEF_H
/** @file AthenaKernel/CLASS_DEF.h
 *  @brief macros to associate a CLID to a type
 *
 *  @author Paolo Calafiura <pcalafiura@lbl.gov>
 *  $Id: CLASS_DEF.h,v 1.3 2009-01-15 19:07:29 binet Exp $
 */

#include "AthenaKernel/ClassID_traits.h"
#include "CxxUtils/unused.h"
#include "CxxUtils/checker_macros.h"
#include <boost/preprocessor/stringize.hpp>

#ifdef __CLING__
# define CLIDREGISTRY_ADDENTRY(CID, NAME)                               \
  namespace detail {							\
    const bool UNUSED(clidEntry_ ## CID) =                              \
      CLIDRegistry::addEntry(CID, typeid(NAME),                         \
                                  ClassID_traits< NAME >::typeNameString(), \
				  ClassID_traits< NAME >::packageInfo(), \
				  ClassName< NAME >::name());		\
  } 
# define CLIDREGISTRY_ADDENTRY2(CID, ARG1, ARG2)                        \
  namespace detail {							\
    const bool UNUSED(clidEntry_ ## CID) =                              \
      CLIDRegistry::addEntry                                            \
      (CID, typeid(ARG1,ARG2),                                          \
       ClassID_traits< ARG1,ARG2 >::typeNameString(),                   \
       ClassID_traits< ARG1,ARG2 >::packageInfo(),			\
       ClassName< ARG1,ARG2 >::name());					\
  } 
#else
# define CLIDREGISTRY_ADDENTRY(CID, NAME)                               \
  namespace detail {							\
    const bool UNUSED(clidEntry_ ## CID) =                              \
      CLIDRegistry::addEntry<CID>(typeid(NAME),                         \
                                  ClassID_traits< NAME >::typeNameString(), \
				  ClassID_traits< NAME >::packageInfo(), \
				  ClassName< NAME >::name());		\
  } 
# define CLIDREGISTRY_ADDENTRY2(CID, ARG1, ARG2)                        \
  namespace detail {							\
    const bool UNUSED(clidEntry_ ## CID) =                              \
      CLIDRegistry::addEntry<CID>                                       \
      (typeid(ARG1,ARG2),                                               \
       ClassID_traits< ARG1,ARG2 >::typeNameString(),                   \
       ClassID_traits< ARG1,ARG2 >::packageInfo(),			\
       ClassName< ARG1,ARG2 >::name());					\
  } 
#endif

/** @def CLASS_DEF(NAME, CID , VERSION) 
 *  @brief associate a clid and a version to a type
 *  eg 
 *  @code 
 *  CLASS_DEF(std::vector<Track*>,8901, 1)
 *  @endcode 
 *  @param NAME 	type name
 *  @param CID 		clid
 *  @param VERSION 	not yet used
 */
#define CLASS_DEF(NAME, CID , VERSION)                                  \
  template <>                                                           \
  struct ClassID_traits< NAME > {                                       \
    typedef std::is_base_of<DataObject, NAME> isDObj_t;                 \
    static const bool s_isDataObject = isDObj_t::value;                 \
    typedef std::integral_constant<bool, s_isDataObject> is_DataObject_tag; \
    typedef std::true_type has_classID_tag;                             \
    static const CLID& ID ATLAS_CHECK_THREAD_SAFETY () { static const CLID c(CID); return  c; }    \
    static const char* typeNameString ATLAS_CHECK_THREAD_SAFETY () {    \
      return #NAME;                                                     \
    }									\
    static const std::string& typeName ATLAS_CHECK_THREAD_SAFETY () {   \
      static const std::string name = typeNameString();                 \
      return name;		 					\
    }									\
    static  Athena::PackageInfo packageInfo() {				\
      static const Athena::PackageInfo pi( BOOST_PP_STRINGIZE(PACKAGE_VERSION_UQ)  ); \
      return pi;							\
    }									\
    static const std::type_info& typeInfo() {				\
      return typeid (NAME);						\
    }									\
    typedef std::true_type has_version_tag;                             \
    static const int s_version = VERSION;                               \
    static const bool s_isConst = false;                                \
  };									\
  CLIDREGISTRY_ADDENTRY(CID, NAME)


/** @def CLASS_DEF2(ARG1, ARG2, CID , VERSION) 
 *  @brief hack to use instead of CLASS_DEF when type name contains a comma ','
 *  eg
 *  @code 
 *    CLASS_DEF2(map<int,string>,8900, 1)
 *  @endcode 
 *  @param ARG1         type name (1st part)
 *  @param ARG2 	type name (2nd part)
 *  @param CID 		clid
 *  @param VERSION 	not yet used
 */
#define CLASS_DEF2(ARG1, ARG2, CID , VERSION)	\
  template <>					\
  struct ClassID_traits< ARG1,ARG2 > {					\
    typedef std::is_base_of<DataObject, ARG1, ARG2 > isDObj_t;          \
    static const bool s_isDataObject = isDObj_t::value;                 \
    typedef std::integral_constant<bool, s_isDataObject> is_DataObject_tag; \
    typedef std::true_type has_classID_tag;                             \
    static const CLID& ID ATLAS_CHECK_THREAD_SAFETY () {                \
      static const CLID c(CID); return  c;                              \
    }									\
    static const char* typeNameString ATLAS_CHECK_THREAD_SAFETY () {    \
      return #ARG1 "," #ARG2;                                           \
    }									\
    static const std::string& typeName ATLAS_CHECK_THREAD_SAFETY () {   \
      static const std::string name = typeNameString();                 \
      return name;							\
    }									\
    static const std::type_info& typeInfo() {				\
      return typeid (ARG1,ARG2);					\
    }									\
    static  Athena::PackageInfo packageInfo() {				\
      return Athena::PackageInfo("Package-00-00-00");			\
    }									\
    typedef std::true_type has_version_tag;                             \
    static const int s_version = VERSION;                               \
    static const bool s_isConst = false;                                \
  };									\
  CLIDREGISTRY_ADDENTRY2(CID, ARG1, ARG2)

#endif // not ATHENAKERNEL_CLASS_DEF_H
