/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArrayScanner.cxx,v 1.1 2009-03-20 20:44:23 ssnyder Exp $
/**
 * @file  ArrayScanner.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Helper class for converting strings to Array's.
 */


#include "CxxUtils/ArrayScanner.h"


namespace CxxUtils {


/**
 * @brief Constructor.
 * @param is     The stream from which to scan.
 *
 * Builds a new scanner reading from stream @a is.
 */
ArrayScanner::ArrayScanner (std::istream& is)
  : m_is (is)
{
}


/**
 * @brief Read opening token.
 * @return True if successful.
 *
 * Consume any white space at the head of the stream.
 * If we're then looking at `[', consume it and return true.
 * Otherwise, return false.
 */
bool ArrayScanner::at_open()
{
  // Skip any opening quote marks.
  at_char ('\'');
  at_char ('"');

  return at_char ('[');
}


/**
 * @brief Read closing token.
 * @return True if successful.
 *
 * Consume any white space at the head of the stream.
 * If we're then looking at `]', consume it and return true.
 * If there's a comma following it, consume that too.
 * Otherwise, return false.
 */
bool ArrayScanner::at_close()
{
  if (at_char (']')) {
    at_char (',');

    // Skip any closing quote marks.
    at_char ('\'');
    at_char ('"');
    return true;
  }
  return false;
}


/**
 * @brief Test for end-of-stream.
 * @return True if successful.
 *
 * Consume any white space at the head of the stream.
 * Return true if we're then at the end of the stream.
 * Otherwise, return false.
 */
bool ArrayScanner::at_end()
{
  std::istream::sentry s (m_is);
  return !s;
}


/**
 * @brief Read a character.
 * @param The character to read.
 * @return True if successful.
 *
 * Consume any white space at the head of the stream.
 * If we're then looking at @a c, consume it and return true.
 * Otherwise, return false.
 */
bool ArrayScanner::at_char (char c)
{
  std::istream::sentry s (m_is);
  if (s && m_is.peek() == c) {
    m_is.get();
    return true;
  }
  return false;
}


/**
 * @brief The non-template part of reading a number.
 * @return True if successful.
 * 
 * This is called after the attempt to read the number itself.
 * This function checks that the read was in fact successful.
 * If so, then it will also consume any following comma.
 */
bool ArrayScanner::at_num_common ()
{
  bool stat = !m_is.fail();
  m_is.clear();
  at_char (',');
  return stat;
}


} // namespace CxxUtils
