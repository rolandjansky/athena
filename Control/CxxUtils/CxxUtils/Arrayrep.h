// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  Arrayrep.h
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Representation class for Array's.
 */

#ifndef CXXUTILS_ARRAYREP_H
#define CXXUTILS_ARRAYREP_H


#include <string>
#include <vector>


// Presently has to be in CaloRec ns for backwards compatibility.
namespace CaloRec {


/// The type of an element of an @c Array.
typedef float Arrayelt;


/**
 * @class Arrayrep
 * @brief Representation class for Array's.
 *
 * This class is used for the implementation of multidimensional
 * array constants.  The user interface is provided by the
 * @c Array template class.  Given an @c Array<N>  @c a, we want
 * @c a[i] to be an @c Array<N-1>.  However, we don't want to have
 * to copy the array data.  Thus, the @c Array class just holds
 * a reference to the actual array data.  Those data are stored
 * in an instance of @c Arrayrep.
 *
 * An @c Arrayrep is defined by two vectors: one giving all the
 * array elements, and the second giving the shape of the array.
 * (A shape is a vector with one integer per array dimension,
 * the integers giving the size of the dimensions.)
 *
 * We maintain one additional array as an optimization.
 * @c m_sizes[0] contains the total size in elements
 * of the array resulting from a single indexing operation.
 * @c m_sizes[1] contains the size of the array resulting from
 * two indexing operations, and so on.  @c m_sizes has the same
 * length as @c m_shape, but the last element is always 1.
 * The function @c init_sizes will initialize the @c m_sizes
 * array from the contents of @c m_shape.
 *
 * A scalar (0-dimensional array) is represented by @c m_shape (and
 * @c m_sizes) being empty, and @c m_data having a single element.
 *
 * If both @c m_shape and @c m_data are empty, then the representation
 * is uninitialized.
 */
struct Arrayrep
{
  /**
   * @brief Default constructor.
   *
   * This makes an uninitialized @c Arrayrep.
   */
  Arrayrep () {}

  /**
   * @brief Construct from a string.
   * @param str The string to convert.
   * @param context An optional string to use for error reporting.
   *
   * Parse the string and initialize the array.
   * This string should be like `[[1, 2], [3, 4]]'.
   */
  explicit Arrayrep (const std::string& str,
                     const std::string& context = "");

  /**
   * @brief Construct an empty array of a given shape.
   * @param shape The shape of the array.
   *
   * Initialize an array of a given shape.
   * The array will contain all 0's.
   */
  explicit Arrayrep (const std::vector<unsigned int>& shape);

  /**
   * @brief Construct an empty array of a given shape.
   * @param shape The shape of the array.
   * @param n     The length of the @a shape array.
   *
   * Initialize an array of a given shape.
   * The array will contain all 0's.
   * This version is more convenient to call with a constant shape.
   */
  explicit Arrayrep (const unsigned int shape[], unsigned int n);

  /// The array data, stored using the C array ordering.
  std::vector<Arrayelt> m_data;

  /// The array shape.
  /// One entry per dimension, giving the size of each dimension.
  std::vector<unsigned int> m_shape;

  /// Subarray sizes, for faster access.  See above.
  /// This member could be considered transient.
  std::vector<unsigned int> m_sizes;

  /**
   * @brief Initialize the @c m_sizes vector from the @c m_shape vector.
   * @param resize_data Should @c m_data be resized appropriately?
   *
   * The contents of the @c m_sizes vector are initialized from
   * the contents of the @c m_shape vector.  If @c resize_data
   * is true, then the size of @c m_data is changed to the total
   * size indicated by @c m_shape.  Otherwise, we verify that
   * @c m_data has the correct size, and raise an assertion if not.
   */
  void init_sizes (bool resize_data = false);

  /**
   * @brief Creates a text representation of the array content.
   * @param std::ostream where the text should be written
   *
   * Writes the content of the array to a ostream. The sub-arrays are
   * enclosed by square-brackets and separated by commas.
   */

  void write_array(std::ostream& stream) const;

  /**
   * @brief Helper function for write_array.
   * @param stream where the array should be written
   * @param idx Current index in @c m_data
   * @param dimIndex Current index in @c m_shapes
   *
   * Calls itself recursively with dimIndex-1
   */

  void write_subarray(std::ostream& stream, std::vector<Arrayelt>::size_type& idx, unsigned dimIndex) const;

};


} // namespace CaloRec


namespace CxxUtils {
  using CaloRec::Arrayrep;
  using CaloRec::Arrayelt;
}


#endif // not CXXUTILS_ARRAYREP_H
