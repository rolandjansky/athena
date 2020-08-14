// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  AthenaKernel/tools/safe_clid.h
 * @author scott snyder
 * @date Nov 2005
 * @brief Find the class ID of a type, without triggering an error
 *        if it isn't defined.
 *
 * The general interface for finding the class ID for some type @a T
 * is to use @a ClassID_traits<T>::ID().  However, if no class ID
 * has been defined for @a T, this will result in a compilation error.
 * In some cases, where the class ID can be considered optional, this
 * can severely restrict the applicability of generic code.
 *
 * This header provides @a SG::safe_clid<T>(), which should return
 * the class ID of @a T, or @a CLID_NULL if @a T does not have a defined
 * class ID (without causing an error).
 *
 * The implementation relies on the fact that when the class ID traits
 * class gets specialized for a specific class ID, it defines
 * @a has_classID_tag.  The @a CLASS_DEF macro makes this definition,
 * but if you specialize @a ClassID_traits yourself, you must remember
 * to include @a has_classID_tag, or this function won't work.
 */

#ifndef ATHENAKERNEL_SAFECLID_H
#define ATHENAKERNEL_SAFECLID_H


#include "GaudiKernel/ClassID.h"


namespace SG {


/**
 * @brief Return the class ID of @a T, or @a CLID_NULL if it doesn't
 *        have one.  Avoids compilation errors if the class ID
 *        is not defined.
 */
template <class T>
constexpr CLID safe_clid();


} // namespace SG


#include "AthenaKernel/tools/safe_clid.icc"


#endif // not ATHENAKERNEL_SAFECLID_H
