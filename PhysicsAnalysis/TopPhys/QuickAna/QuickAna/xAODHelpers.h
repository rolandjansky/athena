/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__X_AOD_HELPERS_H
#define QUICK_ANA__X_AOD_HELPERS_H


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
