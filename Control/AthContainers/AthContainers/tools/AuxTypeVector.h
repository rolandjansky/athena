// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/AuxTypeVector.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Implementation of @c IAuxTypeVector for specific types.
 */


#ifndef ATHCONTAINERS_AUXTYPEVECTOR_H
#define ATHCONTAINERS_AUXTYPEVECTOR_H


#include "AthContainersInterfaces/IAuxTypeVector.h"
#include "CxxUtils/override.h"
#include <vector>
#include <algorithm>


namespace SG {


/**
 * @brief Implementation of @c IAuxTypeVector for specific types.
 *
 * The auxiliary data for a container is stored in a set of STL vectors,
 * one for each data item.  However, we want to allow storing arbitrary
 * types in these vectors.  Thus, we define this abstract interface
 * to operate on the vectors.  This template class provides the
 * concrete implementations of this interface.
 *
 * This base class of @c AuxTypeVector allows specifying independently
 * the payload type and the vector type.  This is needed to handle
 * @c vector<bool> (for which we need to actually store @c vector<char>).
 */
template <class T, class V>
class AuxTypeVector1
  : public IAuxTypeVector
{
private:
  /// 1 for the usual case of @c V being @c vector<T>.
  /// If @c V is @c vector<char>, then this is @c sizeof(T).
  static const int SCALE = sizeof(T) / sizeof(typename V::value_type);


public:
  /// Type of the STL vector used for storage.
  typedef V vector_type;


  /**
   * @brief Constructor.  Makes a new vector.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   */
  AuxTypeVector1 (size_t size, size_t capacity);
    

  /**
   * @brief Make a copy of this vector.
   */
  virtual IAuxTypeVector* clone() const ATH_OVERRIDE;


  /**
   * @brief Return a pointer to the start of the vector's data.
   */
  virtual void* toPtr() ATH_OVERRIDE;


  /**
   * @brief Return a pointer to the STL vector itself.
   */
  virtual void* toVector() ATH_OVERRIDE;


  /**
   * @brief Return the size of the vector.
   */
  virtual size_t size() const ATH_OVERRIDE;


  /**
   * @brief Change the size of the vector.
   * @param sz The new vector size.
   */
  virtual void resize (size_t sz) ATH_OVERRIDE;


  /**
   * @brief Change the capacity of the vector.
   * @param sz The new vector capacity.
   */
  virtual void reserve (size_t sz) ATH_OVERRIDE;


  /**
   * @brief Shift the elements of the vector.
   * @param pos The starting index for the shift.
   * @param offs The (signed) amount of the shift.
   *
   * This operation shifts the elements in the vectors for all
   * aux data items, to implement an insertion or deletion.
   * @c offs may be either positive or negative.
   *
   * If @c offs is positive, then the container is growing.
   * The container size should be increased by @c offs,
   * the element at @c pos moved to @c pos + @c offs,
   * and similarly for following elements.
   * The elements between @c pos and @c pos + @c offs should
   * be default-initialized.
   *
   * If @c offs is negative, then the container is shrinking.
   * The element at @c pos should be moved to @c pos + @c offs,
   * and similarly for following elements.
   * The container should then be shrunk by @c -offs elements
   * (running destructors as appropriate).
   */
  virtual void shift (size_t pos, ptrdiff_t offs) ATH_OVERRIDE;


  /**
   * @brief Copy an element between vectors (static method).
   * @param dst Pointer to the start of the destination vector's data.
   * @param dst_index Index of destination element in the vector.
   * @param src Pointer to the start of the source vector's data.
   * @param src_index Index of source element in the vector.
   *
   * @c dst and @ src can be either the same or different.
   */
  static void copy (void* dst,        size_t dst_index,
                    const void* src,  size_t src_index);


  /**
   * @brief Swap an element between vectors (static method).
   * @param a Pointer to the start of the first vector's data.
   * @param aindex Index of the element in the first vector.
   * @param b Pointer to the start of the second vector's data.
   * @param bindex Index of the element in the second vector.
   *
   * @c a and @ b can be either the same or different.
   */
  static void swap (void* a, size_t aindex,
                    void* b, size_t bindex);


  /**
   * @brief Clear an element within a vector (static method).
   * @param dst Pointer to the start of the vector's data.
   * @param dst_index Index of the element in the vector.
   */
  static void clear (void* dst, size_t dst_index);


private:
  /// The contained vector.
  vector_type m_vec;
};


/**
 * @brief Implementation of @c IAuxTypeVector for specific types.
 *
 * Generic case.
 */
template <class T>
class AuxTypeVector
  : public AuxTypeVector1<T, std::vector<T> >
{
public:
  /**
   * @brief Constructor.  Makes a new vector.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   */
  AuxTypeVector (size_t size, size_t capacity);
};


/**
 * @brief Implementation of @c IAuxTypeVector for specific types.
 *
 * bool case.
 */
template <>
class AuxTypeVector<bool>
  : public AuxTypeVector1<bool, std::vector<char> >
{
public:
  /**
   * @brief Constructor.  Makes a new vector.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   */
  AuxTypeVector (size_t size, size_t capacity);
};


} // namespace SG


#include "AthContainers/tools/AuxTypeVector.icc"


#endif // not ATHCONTAINERS_AUXTYPEVECTOR_H
