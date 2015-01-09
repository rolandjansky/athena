// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVCollectionProxy.h,v 1.7 2008-12-23 05:01:34 ssnyder Exp $

/**
 * @file AthenaROOTAccess/DVCollectionProxy.h
 * @author scott snyder
 * @date April 2007
 * @brief A Root collection proxy for @c DataVector containers.
 */


#ifndef DVCOLLECTIONPROXY_H
#define DVCOLLECTIONPROXY_H


#include "TGenCollectionProxy.h"
#include "TMethodCall.h"


namespace AthenaROOTAccess {


/// @brief Dummy class to use as the @c DataVector template argument
/// for the class we give to the Root proxy.
class DVCollectionProxyDummy {};


/**
 * @brief A Root collection proxy for @c DataVector containers.
 *
 * In order for a container type to be treated properly by @c TTree::Draw
 * and friends, Root must recognize that it is in fact a container type,
 * and generate an appropriate proxy for it.  This doesn't happen
 * automatically, though.  This class serves as an appropriate proxy
 * for @c DataVector classes.  Like the Root proxy classes, it's based
 * on making a hard cast to a dummy @c DataVector type.  But because
 * @c DataVector's can inherit, we need to be careful about pointer
 * conversions, something the Root proxy classes could ignore.
 * We extend the Root environment buffer to cache information
 * about the pointer offsets.
 */
class DVCollectionProxy
  : public TGenCollectionProxy
{
public:
  /**
   * @brief Constructor.
   * @param elttype The name of the contained type (including a trailing '*').
   * @param conttype The name of the container type we're proxying.
   *                 Note that this may be different from
   *                 a @c DataVector of @a elttype for classes which
   *                 derive from @c DataVector.
   */
  DVCollectionProxy (const char* elttype,
                     const char* conttype);


  /**
   * @brief Copy constructor.
   * @param rhs The proxy to copy.
   */
  DVCollectionProxy (const DVCollectionProxy& rhs);


  /**
   * @brief Start working with a new collection.
   * @param objstart The address of the collection.
   */
  virtual void PushProxy(void *objstart);


  /**
   * @brief Clone this object.
   */
  virtual TVirtualCollectionProxy* Generate() const;


private:
  /**
   * @brief Initialize the cached pointer offsets.
   */
  void FindOffsets (const char* elttype,
                    const char* conttype);

  /// The offset of the underlying @c DataVector in the declared type.
  off_t m_contoff;

  /// Method to cast from the DV representation of an element
  /// to the declared element type.
  TMethodCall m_caster;

  // Disallow copying.
  DVCollectionProxy& operator= (const DVCollectionProxy&);
};


} // namespace AthenaROOTAccess


#endif // not DVCOLLECTIONPROXY_H
