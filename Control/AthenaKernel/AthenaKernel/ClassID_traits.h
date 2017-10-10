/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_CLASSID_TRAITS_H
#define ATHENAKERNEL_CLASSID_TRAITS_H
/** @file AthenaKernel/ClassID_traits.h
 * @brief  a traits class that associates a CLID to a type T
 * It also detects whether T inherits from Gaudi DataObject
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 * $Id: ClassID_traits.h,v 1.3 2009-01-15 19:07:29 binet Exp $
 */
#include <string>
#include <typeinfo>

#include <boost/static_assert.hpp>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/System.h"

#include "AthenaKernel/CLIDRegistry.h"
#include "AthenaKernel/ClassName.h"

#include "AthenaKernel/tools/AthenaPackageInfo.h"
#ifndef BOOST_HAS_STATIC_ASSERT
# include "CxxUtils/unused.h"
#endif
#include <type_traits>



template <bool x> struct ERROR_you_should_use_the_CLASS_DEF_macro_to_define_CLID_and_VERSION;
template <>       struct ERROR_you_should_use_the_CLASS_DEF_macro_to_define_CLID_and_VERSION<true>{};

///internal use: issues a compilation error when condition B is false
#ifdef BOOST_HAS_STATIC_ASSERT
#define MY_STATIC_ASSERT( B ) \
  static_assert (B, "You should use the CLASS_DEF macro to define CLID and VERSION");
#else
#define MY_STATIC_ASSERT( B ) \
  UNUSED(typedef ::boost::static_assert_test<                           \
      sizeof(ERROR_you_should_use_the_CLASS_DEF_macro_to_define_CLID_and_VERSION< (bool)( B ) >)>\
         BOOST_JOIN(boost_static_assert_typedef_, __LINE__))
#endif

/** @class ClassID_traits
 * @brief  a traits class that associates a CLID to a type T
 * It also detects whether T inherits from Gaudi DataObject
 */
template <typename T>
struct ClassID_traits {
  //default only works for DataObjects
  typedef std::is_base_of<DataObject, T> isDObj_t; 
  ///flags whether T inherits from DataObject
  static const bool s_isDataObject = isDObj_t::value;

  //default traits for class ID assignment
  typedef std::integral_constant<bool, s_isDataObject> is_DataObject_tag;

  ///the CLID of T
  static const CLID& ID() { 
    MY_STATIC_ASSERT(s_isDataObject);
    return T::classID(); 
  }

  ///the demangled type name of T
  static const std::string& typeName() {
    MY_STATIC_ASSERT(s_isDataObject);
    static const std::string tname = System::typeinfoName(typeid(T));
    return tname;
  }

  ///the type id of T
  static const std::type_info& typeInfo() {
    MY_STATIC_ASSERT(s_isDataObject);
    return typeid(T);
  }

  ///the package name of this CLASSDEF (and BY CONVENTION of the data obj too!)
  static  Athena::PackageInfo packageInfo() {
    MY_STATIC_ASSERT(s_isDataObject);
    return Athena::PackageInfo("Package-00-00-00");
  }

  typedef std::false_type has_version_tag;
  typedef std::false_type has_classID_tag;
  static const int s_version = 0;

  // Is this is true, these types will automatically be made
  // const when added to StoreGate.
  static const bool s_isConst = false;
};

#undef MY_STATIC_ASSERT

#endif // not ATHENAKERNEL_CLASSID_TRAITS_H
