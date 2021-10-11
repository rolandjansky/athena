/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_CLASS_DEF_H
#define ATHENAKERNEL_CLASS_DEF_H
/** @file AthenaKernel/CLASS_DEF.h
 *  @brief macros to associate a CLID to a type
 *
 *  @author Paolo Calafiura <pcalafiura@lbl.gov>
 */

#include "AthenaKernel/ClassID_traits.h"
#include "CxxUtils/checker_macros.h"

#ifdef __CLING__
# define CLIDREGISTRY_ADDENTRY(CID, NAME)                               \
  namespace detail {							\
    [[maybe_unused]]                                                    \
    const bool clidEntry_ ## CID =                                      \
      CLIDRegistry::addEntry(CID, typeid(NAME),                         \
                                  ClassID_traits< NAME >::typeNameString(), \
                                  ClassName< NAME >::name());       \
  } 
# define CLIDREGISTRY_ADDENTRY2(CID, ARG1, ARG2)                        \
  namespace detail {							\
    [[maybe_unused]]                                                    \
    const bool clidEntry_ ## CID =                                      \
      CLIDRegistry::addEntry                                            \
      (CID, typeid(ARG1,ARG2),                                          \
       ClassID_traits< ARG1,ARG2 >::typeNameString(),                   \
       ClassName< ARG1,ARG2 >::name());					\
  } 
#else
# define CLIDREGISTRY_ADDENTRY(CID, NAME)                               \
  namespace detail {							\
    [[maybe_unused]]                                                    \
    const bool clidEntry_ ## CID =                                      \
      CLIDRegistry::addEntry<CID>(typeid(NAME),                         \
                                  ClassID_traits< NAME >::typeNameString(), \
                                  ClassName< NAME >::name());       \
  } 
# define CLIDREGISTRY_ADDENTRY2(CID, ARG1, ARG2)                        \
  namespace detail {							\
    [[maybe_unused]]                                                    \
    const bool clidEntry_ ## CID =                                      \
      CLIDRegistry::addEntry<CID>                                       \
      (typeid(ARG1,ARG2),                                               \
       ClassID_traits< ARG1,ARG2 >::typeNameString(),                   \
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
    typedef std::true_type has_version_tag;                             \
    static const int s_version = VERSION;                               \
    static const bool s_isConst = false;                                \
  };									\
  CLIDREGISTRY_ADDENTRY2(CID, ARG1, ARG2)

#endif // not ATHENAKERNEL_CLASS_DEF_H
