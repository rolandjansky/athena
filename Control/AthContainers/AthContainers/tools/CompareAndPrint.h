/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHCONTAINERS_TOOLS_COMPAREANDPRINT_H
#define ATHCONTAINERS_TOOLS_COMPAREANDPRINT_H 1

//****************************************************************************
// Helpers for deleting @c DataVector/List elements.
//

#include <algorithm> /*sort, unique */
#include <typeinfo>

namespace DataModel_detail {
/**
 * @brief Helper for @c remove_duplicates.
 *
 * Functor to compare two pointers and optionally print a complaint
 * if they're the same.
 */
class CompareAndPrint
{
public:
  /**
   * @brief Constructor.
   * @param quiet If @c false, then print a complaint if duplicate
   *              pointers are found.
   */
  CompareAndPrint(bool quiet)
    : m_quiet(quiet) {}


  /**
   * @brief Compare two pointers.
   *        Maybe print a complaint if we find a duplicate.
   * @param f First pointer to compare.
   * @param s Second pointer to compare.
   * @return The result of the comparison.
   */
  template <typename Element>
  bool operator()(Element* f, Element* s) const
  {
    bool areEq = (f == s); 
    if (!m_quiet && areEq && f != 0)
      warn (typeid (*f), f);
    return areEq;
  }


  /**
   * @brief Duplicate elements were found; print a warning
   * @param ti @c type_info for the element.
   * @param f The element pointer.
   */
  // This is defined out-of-line in the cxx file.
  void warn (const std::type_info& ti, const void* f) const;


private:
  /// Quiet flag.
  bool m_quiet; 
};
} // namespace DataModel_detail
#endif  // not ATHCONTAINERS_TOOLS_COMPAREANDPRINT_H
