/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__OUTPUT_UTILS_H
#define QUICK_ANA__OUTPUT_UTILS_H

/// This file contains helper functions for writing xAOD containers

// Infrastructure includes
#include <AsgTools/StatusCode.h>

// EDM includes
#include <xAODCore/AuxContainerBase.h>

namespace ana
{

  namespace OutputUtils
  {

    /// Write a container and the given aux container to the given store.
    /// This is the function that actually calls record and applies the
    /// naming convention of the aux container as (name + 'Aux.').
    template<class ContainerType, class AuxContainerType, class StoreType>
    StatusCode recordContainer(ContainerType* container,
                               AuxContainerType* auxContainer,
                               StoreType& store,
                               const std::string& name);

  } // namespace OutputUtils

} // namespace ana

#include <QuickAna/OutputUtils.icc>

#endif
