// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/copy_bounded.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2013
 * @brief Copy a range with bounds restriction.
 */


#ifndef CXXUTILS_COPY_BOUNDED_H
#define CXXUTILS_COPY_BOUNDED_H


#include <iterator>
#include <algorithm>
#include "boost/range/iterator.hpp"
#include "boost/range/begin.hpp"
#include "boost/range/end.hpp"


namespace CxxUtils {


/**
 * @brief Copy a range with bounds restriction; generic version.
 */
template <class InputIterator, class OutputIterator,
          class InputTag, class OutputTag>
inline
OutputIterator
copy_bounded1 (InputIterator begi, InputIterator endi,
               OutputIterator bego, OutputIterator endo,
               const InputTag&,
               const OutputTag&)
{
  while (begi != endi && bego != endo) {
    *bego = *begi;
    ++begi;
    ++bego;
  }
  return bego;
}


/**
 * @brief Copy a range with bounds restriction; random_access_iterator version.
 */
template <class InputIterator, class OutputIterator>
inline
OutputIterator
copy_bounded1 (InputIterator begi, InputIterator endi,
               OutputIterator bego, OutputIterator endo,
               const std::random_access_iterator_tag&,
               const std::random_access_iterator_tag&)
{
  size_t n = std::min (endi-begi, endo-bego);
  return std::copy (begi, begi+n, bego);
}


/**
 * @brief Copy a range with bounds restriction.
 * @param begi Start of input range.
 * @param endi End of input range.
 * @param bego Start of output range.
 * @param endo End of output range.
 *
 * Like std::copy(begi, endi, bego), except that it will not copy
 * more than std::distance(bego, endo) elements.
 *
 * Copies exactly n = std::min (std::distance(begi,endi),
 *                              std::distance(bego,endo)) elements.
 * Returns bego + n.
 */
template <class InputIterator, class OutputIterator>
inline
OutputIterator
copy_bounded (InputIterator begi, InputIterator endi,
              OutputIterator bego, OutputIterator endo)
{
  return copy_bounded1
    (begi, endi, bego, endo,
     typename std::iterator_traits<InputIterator>::iterator_category(),
     typename std::iterator_traits<OutputIterator>::iterator_category());
}


/**
 * @brief Copy a range with bounds restriction.
 * @param input Input range
 * @param output Output range
 *
 * copy_bounded written in terms of iterator ranges.
 */
template <class InputRange, class OutputRange>
inline
typename boost::range_iterator<OutputRange>::type
copy_bounded (const InputRange& input, OutputRange& output)
{
  return copy_bounded
    (boost::begin(input),  boost::end(input),
     boost::begin(output), boost::end(output));
}


} // namespace CxxUtils



#endif // not CXXUTILS_COPY_BOUNDED_H
