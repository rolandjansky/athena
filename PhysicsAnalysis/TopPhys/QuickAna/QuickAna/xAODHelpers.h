/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__X_AOD_HELPERS_H
#define QUICK_ANA__X_AOD_HELPERS_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <QuickAna/Global.h>

#include <QuickAna/xAODInclude.h>
// #include <xAODBase/IParticleContainer.h>
#include <string>

namespace ana
{
  /// \brief This namespace contains helper functions for dealing with
  /// xAOD objects via ObjectType and void* pointers.

  namespace xAODHelpers
  {
    /// \brief perform a shallow copy and register it under the given
    /// name in the store
    StatusCode
    makeShallow (StoreType& store, const std::string& name, ObjectType type,
		 void*& pointer, const std::string& inputName);

    /// \brief make a new object and register it under the given name
    /// in the store
    StatusCode
    makeNew (StoreType& store, const std::string& name, ObjectType type,
	     void*& pointer);

    /// \brief get the given type converted into an IParticleContainer
    xAOD::IParticleContainer *
    getParticles (void *pointer, ObjectType type);
  }
}

#endif
