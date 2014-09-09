// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ELStreamer.h,v 1.2 2007-10-30 21:19:52 ssnyder Exp $

/**
 * @file AthenaROOTAccess/ELStreamer.h
 * @author scott snyder
 * @date July 2007
 * @brief Call the required hooks for reading @c ElementLink classes.
 */


#ifndef ATHENAROOTACCESS_ELSTREAMER_H
#define ATHENAROOTACCESS_ELSTREAMER_H


#include "TClassStreamer.h"
#include "TMethodCall.h"
class TClass;
class TBuffer;


namespace AthenaROOTAccess {


/**
 * @brief Call the required hooks for reading @c ElementLink classes.
 *
 * When an @c ElementLink class (that is not T/P separated) is read,
 * we need to arrange to call @c toTransient() and @c source() on it.
 * (The last is required to be sure the source cached in the EL
 * matches the branch in which it lives.  If we defer this, the EL
 * may be dereferenced when another branch is active.)
 *
 * We use a @c TClassStreamer to hook into reading the class.
 * We use @c TMethodCalll to actually make the calls.
 */
class ELStreamer
  : TClassStreamer
{
public:
  /**
   * @brief Constructor.
   * @param cl The class into which we should hook.
   *
   * This will hook the streamer into the given class.
   */
  ELStreamer (TClass* cl);

  /**
   * @brief Copy constructor.
   * @param other The object to copy.
   */
  ELStreamer (const ELStreamer& other);

  /**
   * @brief Destructor.
   */
  ~ELStreamer();

  /**
   * @brief Clone this object.
   */
  virtual TClassStreamer *Generate() const;

  /**
   * @brief Run the streamer.
   * @param b The buffer from which the object is being read.
   * @param objp Pointer to the object.
   */
  virtual void operator()(TBuffer &b, void *objp);


private:
  // Disallow this.
  ELStreamer& operator= (const ELStreamer&);

  /// The class for which we're a streamer.
  TClass* m_cl;

  /// For calling @c toTransient().
  TMethodCall m_metht;

  /// For calling @c source().
  TMethodCall m_meths;

  /// Any previous streamer.
  TClassStreamer* m_old;
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_ELSTREAMER_H
