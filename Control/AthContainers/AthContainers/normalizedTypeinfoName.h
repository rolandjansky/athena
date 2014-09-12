// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/normalizedTypeinfoName.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Convert a @c type_info to a normalized string representation
 *        (matching the names used in the root dictionary).
 */


#ifndef ATHCONTAINERS_NORMALIZEDTYPEINFONAME_H
#define ATHCONTAINERS_NORMALIZEDTYPEINFONAME_H


#include <string>
#include <typeinfo>


namespace SG {


/**
 * @brief Convert a @c type_info to a normalized string representation
 *        (matching the names used in the root dictionary).
 * @param info The type to convert.
 *
 * The function `AthContainer_detail::typeinfoName` may be used to convert
 * a C++ `type_info` to a string representing the name of the class; this
 * handles platform-dependent details such as performing demangling.
 *
 * However, the name you get as a result of this does not necessarily match
 * the name by which the class is known in the ROOT dictionary.
 * In particular, defaulted template arguments for STL containers and
 * @c DataVector are suppressed in the dictionary.  So, for example,
 * for a vector class @c typeinfoName may produce
 * `std::vector<int, std::allocator<T> >`, while in the dictionary
 * it is known as `std::vector<int>`.  Using @c normalizedTypeinfoName
 * instead will transform the names to match what's in the dictionary.
 * This function will also cache the typeinfo -> string conversions.
 */
std::string normalizedTypeinfoName (const std::type_info& info);


}


#endif // not ATHCONTAINERS_NORMALIZEDTYPEINFONAME_H
