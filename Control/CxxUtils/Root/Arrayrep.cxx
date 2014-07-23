/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Arrayrep.cxx,v 1.3 2009-04-08 21:12:44 ssnyder Exp $
/**
 * @file  Arrayrep.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Representation class for Array's.
 */


#include "CxxUtils/Arrayrep.h"
#include "CxxUtils/ArrayScanner.h"
#include <stdexcept>
#include <cassert>
#include <sstream>


namespace CaloRec {


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
void Arrayrep::init_sizes (bool resize_data /*= false*/)
{
  // Don't do anything if we've already done this, or if the array
  // is empty.
  if (m_sizes.size() < m_shape.size())
  {
    // Calculate the m_sizes array.
    unsigned int sz = 1;
    unsigned int dim = m_shape.size();
    m_sizes.resize (dim);
    m_sizes[0] = 1;
    for (unsigned int i=0; i < dim; i++) {
      m_sizes[i] = sz;
      sz *= m_shape[dim-1 - i];
    }
    
    if (resize_data) {
      // Resize m_data to the proper size.
      m_data.resize (sz);
    }
    else {
      // Check that m_data has the correct size.
      assert (sz == m_data.size());
    }
  }
}


/**
 * @brief Construct an empty array of a given shape.
 * @param shape The shape of the array.
 *
 * Initialize an array of a given shape.
 * The array will contain all 0's.
 */
Arrayrep::Arrayrep (const std::vector<unsigned int>& shape)
  : m_shape (shape)
{
  // Set up m_sizes.
  init_sizes (true);
}


/**
 * @brief Construct an empty array of a given shape.
 * @param shape The shape of the array.
 * @param n     The length of the @a shape array.
 *
 * Initialize an array of a given shape.
 * The array will contain all 0's.
 * This version is more convenient to call with a constant shape.
 */
Arrayrep::Arrayrep (const unsigned int shape[], unsigned int n)
{
  // Copy the data into m_shape.
  m_shape.reserve (n);
  for (unsigned int i=0; i < n; i++)
    m_shape.push_back (shape[i]);

  // Set up m_sizes.
  init_sizes (true);
}


namespace {


/**
 * @brief Report an error in building an @c Arrayrep from a string.
 * @param context Additional context for the error.
 * @param msg The error message to report.
 *
 * Report an error by raising a @c std::runtime_error.
 */
void error (const std::string& context,
            const std::string& msg)
{
  throw std::runtime_error (msg + ": " + context);
}


/**
 * @brief Helper function to build an @c Arrayrep from a string.
 * @param rep  The @c Arrayrep we're building.
 * @param s    The @c ArrayScanner stream from which we're reading.
 * @param nest The current nesting level.
 * @param context For error reporting.
 *
 * Read tokens from @a s and build the array information
 * in @a Arrayrep.  This function should be called just after
 * consuming the opening bracket of an array.  It calls itself
 * recursively to handle subarrays.
 */
void read_array (Arrayrep& rep,
                 ArrayScanner& s,
                 unsigned int nest,
                 const std::string& context)
{
  // The first time we handle a subarray at a given nesting, we don't
  // know the size for that dimension, so we should just take what
  // we're given.  After that, though, we should be sure that the
  // size matches what was used before (a rectangular array).
  bool know_size = false;
  if (rep.m_shape.size() > nest) {
    // We know what the size should be.
    know_size = true;
  }
  else {
    // We don't know the size yet.  Go ahead and push a dummy
    // entry in m_shape; we'll update it later.
    rep.m_shape.push_back (0);
  }

  // Number of array elements (each of which could be a subarray)
  // which we're read.
  unsigned int n = 0;

  // Are we now looking at plain elements or at a nested subarray?
  if (s.at_open()) {
    // A nested subarray.
    // Disallow constructions like
    //  [[1], [[2]]]
    // Maximum nesting must be the same in all subarrays.
    if (rep.m_data.size() > 0 && nest >= rep.m_shape.size())
      error (context, "Bad array nesting");

    // Read the subarrays as long as we keep seeing more.
    do {
      read_array (rep, s, nest+1, context);
      ++n;
    } while (s.at_open());
  }
  else {
    // Plain elements.  Read them as long as we keep seeing them.
    Arrayelt elt;
    while (s.at_num (elt)) {
      rep.m_data.push_back (elt);
      ++n;
    }
  }

  // We should now be at a closing bracket.
  if (!s.at_close())
    error (context, "Missing close bracket");

  if (!know_size) {
    // We didn't yet know the size for this dimension.  Fill it in now.
    rep.m_shape[nest] = n;
  }
  else if (rep.m_shape[nest] != n) {
    // What we got doesn't match the size for this dimension we've
    // seen earlier.  Complain.
    error (context, "Array not rectangular");
  }
}


} // anonymous namespace


/**
 * @brief Construct from a string.
 * @param str The string to convert.
 * @param context An optional string to use for error reporting.
 *
 * Parse the string and initialize the array.
 * This string should be like `[[1, 2], [3, 4]]'.
 */
Arrayrep::Arrayrep (const std::string& str, const std::string& context /*=""*/)
{
  // Special cases for True and False.
  if (str == "True")
    m_data.push_back (1);
  else if (str == "False")
    m_data.push_back (0);
  else {
    // Make the token stream from which we're reading.
    std::istringstream is (str);
    ArrayScanner s (is);

    // If we're looking at an open bracket, consume it, and proceed
    // to read an array.
    if (s.at_open())
      read_array (*this, s, 0, context);
    else {
      // Otherwise, we're reading a scalar.  So read a single number.
      Arrayelt elt;
      if (!s.at_num (elt))
	error (context, "Number expected");
      m_data.push_back (elt);
    }

    // We should now be at the end of the stream.
    if (!s.at_end())
      error (context, "End of vector before end of string");
  }

  // Set up the m_sizes vector.
  init_sizes();
}


