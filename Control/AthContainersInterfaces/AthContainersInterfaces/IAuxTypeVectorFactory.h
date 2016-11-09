// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersInterfaces/IAuxTypeVectorFactory.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Interface for factory objects that create vectors.
 */


#ifndef ATHCONTAINERSINTERFACES_IAUXTYPEVECTORFACTORY_H
#define ATHCONTAINERSINTERFACES_IAUXTYPEVECTORFACTORY_H


#include <cstddef>
#include <typeinfo>
#include <cstdlib>


namespace SG {


class IAuxTypeVector;


/**
 * @brief Interface for factory objects that create vectors.
 *
 * The auxiliary data for a container are stored in a set of STL vectors,
 * one for each data item.  However, we want to allow storing arbitrary
 * types in these vectors.  Thus, we define an abstract interface
 * to operate on the vectors, @c IAuxTypeVector.
 *
 * Now, we need to registry of how to create an appropriate
 * @c IAuxTypeVector for a given @c std::type_info.  To make that
 * easier, we encapsulate the creation of those vector objects
 * using this factory interface.  There will be one instance of this
 * for each vector type that we deal with.  Usually, this interface
 * will be implemented by @c AuxTypeVector; however, other implementations
 * are used, for example, for reading auxiliary data from root when
 * we don't know its type at compile-time.
 */
class IAuxTypeVectorFactory
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~IAuxTypeVectorFactory() {}


  /**
   * @brief Create a vector object of this type.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   *
   * Returns a newly-allocated object.
   * FIXME: Should return a unique_ptr.
   */
  virtual IAuxTypeVector* create (size_t size, size_t capacity) const = 0;


  /**
   * @brief Create a vector object of this type from a data blob.
   * @param data The vector object.
   * @param isPacked If true, @c data is a @c PackedContainer.
   * @param ownFlag If true, the newly-created IAuxTypeVector object
   *                will take ownership of @c data.
   *
   * If the element type is T, then @c data should be a pointer
   * to a std::vector<T> object, which was obtained with @c new.
   * But if @c isPacked is @c true, then @c data
   * should instead point at an object of type @c SG::PackedContainer<T>.
   *
   * Returns a newly-allocated object.
   * FIXME: Should return a unique_ptr.
   */
  virtual IAuxTypeVector* createFromData (void* /*data*/,
                                          bool /*isPacked*/,
                                          bool /*ownFlag*/) const
  { std::abort(); }


  /**
   * @brief Copy an element between vectors.
   * @param dst Pointer to the start of the destination vector's data.
   * @param dst_index Index of destination element in the vector.
   * @param src Pointer to the start of the source vector's data.
   * @param src_index Index of source element in the vector.
   *
   * @c dst and @ src can be either the same or different.
   */
  virtual void copy (void* dst,        size_t dst_index,
                     const void* src,  size_t src_index) const = 0;


  /**
   * @brief Swap an element between vectors.
   * @param a Pointer to the start of the first vector's data.
   * @param aindex Index of the element in the first vector.
   * @param b Pointer to the start of the second vector's data.
   * @param bindex Index of the element in the second vector.
   *
   * @c a and @ b can be either the same or different.
   */
  virtual void swap (void* a, size_t aindex,
                     void* b, size_t bindex) const = 0;


  /**
   * @brief Clear an element within a vector (static method).
   * @param dst Pointer to the start of the vector's data.
   * @param dst_index Index of the element in the vector.
   */
  virtual void clear (void* dst, size_t dst_index) const = 0;


  /**
   * @brief Return the size of an element of this vector type.
   */
  virtual size_t getEltSize() const = 0;


  /**
   * @brief Return the @c type_info of the vector.
   */
  virtual const std::type_info* tiVec() const = 0;


  /**
   * @brief True if the vectors created by this factory work by dynamic
   *        emulation (via @c TVirtualCollectionProxy or similar); false
   *        if the std::vector code is used directly.
   */
  virtual bool isDynamic() const = 0;
};


} // namespace SG


#endif // not ATHCONTAINERSINTERFACES_IAUXTYPEVECTORFACTORY_H
