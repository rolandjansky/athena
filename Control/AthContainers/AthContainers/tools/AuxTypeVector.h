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
#include "AthContainersInterfaces/IAuxSetOption.h"
#include "AthContainers/tools/AuxDataTraits.h"
#include "AthContainers/PackedContainer.h"
#include "CxxUtils/override.h"
#include <typeinfo>
#include <vector>
#include <algorithm>
#include <stdint.h>


#if __cplusplus > 201100
# include <type_traits>
namespace SG_STD_OR_BOOST = std;
#else
# include "boost/type_traits/is_base_of.hpp"
# include "boost/type_traits/is_arithmetic.hpp"
namespace SG_STD_OR_BOOST = boost;
#endif


namespace SG {


class AuxDataOption;


/**
 * @brief Implementation of @c IAuxTypeVector for specific types.
 *
 * The auxiliary data for a container is stored in a set of STL vectors,
 * one for each data item.  However, we want to allow storing arbitrary
 * types in these vectors.  Thus, we define this abstract interface
 * to operate on the vectors.  This template class provides the
 * concrete implementations of this interface.
 *
 * This class is initialized with a pointer to the actual vector.
 * For a version that holds the vector internally, see the derived
 * class @c AuxTypeVector.
 */
template <class T, class CONT = typename AuxDataTraits<T>::vector_type>
class AuxTypeVectorHolder
  : public IAuxTypeVector
{
public:
  /// Type of the STL vector used for storage.
  typedef CONT vector_type;

  /// Type that the user sees.
  typedef typename AuxDataTraits<T>::element_type element_type;

  /// Vector element type.
  typedef typename vector_type::value_type vector_value_type;
  
protected:
  /// 1 for the usual case of @c V being @c vector<T>.
  /// If @c V is @c vector<char>, then this is @c sizeof(T).
  static const int SCALE = sizeof(element_type) / sizeof(vector_value_type);


public:
  /**
   * @brief Constructor.
   * @param vecPtr Pointer to the object (of type @c CONT).
   * @param ownFlag If true, take ownership of the object.
   */
  AuxTypeVectorHolder (vector_type* vecPtr, bool ownFlag);
    

  /**
   * @brief Destructor.
   * If @c ownFlag was true, then delete the vector object.
   */
  ~AuxTypeVectorHolder();


  /**
   * @brief Copy constructor.
   * @param other Object to copy.
   */
  AuxTypeVectorHolder (const AuxTypeVectorHolder& other);


   /**
   * @brief Move constructor.
   * @param other Object to move.
   */
  AuxTypeVectorHolder (AuxTypeVectorHolder&& other);


  /**
   * @brief Assignment.
   */
  AuxTypeVectorHolder& operator= (const AuxTypeVectorHolder& other);
    

  /**
   * @brief Move assignment.
   */
  AuxTypeVectorHolder& operator= (AuxTypeVectorHolder&& other);


  /**
   * @brief Return a reference to the payload vector.
   */
  vector_type& vec();


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
   * @brief Return the type of the payload object for this instance.
   *
   * May be different from what we get from the registry; if packing
   * is used, for example.
   */
  virtual const std::type_info* objType() const ATH_OVERRIDE;


  /**
   * @brief Return the size of the vector.
   */
  virtual size_t size() const ATH_OVERRIDE;


  /**
   * @brief Change the size of the vector.
   * @param sz The new vector size.
   * Returns true if it is known that iterators have not been invalidated;
   * false otherwise.
   */
  virtual bool resize (size_t sz) ATH_OVERRIDE;


  /**
   * @brief Change the capacity of the vector.
   * @param sz The new vector capacity.
   */
  virtual void reserve (size_t sz) ATH_OVERRIDE;


  /**
   * @brief Make an option setting.
   * @param option The option to set.
   *
   * The interpretation of the option depends on the concrete class.
   *
   * Returns true if the option setting was successful; false otherwise.
   */
  virtual bool setOption (const AuxDataOption& option) ATH_OVERRIDE;


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
   * @brief Insert elements into the vector via move semantics.
   * @param pos The starting index of the insertion.
   * @param beg Start of the range of elements to insert.
   * @param end End of the range of elements to insert.
   *
   * @c beg and @c end define a range of container elements, with length
   * @c len defined by the difference of the pointers divided by the
   * element size.
   *
   * The size of the container will be increased by @c len, with the elements
   * starting at @c pos copied to @c pos+len.
   *
   * The contents of the @c beg:end range will then be moved to our vector
   * starting at @c pos.  This will be done via move semantics if possible;
   * otherwise, it will be done with a copy.
   *
   * Returns true if it is known that the vector's memory did not move,
   * false otherwise.
   */
  virtual bool insertMove (size_t pos, void* beg, void* end) override;


  /**
   * @brief Try to convert this aux vector to a @c PackedContainer.
   *
   * If successful, returns a newly-allocated @c IAuxTypeVector.
   * In this case, the contents of the vector will have been moved
   * to the new vector (and this object will be empty).
   *
   * Returns null on failure.
   */
  virtual IAuxTypeVector* toPacked() ATH_OVERRIDE;


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
  /**
   * @brief Helper for @c insertMove.
   * @param pos The starting index of the insertion.
   * @param beg Start of the range of elements to insert.
   * @param end End of the range of elements to insert.
   *
   * This does the actual move for POD types.
   */
  void insertMove1 (typename CONT::iterator pos,
                    element_type* beg,
                    element_type* end,
                    std::true_type);


  /**
   * @brief Helper for @c insertMove.
   * @param pos The starting index of the insertion.
   * @param beg Start of the range of elements to insert.
   * @param end End of the range of elements to insert.
   *
   * This does the actual move for non-POD types.
   */
  void insertMove1 (typename CONT::iterator pos,
                    element_type* beg,
                    element_type* end,
                    std::false_type);

  /// The contained vector.
  vector_type* m_vecPtr;

  /// True if we need to delete the object.
  bool m_ownFlag;
};


//**********************************************************************


/**
 * @brief Implementation of @c IAuxTypeVector holding a vector instance.
 *
 * This is a derived class of @c AuxTypeVectorHolder that holds the vector
 * instance as a member variable (and thus manages memory internally).
 */
template <class T, class CONT = typename AuxDataTraits<T>::vector_type>
class AuxTypeVector
  : public AuxTypeVectorHolder<T, CONT>
{
public:
  typedef AuxTypeVectorHolder<T, CONT> Base;
  typedef typename Base::vector_type vector_type;
  typedef typename Base::element_type element_type;
  typedef typename Base::vector_value_type vector_value_type;

  /**
   * @brief Constructor.  Makes a new vector.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   */
  AuxTypeVector (size_t size, size_t capacity);



  /**
   * @brief Copy constructor.
   */
  AuxTypeVector (const AuxTypeVector& other);


  /**
   * @brief Move constructor.
   */
  AuxTypeVector (AuxTypeVector&& other);


  /**
   * @brief Assignment.
   */
  AuxTypeVector& operator= (const AuxTypeVector& other);


  /**
   * @brief Move assignment.
   */
  AuxTypeVector& operator= (AuxTypeVector&& other);


  /**
   * @brief Make a copy of this vector.
   */
  virtual IAuxTypeVector* clone() const override;

  
private:
  /// The contained vector.
  vector_type m_vec;
};

 
} // namespace SG


#include "AthContainers/tools/AuxTypeVector.icc"


#endif // not ATHCONTAINERS_AUXTYPEVECTOR_H
