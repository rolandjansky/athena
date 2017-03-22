// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IAuxTypeVector.h 797203 2017-02-14 19:35:56Z ssnyder $
/**
 * @file AthContainersInterfaces/IAuxTypeVector.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Abstract interface for manipulating vectors of arbitrary types.
 */


#ifndef ATHCONTAINERSINTERFACES_IAUXTYPEVECTOR_H
#define ATHCONTAINERSINTERFACES_IAUXTYPEVECTOR_H


#include <cstddef>
#include <typeinfo>


namespace SG {


class AuxDataOption;


/**
 * @brief Abstract interface for manipulating vectors of arbitrary types.
 *
 * The auxiliary data for a container are stored in a set of STL vectors,
 * one for each data item.  However, we want to allow storing arbitrary
 * types in these vectors.  Thus, we define this abstract interface
 * to operate on the vectors.  The concrete version of this will
 * own one vector.
 */
class IAuxTypeVector
{
public:
  /// Destructor.
  virtual ~IAuxTypeVector() {}


  /**
   * @brief Make a copy of this vector.
   */
  virtual IAuxTypeVector* clone() const = 0;


  /**
   * @brief Return a pointer to the start of the vector's data.
   */
  virtual void* toPtr() = 0;


  /**
   * @brief Return a pointer to the STL vector itself.
   */
  virtual void* toVector() = 0;


  /**
   * @brief Return the size of the vector.
   */
  virtual size_t size() const = 0;


  /**
   * @brief Change the size of the vector.
   * @param sz The new vector size.
   * Returns true if it is known that iterators have not been invalidated;
   * false otherwise.
   */
  virtual bool resize (size_t sz) = 0;


  /**
   * @brief Change the capacity of the vector.
   * @param sz The new vector capacity.
   */
  virtual void reserve (size_t sz) = 0;


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
  virtual void shift (size_t pos, ptrdiff_t offs) = 0;


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
  virtual bool insertMove (size_t pos, void* beg, void* end) = 0;


  /**
   * @brief Set an option for this variable.
   * @param option The option to set.
   *
   * The interpretation of the option depends on the particular representation
   * of the variable provided by the concrete class.
   *
   * Returns true if the option setting was successful; false otherwise.
   */
  virtual bool setOption (const AuxDataOption& /*option*/)
  { return false; }


  /**
   * @brief Make a packed version of the variable.
   *
   * If possible, return a new vector object that stores the data
   * in a @c PackedContainer.  The data itself should be _moved_
   * to the new container (so that this vector becomes empty).
   * This ensures that pointers to the data are preserved.
   *
   * If successful, a newly-allocated object is returned.
   * A null pointer is returned on failure (operation not supported,
   * type can't be packed, type is already packed).
   */
  virtual IAuxTypeVector* toPacked() { return 0; }


  /**
   * @brief Return the type of the complete object to be saved.
   *
   * For example, if the object is a @c std::vector, then we return
   * the @c type_info of the vector.  But if we're holding
   * a @c PackedContainer, then we return the @c type_info of the
   * @c PackedContainer.
   *
   * Can return null if the operation is not supported.  In that case,
   * I/O will use the type found from the variable registry.
   */
  virtual const std::type_info* objType() const { return 0; }
};


} // namespace SG


#endif // not ATHCONTAINERSINTERFACES_IAUXTYPEVECTOR_H
