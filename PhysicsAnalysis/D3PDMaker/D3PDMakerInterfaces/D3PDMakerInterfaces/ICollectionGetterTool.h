// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/ICollectionGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface to get a collection of objects and iterate over it.
 */

#ifndef D3PDMAKERINTERFACES_ICOLLECTIONGETTERTOOL_H
#define D3PDMAKERINTERFACES_ICOLLECTIONGETTERTOOL_H


#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "D3PDMakerInterfaces/IIteration.h"
#include "GaudiKernel/IAlgTool.h"


namespace D3PD {


/**
 * @brief Abstract interface to get a collection of objects and iterate over it.
 *
 * This extends the @c IObjGetterTool interface to allow iterating over
 * the objects within a container.
 *
 * The iteration is started by calling @c reset().  After that, call
 * #@c nextUntyped() repeatedly until it returns 0.
 */
class ICollectionGetterTool
  : virtual public IObjGetterTool,
    public IIteration
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(ICollectionGetterTool, 1, 0);


  /**
   * @brief Reset the iteration to the start of the collection.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return failure if the container cannot be retrieved.
   */
  virtual StatusCode reset (bool allowMissing = false) = 0;


  /**
   * @brief Return an estimate of the number of elements in the iteration.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * This can be used to pre-allocate memory.
   * (It's possible that this isn't known in advance of
   * iterating over the entire collection, for example
   * if a selection is being applied, so this is only a hint.)
   */
  virtual size_t sizeHint (bool allowMissing = false) = 0;


  /**
   * @brief Type-safe wrapper for @c next.
   *
   * Return the next element from the collection as a pointer to @c T.
   * Return 0 if the pointer can't be converted or at the end of the
   * iteration.
   *
   * This is implemented in terms of @c nextTypeinfo().
   */
  template <class T>
  const T* next();


  /**
   * @brief Test type compatibility.
   *
   * Test to see if the object being returned by the tool can be converted
   * to a pointer to @c T.  This can be used to perform type checks during job
   * initialization.
   *
   * This is implemented in terms of @c configureElementTypeinfo.
   */
  template <class T>
  StatusCode configureD3PD();


  /**
   * @brief Return the next object cast to a different pointer type.
   *
   * Return the next element from the collection
   * as a pointer to the @c ti type.
   * Return 0 if the pointer can't be converted or at the end of the
   * iteration.
   */
  virtual const void* nextTypeinfo (const std::type_info& ti) = 0;


  /**
   * @brief Test type compatibility.
   * @param ti The desired type.
   *
   * Test to see if the object being returned by the tool can be converted
   * to a pointer to @c T.  This can be used to perform type checks during job
   * initialization.
   */
  virtual StatusCode configureElementTypeinfo (const std::type_info& ti) = 0;


  /**
   * @brief Type-safe wrapper for @c releaseElementUntyped.
   * @param p The element to release.
   *
   * Call this when you are done with the object returned by
   * @c next().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   *
   * This is implemented in terms of @c releaseElementTypeinfo().
   */
  template <class T>
  void releaseElement (const T* p);


  /**
   * @brief Release an element retrieved from the getter.
   * @param p The element to release.
   * @param ti The type of p.
   *
   * Call this when you are done with the element returned by
   * @c nextUntyped().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  void releaseElementTypeinfo (const void* p,
                               const std::type_info& ti);
};


} // namespace D3PD


#include "D3PDMakerInterfaces/ICollectionGetterTool.icc"


#endif // not D3PDMAKERINTERFACES_ICOLLECTIONGETTERTOOL_H
