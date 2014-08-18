// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelAthenaPool/src/ElementLinkStreamer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Set up a streamer for @c ElementLinkBase to implement T->P conversion.
 *
 * On reading, we just use the usual @c TStreamerInfo machinery.
 * For writing, we make a copy of the object and call @c thin() on the copy;
 * this does toPersistent(), remapping, and thinning.
 *
 * For this to work, we need to prohibit splitting for @c ElementLinkBase.
 * To avoid a root bug, we also need to prohibit splitting for derived
 * @c ElementLink classes.
 */


#ifndef DATAMODELATHENAPOOL_ELEMENTLINKSTREAMER_H
#define DATAMODELATHENAPOOL_ELEMENTLINKSTREAMER_H


namespace DataModelAthenaPool {


/**
 * @brief Set up a streamer for @c ElementLinkBase to implement T->P conversion.
 */
void installElementLinkBaseStreamer();


} // namespace DataModelAthenaPool


#endif // not DATAMODELATHENAPOOL_ELEMENTLINKSTREAMER_H
