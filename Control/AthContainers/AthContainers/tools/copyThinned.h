// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthContainers/tools/copyThinned.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Helper to copy an object while applying thinning.
 *
 * The code in this file handles thinning for @c DataVector and
 * @c IAuxStore objects.  Support for other types may be added by
 * providing additional overloads for @c copyThinned1.
 *
 * There are two variants: @c copyThinned and @ copyThinnedConst.  The former returns
 * a non-const pointer, and the latter returns a const pointer.
 * Only the const variety may be used with a @c DataVector instance.
 *
 * In the signature of @c copyThinned1, the container is passed twice:
 * once as a reference and once as a pointer.  This allows one to use
 * overload resolution on the pointer argument to select the proper
 * overload, while still being able to get the derived type of the container
 * via template binding to the first argument.  See the declarations
 * below for concrete examples.
 */


#ifndef ATHCONTAINERS_COPYTHINNED_H
#define ATHCONTAINERS_COPYTHINNED_H


#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthenaKernel/ThinningDecisionBase.h"


namespace SG {


/**
 * @brief Helper to copy an object while applying thinning.
 * @param orig The object to copy.
 * @param info Thinning information for this object (or nullptr).
 *
 * Returns a new copy of @c orig with elements removed according to the
 * thinning defined in @c svc.  Ownership of the new object is passed
 * back to the caller.
 *
 * The code here handles @c IAuxStore objects.
 * (For a @c DataVector object, use @c copyThinnedConst.)
 * Support for additional object types may be added by adding
 * overloads for @c copyThinned1.
 */
template <class CONTAINER>
std::unique_ptr<CONTAINER>
copyThinned (CONTAINER& orig,
             const SG::ThinningInfo* info);


/**
 * @brief Helper to copy an object while applying thinning, const version.
 * @param orig The object to copy.
 * @param info Thinning information for this object (or nullptr).
 *
 * Returns a new copy of @c orig with elements removed according to the
 * thinning defined in @c svc.  Ownership of the new object is passed
 * back to the caller.
 *
 * The code here handles @c DataVector and @c IAuxStore objects.
 * Support for additional object types may be added by adding
 * overloads for @c copyThinned1.
 */
template <class CONTAINER>
std::unique_ptr<const CONTAINER>
copyThinnedConst (const CONTAINER& orig,
                  const SG::ThinningInfo* info);


/**
 * @brief Helper to copy an object while applying thinning.
 * @param orig The object to copy.
 * @param dummy Dummy argument for overload resolution.
 * @param info Thinning information for this object (or nullptr).
 *
 * This is the generic version of @c copyThinned, which matches types
 * for which there is not a more specific overload.  It simply makes
 * a copy of @c orig using the copy constructor.
 */
template <class CONTAINER>
std::unique_ptr<CONTAINER>
copyThinned1 (const CONTAINER& orig,
              const void* dummy,
              const SG::ThinningInfo* info);


/**
 * @brief Helper to copy an object while applying thinning.
 * @param orig The object to copy.
 * @param dummy Dummy argument for overload resolution.
 * @param info Thinning information for this object (or nullptr).
 *
 * This overload handles @c DataVector types.  It returns a view container
 * copy of @c orig, from which any thinned elements are removed.
 */
template <class CONTAINER>
std::unique_ptr<CONTAINER>
copyThinned1 (CONTAINER& orig,
              DataVector<typename CONTAINER::base_value_type>* /*dummy*/,
              const SG::ThinningInfo* info);


/**
 * @brief Helper to copy an object while applying thinning.
 * @param orig The object to copy.
 * @param dummy Dummy argument for overload resolution.
 * @param info Thinning information for this object (or nullptr).
 *
 * This overload handles @c DataVector types.  It returns a view container
 * copy of @c orig, from which any thinned elements are removed.
 */
template <class CONTAINER>
std::unique_ptr<const CONTAINER>
copyThinned1 (const CONTAINER& orig,
              const DataVector<typename CONTAINER::base_value_type>* dummy,
              const SG::ThinningInfo* info);


/**
 * @brief Helper to copy an object while applying thinning.
 * @param orig The object to copy.
 * @param dummy Dummy argument for overload resolution.
 * @param info Thinning information for this object (or nullptr).
 *
 * This overload handles @c IAuxStore types.  It returns a new copy
 * of the store, with any thinned elements removed.
 */
template <class CONTAINER>
std::unique_ptr<CONTAINER>
copyThinned1 (const CONTAINER& orig,
              const SG::IAuxStore* dummy,
              const SG::ThinningInfo* info);


} // namespace SG


#include "AthContainers/tools/copyThinned.icc"


#endif // not ATHCONTAINERS_COPYTHINNED_H
