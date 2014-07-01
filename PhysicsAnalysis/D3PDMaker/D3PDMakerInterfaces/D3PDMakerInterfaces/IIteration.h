// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/IIteration.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Abstract interface for iterating over a set of things.
 */

#ifndef D3PDMAKERINTERFACES_IITERATION_H
#define D3PDMAKERINTERFACES_IITERATION_H


namespace D3PD {


/**
 * @brief Abstract interface for iterating over a set of things.
 *
 * There are a couple places where we provide an interface for iterating
 * over a set of objects; ICollectionGetterTool and IMultiAssociationTool
 * are two.  This interface collects from these the methods that are
 * common to the actual iteration.  Some of the core code involved
 * in iteration is non-trivial (such as wrapping variables in vectors)
 * and we'd like to use the same code for getters and associators.
 *
 * Note that generally the iteration must be initialized before
 * starting to call next*.  That's not covered by this interface;
 * it should be specified in a derived interface.
 *
 * Once you're done with an object, you should call releaseElement().
 * This will usually be a no-op, but it allows the getter to return
 * a dynamically-allocated object.
 */
class IIteration
{
public:
  /// This is a polymorphic class.
  virtual ~IIteration() {}


  /**
   * @brief Return the element type of the collection.
   *
   * I.e., @c nextUntyped returns a pointer to this type.
   */
  virtual const std::type_info& elementTypeinfo() const = 0;


  /**
   * @brief Return a pointer to the next element in the iteration.
   *
   * Return 0 when the collection has been exhausted.
   */
  virtual const void* nextUntyped() = 0;


  /**
   * @brief Release an object retrieved from the getter.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c nextUntyped().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseElementUntyped (const void* p);
};


} // namespace D3PD


#include "D3PDMakerInterfaces/IIteration.icc"


#endif // not D3PDMAKERINTERFACES_IITERATION_H
