// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/ILockable.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Interface to allow an object to lock itself when made const in SG.
 */


#ifndef ATHENAKERNEL_ILOCKABLE_H
#define ATHENAKERNEL_ILOCKABLE_H


/**
 * @brief Interface to allow an object to lock itself when made const in SG.
 *
 * An object recorded in StoreGate can be `locked' by a call to
 * @c StoreGateSvc::setConst().  Normally, this just sets a flag in the
 * @c DataProxy, which tells StoreGate not to allow non-const retrieves.
 * However, sometimes the object itself should know about this; for example,
 * when a container with an auxiliary store is locked.  If an object
 * in StoreGate derives from @c ILockable, then a setConst() operation
 * will call @c lock() on the object.
 */
class ILockable
{
public:
  /// Destructor.
  virtual ~ILockable() {}


  /**
   * @brief Lock this object.
   *
   * When an object in StoreGate is locked via @c StoreGateSvc::setConst,
   * StoreGate will call this method on the object, provided the object
   * derives from @c ILockable.
   */
  virtual void lock() = 0;
};



#endif // not ATHENAKERNEL_ILOCKABLE_H
