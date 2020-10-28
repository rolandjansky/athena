// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CLASS_DEF.h 613558 2014-08-26 17:16:16Z krasznaa $
#ifndef XAODCORE_CLASS_DEF_H
#define XAODCORE_CLASS_DEF_H

/// @file xAODCore/CLASS_DEF.h
/// @author Scott Snyder <snyder@bnl.gov>
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// @short File providing the CLASS_DEF macro
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
/// In a ROOT analysis we need much less information about the classes in such
/// a ROOT-independent way. We mostly just make use of that information to
/// decode the trigger navigation EDM.
///
/// All xAOD packages should use this header for declaring class IDs for their
/// EDM classes instead of using AthenaKernel directly.
///
/// $Revision: 613558 $
/// $Date: 2014-08-26 19:16:16 +0200 (Tue, 26 Aug 2014) $

#ifdef XAOD_STANDALONE

// Include the ClassID_traits class:
#include "xAODCore/ClassID_traits.h"

// Present a dummy macro to ROOT 6's dictionary generator. Otherwise
// it becomes over-eager, and starts generating dictionaries for types
// that it should just ignore.
#ifdef __CLING__

/// Dummy version of the macro
#define CLASS_DEF( __type, __clid, __version )

#else // __CLING__

/// Macro defining a specialisation for the ClassID_traits type
#define CLASS_DEF( __type, __clid, __version )                          \
   template<>                                                           \
   struct ClassID_traits< __type > {                                    \
      static CLID ID() {                                                \
         return __clid;                                                 \
      }                                                                 \
      static const std::string& typeName() {                            \
         static std::string name( #__type );                            \
         return name;                                                   \
      }                                                                 \
      static const std::type_info& typeId() {                           \
         return typeid( __type );                                       \
      }                                                                 \
  };

#endif // __CLING__

#else // not XAOD_STANDALONE

// Pull in the proper macro definition
#include "AthenaKernel/CLASS_DEF.h"

#endif // not XAOD_STANDALONE
#endif // not XAODCORE_CLASS_DEF_H
