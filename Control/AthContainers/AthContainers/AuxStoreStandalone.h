// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxStoreStandalone.h 597621 2014-05-18 04:19:29Z ssnyder $
/**
 * @file AthContainers/AuxStoreStandalone.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2013
 * @brief Auxiliary data store for standalone objects.
 */


#ifndef ATHCONTAINERS_AUXSTORESTANDALONE_H
#define ATHCONTAINERS_AUXSTORESTANDALONE_H


#include "AthContainers/AuxStoreInternal.h"


namespace SG {


/**
 * @brief Auxiliary data store for standalone objects.
 *
 * This is a variant of @c AuxStoreInternal (from which it derives),
 * meant to be used with standalone objects.  (That is, objects
 * that have aux data but are not in a container; this includes
 * objects with a private store.)
 *
 * The difference from @c AuxStoreInternal is what we return for
 * the @c IAuxStoreIO methods.  For @c getIOData, we return a pointer
 * to the start of the vector data, rather than the vector itself; and
 * for @c getIOType, we return the type itself rather than the vector type.
 */
class AuxStoreStandalone
  : public AuxStoreInternal
{
public:
  /**
   * @brief Constructor.
   */
  AuxStoreStandalone();
};


} // namespace SG


#endif // not ATHCONTAINERS_AUXSTORESTANDALONE_H
