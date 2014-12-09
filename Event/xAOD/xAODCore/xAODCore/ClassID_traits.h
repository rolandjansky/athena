// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClassID_traits.h 613558 2014-08-26 17:16:16Z krasznaa $
#ifndef XAODCORE_CLASSID_TRAITS_H
#define XAODCORE_CLASSID_TRAITS_H

/// @file xAODCore/ClassID_traits.h
/// @author Tomasz Bold <tomasz.bold@cern.ch>
///
/// @short File providing the ClassID_traits traits class
///
/// In Athena all the EDM classes are identified using an ATLAS specific
/// system that assigns "class IDs" to the different classes. We do this by
/// using the CLASS_DEF macro in the headers of the EDM classes like:
///
/// <code>
///    #include "xAODCore/CLASS_DEF.h"<br/>
///    CLASS_DEF( xAOD::ClassName, 123456, 1 )
/// </code>
///
/// So made association between the type and identifier is helpful in varous
/// places, trigger among them. It is used in there in order to navigate from
/// the type to the CLID and thanks to that, to RoIs.
///
/// Code to do it is <code>ClassID_traits<xAOD::ClassName>::ID()</code> and
/// <code>ClassID_traits<xAOD::ClassName>::typeName()</code>.
/// This functionality is provided below.
///
/// To help developers in figurng out that the CLASS_DEF has not been made for
/// a given class, a compile time error is emitted.
///
/// $Revision: 613558 $
/// $Date: 2014-08-26 19:16:16 +0200 (Tue, 26 Aug 2014) $

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>
#include <typeinfo>

/// The Class ID type
typedef uint32_t CLID;

/// Helper used in order to get the clear error message for the developer
template< class T >
struct ERROR_you_should_use_the_CLASS_DEF_macro_to_define_CLID_and_VERSION {};

/// Default, invalid implementation of ClassID_traits
///
/// The CLASS_DEF macro specialises this class in order to provide functional
/// descriptions of the EDM classes. This default class is non-functional on
/// purpose, to generate compile-time errors in case some code tries using
/// CLID information on a type that doesn't provide one.
///
/// @author Tomasz Bold <tomasz.bold@cern.ch>
///
/// $Revision: 613558 $
/// $Date: 2014-08-26 19:16:16 +0200 (Tue, 26 Aug 2014) $
///
template< class T >
struct ClassID_traits {

   /// Function returning a unique integer identifier for the selected type
   static CLID ID() {
      return ERROR_you_should_use_the_CLASS_DEF_macro_to_define_CLID_and_VERSION< T >::please_correct;
   }
   /// Function returning a user-readable type name for the selected type
   static const std::string& typeName() {
      return ERROR_you_should_use_the_CLASS_DEF_macro_to_define_CLID_and_VERSION< T >::please_correct;
   }
   /// Function returning a type identifier for the selected type
   static const std::type_info& typeId() {
      return ERROR_you_should_use_the_CLASS_DEF_macro_to_define_CLID_and_VERSION< T >::please_correct;
   }

}; // struct ClassID_traits

#endif // not XAODCORE_CLASSID_TRAITS_H
