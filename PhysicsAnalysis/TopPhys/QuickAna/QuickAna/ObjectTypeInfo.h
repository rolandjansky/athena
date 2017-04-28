/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__OBJECT_TYPE_INFO_H
#define QUICK_ANA__OBJECT_TYPE_INFO_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <QuickAna/Global.h>

#include <string>

namespace ana
{
  /// \brief This class contains various static information about the
  /// different object types.
  ///
  /// This is mostly to help in using ObjectType inside templates, to
  /// match them to the right objects, etc.

  struct ObjectTypeInfo
  {
    /// \brief the number of object types available
    static const unsigned numTypes = OBJECT_OVERLAP_REMOVAL;

    /// \brief the number of object types available as definitions
    static const unsigned numDefTypes = OBJECT_NONE + 1;


    /// \brief the string name of the type
    static const std::string name [numDefTypes];


    /// \brief whether this object type supports an IParticle interface
    static bool supportsIParticle (ObjectType type);


    /// \brief the actual object type associated with this
    template<unsigned> struct asXAOD;


    /// \brief the object types supported for a given xAOD type
    template<class T> struct fromXAOD;
  };
}

#include <QuickAna/ObjectTypeInfo.icc>

#endif
