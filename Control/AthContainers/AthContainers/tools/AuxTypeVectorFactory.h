// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/AuxTypeVectorFactory.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Factory objects that creates vectors using @c AuxTypeVector.
 */


#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"
#include "AthContainers/tools/AuxTypeVector.h"


#ifndef ATHCONTAINERS_AUXTYPEVECTORFACTORY_H
#define ATHCONTAINERS_AUXTYPEVECTORFACTORY_H


namespace SG {


/**
 * @brief Factory objects that creates vectors using @c AuxTypeVector.
 *
 * This is an implementation of @c IAuxTypeVectorFactory that makes
 * vectors using the @c AuxTypeVector implementation.
 */
template <class T>
class AuxTypeVectorFactory
  : public IAuxTypeVectorFactory
{
public:
  /**
   * @brief Create a vector object of this type.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   *
   * Returns a newly-allocated object.
   * FIXME: Should return a unique_ptr.
   */
  virtual IAuxTypeVector* create (size_t size, size_t capacity) const;


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
  virtual IAuxTypeVector* createFromData (void* data,
                                          bool isPacked,
                                          bool ownFlag) const;

  
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
                     const void* src,  size_t src_index) const;


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
                     void* b, size_t bindex) const;


  /**
   * @brief Clear an element within a vector (static method).
   * @param dst Pointer to the start of the vector's data.
   * @param dst_index Index of the element in the vector.
   */
  virtual void clear (void* dst, size_t dst_index) const;


  /**
   * @brief Return the size of an element of this vector type.
   */
  virtual size_t getEltSize() const;


  /**
   * @brief Return the @c type_info of the vector.
   */
  virtual const std::type_info* tiVec() const;


  /**
   * @brief True if the vectors created by this factory work by dynamic
   *        emulation (via @c TVirtualCollectionProxy or similar); false
   *        if the std::vector code is used directly.
   */
  virtual bool isDynamic() const;


private:
  /// Helpers for creating vector from a data blob,
  IAuxTypeVector* createFromData (void* data, bool isPacked, bool ownFlag,
                                  std::true_type) const;
  IAuxTypeVector* createFromData (void* data, bool isPacked, bool ownFlag,
                                  std::false_type) const;
};


} // namespace SG


#include "AthContainers/tools/AuxTypeVectorFactory.icc"


#endif // not ATHCONTAINERS_AUXTYPEVECTORFACTORY_H