  /**
   * @brief Helper function for write_array.
   * @param stream where the array should be written
   * @param idx Current index in @c m_data
   * @param dimIndex Current index in @c m_shapes
   *
   * Calls itself recursively with dimIndex-1
   */

void Arrayrep::write_array(std::ostream& stream) const {
  if (!m_data.size()) {//Empty array
    stream << "[ ]" << std::endl;
    return;
  }

  if (!m_shape.size()) {//Single element
    stream << m_data[0] << std::endl;
    return;
  }

  //All other cases: Array of dimension>=1
  //check consistency of Array
  unsigned totSize=m_shape[0];
  for (unsigned i=1;i<m_shape.size();i++)
    totSize=totSize*m_shape[i];
  if (totSize!=m_data.size()) 
    error("","Array is inconsistent!");

  std::vector<Arrayelt>::size_type dataIndex=0;
  write_subarray(stream,dataIndex,0);
  stream << "]" << std::endl;
  return;
}


 /**
   * @brief Creates a text representation of the array content.
   * @param std::ostream where the text should be written
   *
   * Writes the content of the array to a ostream. The sub-arrays are
   * enclosed by square-brackets and separated by commas.
   */

void Arrayrep::write_subarray(std::ostream& stream, std::vector<Arrayelt>::size_type& idx, unsigned dimIndex) const {
  if (dimIndex<(m_shape.size()-1)) {
    stream << "[\n    ";
    for (unsigned i=0;i<m_shape[dimIndex];i++) {
      write_subarray(stream,idx,dimIndex+1);
      if (i==m_shape[dimIndex]-1)
	stream << "]\n    ";
      else
	stream << "],\n    ";
    }
  }
  else { // last dimension
    stream << "[" << m_data[idx++];
    for (unsigned i=1;i<m_shape[dimIndex];i++)
      stream << ", " << m_data[idx++];
  }
  return;
}



} // namespace CxxUtils
