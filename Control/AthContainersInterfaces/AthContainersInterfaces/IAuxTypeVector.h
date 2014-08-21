// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IAuxTypeVector.h 612955 2014-08-21 19:25:10Z ssnyder $
/**
 * @file AthContainersInterfaces/IAuxTypeVector.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Abstract interface for manipulating vectors of arbitrary types.
 */


#ifndef ATHCONTAINERSINTERFACES_IAUXTYPEVECTOR_H
#define ATHCONTAINERSINTERFACES_IAUXTYPEVECTOR_H


#include <cstddef>


namespace SG {


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
   */
  virtual void resize (size_t sz) = 0;


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
};


} // namespace SG


#endif // not ATHCONTAINERSINTERFACES_IAUXTYPEVECTOR_H
