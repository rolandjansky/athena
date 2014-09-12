// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/DVL_iter_swap.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Specializations of @c iter_swap for @c DataVector/@c DataList.
 */


#ifndef ATHCONTAINERS_TOOLS_DVL_ITER_SWAP_H
#define ATHCONTAINERS_TOOLS_DVL_ITER_SWAP_H


#include "AthContainers/tools/DVLIterator.h"
#include "AthContainers/tools/ATHCONTAINERS_ASSERT.h"
#include <iterator>


namespace std {


/**
 * @brief Specialization of @c iter_swap for @c DataVector/@c DataList.
 * @param a First iterator for the swap.
 * @param b Second iterator for the swap.
 *
 * Swaps @c *a with @c *b, respecting the @c DataVector/List ownership rules.
 */
template <class DV>
void iter_swap (typename DataModel_detail::iterator<DV> a,
                typename DataModel_detail::iterator<DV> b)
{
  DV::iter_swap (a, b);
}


/**
 * @brief Specialization of @c iter_swap for @c DataVector/@c DataList.
 * @param a First reverse_iterator for the swap.
 * @param b Second reverse_iterator for the swap.
 *
 * Swaps @c *a with @c *b, respecting the @c DataVector/List ownership rules.
 */
template <class DV>
void
iter_swap (typename std::reverse_iterator<DataModel_detail::iterator<DV> > a,
           typename std::reverse_iterator<DataModel_detail::iterator<DV> > b)
{
  DataModel_detail::iterator<DV> ai = a.base();
  DataModel_detail::iterator<DV> bi = b.base();
  --ai;
  --bi;
  DV::iter_swap (ai, bi);
}


/**
 * @brief Specialization of @c swap_ranges for @c DataVector/@c DataList.
 * @param first1 Start of the first range.
 * @param last End of the first range.
 * @param first2 Start of the second range.
 *
 * Swap, element-by-element, the two ranges [first1, last) and
 * [first2, first2+(last-first1)).
 */
template <class DV>
DataModel_detail::iterator<DV>
swap_ranges (DataModel_detail::iterator<DV> first1,
             DataModel_detail::iterator<DV> last1,
             DataModel_detail::iterator<DV> first2)
{

  for (; first1 != last1; ++first1, ++first2)
    std::iter_swap(first1, first2);
  return first2;
}


/**
 * @brief Specialization of @c swap_ranges for @c DataVector/@c DataList.
 * @param first1 Start of the first range.
 * @param last End of the first range.
 * @param first2 Start of the second range.
 *
 * Swap, element-by-element, the two ranges [first1, last) and
 * [first2, first2+(last-first1)).
 */
template <class DV>
std::reverse_iterator<DataModel_detail::iterator<DV> >
swap_ranges (std::reverse_iterator<DataModel_detail::iterator<DV> > first1,
             std::reverse_iterator<DataModel_detail::iterator<DV> > last1,
             std::reverse_iterator<DataModel_detail::iterator<DV> > first2)
{

  for (; first1 != last1; ++first1, ++first2)
    std::iter_swap(first1, first2);
  return first2;
}


} // namespace std


#endif // not ATHCONTAINERS_TOOLS_DVL_ITER_SWAP_H
