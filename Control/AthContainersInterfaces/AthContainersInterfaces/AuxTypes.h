// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxTypes.h 781615 2016-11-01 16:20:50Z ssnyder $
/**
 * @file AthContainersInterfaces/AuxTypes.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Basic definitions for auxiliary types.
 *
 * This defines types used to identify pieces of aux data.
 */


#ifndef ATHCONTAINERSINTERFACES_AUXTYPES_H
#define ATHCONTAINERSINTERFACES_AUXTYPES_H


#include <cstddef>
#include <unordered_set>


namespace SG {

/// Identifier for a particular aux data item.
typedef size_t auxid_t;

/// A set of aux data identifiers.
typedef std::unordered_set<size_t> auxid_set_t;

/// To signal no aux data item.
static const auxid_t null_auxid = static_cast<auxid_t> (-1);

/// Used as type_info when we read an aux data item but it doesn't
/// exist in the registry.
class AuxTypePlaceholder {};
   
} // namespace SG



#endif // not ATHCONTAINERSINTERFACES_AUXTYPES_H
