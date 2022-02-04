// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/reverse_wrapper.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2022
 * @brief Helper for iterating over a container in reverse order.
 *
 * Given a container @c c, this code will iterate over the container
 * in reverse order:
 *
 *@code
 *   for (... elt : CxxUtils::make_reverse_wrapper (c)) {
 @endcode
 */


#ifndef CXXUTILS_REVERSE_WRAPPER_H
#define CXXUTILS_REVERSE_WRAPPER_H


namespace CxxUtils {


/**
 * @brief Adapter for a container-like class to be used in a range-for
 *        so as to iterate in the reverse direction.
 */
template <class C>
class reverse_wrapper
{
public:
  reverse_wrapper (C& r) : m_r (r) {}
  auto begin() { return m_r.rbegin(); }
  auto end() { return m_r.rend(); }
  C& m_r;
};


/**
 * @brief Make a @c reverse_wrapper for a given container-like object.
 */
template <class T>
auto make_reverse_wrapper (T& r) { return reverse_wrapper(r); }


} // namespace CxxUtils


#endif // not CXXUTILS_REVERSE_WRAPPER_H
