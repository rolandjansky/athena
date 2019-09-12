// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file AthContainers/tools/supportsThinning.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Metafunctions to test for thinning support.
 *
 * This file provides a couple metafunctions that can be used to select
 * the proper pool converter class for a given data object.
 *
 * @c xAODInterfaceSupportingThinning<T>() evaluates to @c true 
 * if @c T is an xAOD interface class (@c DataVector or standalone)
 * that has not been tagged with supportsThinning = false.
 * @c xAODStoreSupportingThinning() is the same for xAOD auxiliary
 * store objects.
 */


#ifndef ATHCONTAINERS_SUPPORTSTHINNING_H
#define ATHCONTAINERS_SUPPORTSTHINNING_H


#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxElement.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include <type_traits>


namespace SG {


/**
 * @brief Test for an xAOD interface class supporting thinning.
 *
 * Returns @c true if @c T derives from either @c AuxVectorBase
 * or @c AuxElement and @c T::supportsThinning is @c true.
 * (The enable_if's are to avoid getting undefined symbol errors
 * when @c T is not an xAOD object.)
 */
template <class T>
inline constexpr bool is_xAODInterfaceObject_v =
  std::is_base_of_v<SG::AuxVectorBase, T> || std::is_base_of_v<SG::AuxElement, T>;
template <class T,
          typename std::enable_if_t<is_xAODInterfaceObject_v<T> >* = nullptr>
bool constexpr xAODInterfaceSupportingThinning () { return T::supportsThinning; }
template <class T,
          typename std::enable_if_t<!is_xAODInterfaceObject_v<T> >* = nullptr>
bool constexpr xAODInterfaceSupportingThinning () { return false; }


/**
 * @brief Test for an xAOD auxiliary store class supporting thinning.
 *
 * Returns @c true if @c T derives from @c IAuxStore
 * and @c T::supportsThinning is @c true.
 * (The enable_if's are to avoid getting undefined symbol errors
 * when @c T is not an xAOD object.)
 */
template <class T>
inline constexpr bool is_xAODStoreObject_v =
  std::is_base_of_v<SG::IAuxStore, T>;
template <class T,
          typename std::enable_if_t<is_xAODStoreObject_v<T> >* = nullptr>
bool constexpr xAODStoreSupportingThinning () { return T::supportsThinning; }
template <class T,
          typename std::enable_if_t<!is_xAODStoreObject_v<T> >* = nullptr>
bool constexpr xAODStoreSupportingThinning () { return false; }


} // namespace SG


#endif // not ATHCONTAINERS_SUPPORTSTHINNING_H
