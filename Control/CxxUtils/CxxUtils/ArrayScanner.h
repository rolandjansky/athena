// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArrayScanner.h,v 1.1 2009-03-20 20:44:22 ssnyder Exp $
/**
 * @file  ArrayScanner.h
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Helper class for converting strings to Array's.
 */


#ifndef CXXUTILS_ARRAYSCANNER_H
#define CXXUTILS_ARRAYSCANNER_H


#include <istream>


namespace CxxUtils {


/**
 * @class ArrayScanner
 * @brief Helper class for converting strings to Array's.
 *
 * This class is a simple lexical analyzer used in converting strings
 * to multidimensional array representations.  We get a stream as input.
 * This stream can contain three types of tokens: an open bracket,
 * a close bracket, or a floating-point number.  We provide methods
 * to test if either of these three items is at the head of the stream.
 * If so, the item is consumed.  (An optional comma may follow a close
 * bracket or a number; it is consumed when the item in front of it
 * is consumed.)  We can also test to see if we're at the end.
 */
class ArrayScanner
{
public:
  /**
   * @brief Constructor.
   * @param is     The stream from which to scan.
   *
   * Builds a new scanner reading from stream @a is.
   */
  ArrayScanner (std::istream& is);

  /**
   * @brief Read opening token.
   * @return True if successful.
   *
   * Consume any white space at the head of the stream.
   * If we're then looking at `[', consume it and return true.
   * Otherwise, return false.
   */
  bool at_open();

  /**
   * @brief Read closing token.
   * @return True if successful.
   *
   * Consume any white space at the head of the stream.
   * If we're then looking at `]', consume it and return true.
   * If there's a comma following it, consume that too.
   * Otherwise, return false.
   */
  bool at_close();

  /**
   * @brief Test for end-of-stream.
   * @return True if successful.
   *
   * Consume any white space at the head of the stream.
   * Return true if we're then at the end of the stream.
   * Otherwise, return false.
   */
  bool at_end();

  /**
   * @brief Read number.
   * @param elt[out] The number read.
   * @return True if successful.
   *
   * Consume any white space at the head of the stream.
   * If we're then looking at a number that can be converted to type @c T,
   * read it and return true.  The value is returned in @a elt.
   * If there's a comma following it, consume that too.
   * Otherwise, return false.
   */
  template <class T>
  bool at_num (T& elt);


private:
  /**
   * @brief Read a character.
   * @param c The character to read.
   * @return True if successful.
   *
   * Consume any white space at the head of the stream.
   * If we're then looking at @a c, consume it and return true.
   * Otherwise, return false.
   */
  bool at_char (char c);

  /**
   * @brief The non-template part of reading a number.
   * @return True if successful.
   * 
   * This is called after the attempt to read the number itself.
   * This function checks that the read was in fact successful.
   * If so, then it will also consume any following comma.
   */
  bool at_num_common();


  /// The stream from which we're reading.
  std::istream& m_is;
};


template <class T>
bool ArrayScanner::at_num (T& elt)
{
  m_is >> elt;
  return at_num_common();
}


} // namespace CxxUtils


// Backwards compatibility.
namespace CaloRec {
 using CxxUtils::ArrayScanner;
}


#endif // not CXXUTILS_ARRAYSCANNER_H
