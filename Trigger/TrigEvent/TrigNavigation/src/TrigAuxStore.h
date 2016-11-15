// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigNavigation/src/TrigAuxStore.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Aux store instance used to hold dynamic variables deserialized
 *        from the bytestream.
 */


#ifndef TRIGNAVIGATION_TRIGAUXSTORE_H
#define TRIGNAVIGATION_TRIGAUXSTORE_H


#include "AthContainers/AuxStoreInternal.h"


namespace HLTNavDetails {


/**
 * @brief Aux store instance used to hold dynamic variables deserialized
 *        from the bytestream.
 *
 * This is just AuxStoreInternal except that we make the @c addVector
 * method available to allow adding vector instances to the store.
 */
class TrigAuxStore
  : public SG::AuxStoreInternal
{
public:
  TrigAuxStore() {}

  using SG::AuxStoreInternal::addVector;
};


} // namespace HLTNavDetails


#endif // not TRIGNAVIGATION_TRIGAUXSTORE_H
