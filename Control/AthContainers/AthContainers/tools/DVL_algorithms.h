// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/DVL_algorithms.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Specializations of STL algorithms to work with @c DataVector/List.
 */


#ifndef ATHCONTAINERS_TOOLS_DVL_ALGORITHMS_H
#define ATHCONTAINERS_TOOLS_DVL_ALGORITHMS_H


#include "AthContainers/tools/DVLIterator.h"
#include <iterator>
#include <algorithm>


#include "AthContainers/tools/DVL_algorithms.icc"



//===========================================================================
// Algorithms requiring @c ForwardIterator.
// Usable with @c DataVector and @c DataList.
//

namespace std {


/**
 * @brief Specialization of @c remove for @c DataVector/List.
 * @param beg The start iterator for the remove.
 * @param end The end iterator for the remove.
 * @param value The value to remove.
 *
 * @c beg and @c end should both be iterators from the same @c DataVector/List.
 * This performs the remove in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class T>
typename DataModel_detail::iterator<DVL>
remove (typename DataModel_detail::iterator<DVL> beg,
        typename DataModel_detail::iterator<DVL> end,
        const T& value)
{
  // Implemented with iter_swap, no extra index manipulation needed.
  return DataModel_detail::dvl_remove (beg, end, value);
}


/**
 * @brief Specialization of @c remove_if for @c DataVector/List.
 * @param beg The start iterator for the remove.
 * @param end The end iterator for the remove.
 * @param pred The predicate for the removal.
 *
 * @c beg and @c end should both be iterators from the same @c DataVector/List.
 * This performs the remove in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class Predicate>
typename DataModel_detail::iterator<DVL>
remove_if (typename DataModel_detail::iterator<DVL> beg,
           typename DataModel_detail::iterator<DVL> end,
           Predicate pred)
{
  // Implemented with iter_swap, no extra index manipulation needed.
  return DataModel_detail::dvl_remove_if (beg, end, pred);
}


/**
 * @brief Specialization of @c remove for @c DataVector/List.
 * @param beg The start reverse_iterator for the remove.
 * @param end The end reverse_iterator for the remove.
 * @param value The value to remove.
 *
 * @c beg and @c end should both be reverseIterators
 * from the same @c DataVector/List.
 * This performs the remove in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class T>
typename std::reverse_iterator<DataModel_detail::iterator<DVL> >
remove (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
        typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
        const T& value)
{
  // Implemented with iter_swap, no extra index manipulation needed.
  return DataModel_detail::dvl_remove (beg, end, value);
}


/**
 * @brief Specialization of @c remove_if for @c DataVector/List.
 * @param beg The start reverse_iterator for the remove.
 * @param end The end reverse_iterator for the remove.
 * @param pred The predicate for the removal.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the remove in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class Predicate>
typename std::reverse_iterator<DataModel_detail::iterator<DVL> >
remove_if (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
           typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
           Predicate pred)
{
  // Implemented with iter_swap, no extra index manipulation needed.
  return DataModel_detail::dvl_remove_if (beg, end, pred);
}


/**
 * @brief Specialization of @c unique for @c DataVector/List.
 * @param beg The start iterator for the unique operation.
 * @param end The end iterator for the unique operation.
 * @param value The value to remove.
 *
 * @c beg and @c end should both be iterators from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's object ownership rules.
 */
template <class DVL>
typename DataModel_detail::iterator<DVL>
unique (typename DataModel_detail::iterator<DVL> beg,
        typename DataModel_detail::iterator<DVL> end)
{
  // Implemented with iter_swap, no extra index manipulation needed.
  return DataModel_detail::dvl_unique (beg, end);
}


/**
 * @brief Specialization of @c unique for @c DataVector/List.
 * @param beg The start iterator for the unique operation.
 * @param end The end iterator for the unique operation.
 * @param pred The predicate for the operation.
 *
 * @c beg and @c end should both be iterators from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's object ownership rules.
 */
template <class DVL, class BinaryPredicate>
typename DataModel_detail::iterator<DVL>
unique (typename DataModel_detail::iterator<DVL> beg,
        typename DataModel_detail::iterator<DVL> end,
        BinaryPredicate pred)
{
  // Implemented with iter_swap, no extra index manipulation needed.
  return DataModel_detail::dvl_unique (beg, end, pred);
}


/**
 * @brief Specialization of @c unique for @c DataVector/List.
 * @param beg The start reverse_iterator for the unique operation.
 * @param end The end reverse_iterator for the unique operation.
 * @param value The value to remove.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's object ownership rules.
 */
template <class DVL>
typename std::reverse_iterator<DataModel_detail::iterator<DVL> >
unique (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
        typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end)
{
  // Implemented with iter_swap, no extra index manipulation needed.
  return DataModel_detail::dvl_unique (beg, end);
}


/**
 * @brief Specialization of @c unique for @c DataVector/List.
 * @param beg The start reverse_iterator for the unique operation.
 * @param end The end reverse_iterator for the unique operation.
 * @param pred The predicate for the operation.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class BinaryPredicate>
typename std::reverse_iterator<DataModel_detail::iterator<DVL> >
unique (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
        typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
        BinaryPredicate pred)
{
  // Implemented with iter_swap, no extra index manipulation needed.
  return DataModel_detail::dvl_unique (beg, end, pred);
}


/**
 * @brief Specialization of @c rotate for @c DataVector/List.
 * @param beg The start iterator for the rotate operation.
 * @param mid The middle iterator for the rotate operation.
 * @param end The end iterator for the rotate operation.
 *
 * @c beg and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL>
void rotate (typename DataModel_detail::iterator<DVL> beg,
             typename DataModel_detail::iterator<DVL> mid,
             typename DataModel_detail::iterator<DVL> end)
{
  std::rotate (beg.base(), mid.base(), end.base());
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c rotate for @c DataVector/List.
 * @param beg The start reverse_iterator for the rotate operation.
 * @param mid The middle reverse_iterator for the rotate operation.
 * @param end The end reverse_iterator for the rotate operation.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL>
void
rotate (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
        typename std::reverse_iterator<DataModel_detail::iterator<DVL> > mid,
        typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::rotate (ri(beg.base().base()),
               ri(mid.base().base()),
               ri(end.base().base()));
  DataModel_detail::resortAux (beg, end);
}


} // namespace std



//===========================================================================
// Algorithms requiring @c BidirectionalIterator.
// Usable with @c DataVector and @c DataList.
//

namespace std {


/**
 * @brief Specialization of @c reverse for @c DataVector/List.  @param beg
 * The start iterator for the reverse operation.  @param end The end
 * iterator for the reverse operation.
 *
 * @c beg and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL>
void reverse (typename DataModel_detail::iterator<DVL> beg,
              typename DataModel_detail::iterator<DVL> end)
{
  std::reverse (beg.base(), end.base());
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c reverse for @c DataVector/List.
 * @param beg The start reverse_iterator for the reverse operation.
 * @param end The end reverse_iterator for the reverse operation.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL>
void
reverse (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
         typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::reverse (ri(beg.base().base()), ri(end.base().base()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c partition for @c DataVector/List.
 * @param beg The start iterator for the partition operation.
 * @param end The end iterator for the partition operation.
 * @param pred The predicate for the partition.
 *
 * @c beg and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class Predicate>
typename DataModel_detail::iterator<DVL>
partition (typename DataModel_detail::iterator<DVL> beg,
           typename DataModel_detail::iterator<DVL> end,
           Predicate pred)
{
  typename DataModel_detail::iterator<DVL> ret
    (std::partition (beg.base(), end.base(),
                     DataModel_detail::Predwrapper<DVL, Predicate> (pred)),
     beg.container());
  DataModel_detail::resortAux (beg, end);
  return ret;
}


/**
 * @brief Specialization of @c partition for @c DataVector/List.
 * @param beg The start reverse_iterator for the partition operation.
 * @param end The end reverse_iterator for the partition operation.
 * @param pred The predicate for the partition.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class Predicate>
typename std::reverse_iterator<DataModel_detail::iterator<DVL> >
partition (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
           typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
           Predicate pred)
{
  typedef typename DataModel_detail::iterator<DVL> Iterator;
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  typedef typename DataModel_detail::Predwrapper<DVL, Predicate> pwrap_t;
  std::reverse_iterator<Iterator> ret
    (Iterator (std::partition (ri(beg.base().base()),
                               ri(end.base().base()),
                               pwrap_t (pred)).base(),
               beg.base().container()));
  DataModel_detail::resortAux (beg, end);
  return ret;
}


/**
 * @brief Specialization of @c stable_partition for @c DataVector/List.
 * @param beg The start iterator for the partition operation.
 * @param end The end iterator for the partition operation.
 * @param pred The predicate for the partition.
 *
 * @c beg and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class Predicate>
typename DataModel_detail::iterator<DVL>
stable_partition (typename DataModel_detail::iterator<DVL> beg,
                  typename DataModel_detail::iterator<DVL> end,
                  Predicate pred)
{
  DataModel_detail::iterator<DVL> ret
    (std::stable_partition (beg.base(), end.base(),
                            DataModel_detail::Predwrapper<DVL, Predicate>
                              (pred)),
     beg.container());
  DataModel_detail::resortAux (beg, end);
  return ret;
}


/**
 * @brief Specialization of @c stable_partition for @c DataVector/List.
 * @param beg The start reverse_iterator for the partition operation.
 * @param end The end reverse_iterator for the partition operation.
 * @param pred The predicate for the partition.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class Predicate>
typename std::reverse_iterator<DataModel_detail::iterator<DVL> >
stable_partition
  (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
   typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
   Predicate pred)
{
  typedef typename DataModel_detail::iterator<DVL> Iterator;
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  typedef typename DataModel_detail::Predwrapper<DVL, Predicate> pwrap_t;
  std::reverse_iterator<Iterator> ret
    (Iterator (std::stable_partition (ri(beg.base().base()),
                                      ri(end.base().base()),
                                      pwrap_t (pred)).base(),
               beg.base().container()));
  DataModel_detail::resortAux (beg, end);
  return ret;
}


/**
 * @brief Specialization of @c inplace_merge for @c DataVector/List.
 * @param beg The start iterator for the merge operation.
 * @param mid Divider between the two sequences to be merged.
 * @param end The end iterator for the merge operation.
 *
 * @c beg, @c mid, and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL>
void
inplace_merge (typename DataModel_detail::iterator<DVL> beg,
               typename DataModel_detail::iterator<DVL> mid,
               typename DataModel_detail::iterator<DVL> end)
{
  std::inplace_merge (beg.base(), mid.base(), end.base());
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c inplace_merge for @c DataVector/List.
 * @param beg The start iterator for the merge operation.
 * @param mid Divider between the two sequences to be merged.
 * @param end The end iterator for the merge operation.
 * @param comp The comparison object.
 *
 * @c beg, @c mid, and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class Compare>
void
inplace_merge (typename DataModel_detail::iterator<DVL> beg,
               typename DataModel_detail::iterator<DVL> mid,
               typename DataModel_detail::iterator<DVL> end,
               Compare comp)
{
  std::inplace_merge (beg.base(), mid.base(), end.base(),
                      typename DataModel_detail::Compwrapper<DVL, Compare>(comp));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c inplace_merge for @c DataVector/List.
 * @param beg The start iterator for the merge operation.
 * @param mid Divider between the two sequences to be merged.
 * @param end The end iterator for the merge operation.
 *
 * @c beg, @c mid, and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL>
void
inplace_merge (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
               typename std::reverse_iterator<DataModel_detail::iterator<DVL> > mid,
               typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::inplace_merge (ri (beg.base().base()),
                      ri (mid.base().base()),
                      ri (end.base().base()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c inplace_merge for @c DataVector/List.
 * @param beg The start iterator for the merge operation.
 * @param mid Divider between the two sequences to be merged.
 * @param end The end iterator for the merge operation.
 * @param comp The comparison object.
 *
 * @c beg, @c mid, and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class Compare>
void
inplace_merge (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
               typename std::reverse_iterator<DataModel_detail::iterator<DVL> > mid,
               typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
               Compare comp)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::inplace_merge (ri (beg.base().base()),
                      ri (mid.base().base()),
                      ri (end.base().base()),
                      typename DataModel_detail::Compwrapper<DVL, Compare>(comp));
  DataModel_detail::resortAux (beg, end);
}


} // namespace std



//===========================================================================
// Algorithms requiring @c RandomAccessIterator.
// Usable with @c DataVector.
//

namespace std {


/**
 * @brief Specialization of @c sort for @c DataVector/List.
 * @param beg The start iterator for the sort.
 * @param end The end iterator for the sort.
 *
 * @c beg and @c end should both be iterators from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL>
void sort (typename DataModel_detail::iterator<DVL> beg,
           typename DataModel_detail::iterator<DVL> end)
{
  // Wrap the default comparison object,
  // in case someone's overridden operator< for pointers.
  typedef std::less<typename DVL::BaseContainer::value_type> less;
  std::sort (beg.base(), end.base(),
             typename DataModel_detail::Compwrapper<DVL, less> (less()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c sort for @c DataVector/List.
 * @param beg The start iterator for the sort.
 * @param end The end iterator for the sort.
 * @param comp The comparison functional object.
 *
 * @c beg and @c end should both be iterators from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class Compare>
void sort (typename DataModel_detail::iterator<DVL> beg,
           typename DataModel_detail::iterator<DVL> end,
           Compare comp)
{
  std::sort (beg.base(), end.base(),
             typename DataModel_detail::Compwrapper<DVL, Compare> (comp));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c sort for @c DataVector/List.
 * @param beg The start reverse_iterator for the sort.
 * @param end The end reverse_iterator for the sort.
 *
 * @c beg and @c end should both be reverse iterators
 * from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL>
void sort (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
           typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  // Wrap the default comparison object,
  // in case someone's overridden operator< for pointers.
  typedef std::less<typename DVL::BaseContainer::value_type> less;
  std::sort (ri (beg.base().base()), ri (end.base().base()),
             typename DataModel_detail::Compwrapper<DVL, less> (less()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c sort for @c DataVector/List.
 * @param beg The start reverse_iterator for the sort.
 * @param end The end reverse_iterator for the sort.
 * @param comp The comparison functional object.
 *
 * @c beg and @c end should both be reverse iterators
 * from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class Compare>
void sort (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
           typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
           const Compare& comp)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::sort (ri (beg.base().base()), ri (end.base().base()),
             typename DataModel_detail::Compwrapper<DVL, Compare> (comp));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c stable_sort for @c DataVector/List.
 * @param beg The start iterator for the sort.
 * @param end The end iterator for the sort.
 *
 * @c beg and @c end should both be iterators from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL>
void stable_sort (DataModel_detail::iterator<DVL> beg,
                  DataModel_detail::iterator<DVL> end)
{
  // Wrap the default comparison object,
  // in case someone's overridden operator< for pointers.
  typedef std::less<typename DVL::BaseContainer::value_type> less;
  std::stable_sort (beg.base(), end.base(),
                    DataModel_detail::Compwrapper<DVL, less> (less()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c stable_sort for @c DataVector/List.
 * @param beg The start iterator for the sort.
 * @param end The end iterator for the sort.
 *
 * @c beg and @c end should both be iterators from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class Compare>
void stable_sort (DataModel_detail::iterator<DVL> beg,
                  DataModel_detail::iterator<DVL> end,
                  Compare comp)
{
  std::stable_sort (beg.base(), end.base(),
                    DataModel_detail::Compwrapper<DVL, Compare> (comp));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c stable_sort for @c DataVector/List.
 * @param beg The start reverse_iterator for the sort.
 * @param end The end reverse_iterator for the sort.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL>
void stable_sort (std::reverse_iterator<DataModel_detail::iterator <DVL> > beg,
                  std::reverse_iterator<DataModel_detail::iterator <DVL> > end)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  // Wrap the default comparison object,
  // in case someone's overridden operator< for pointers.
  typedef std::less<typename DVL::BaseContainer::value_type> less;
  std::stable_sort (ri (beg.base().base()), ri (end.base().base()),
                    typename DataModel_detail::Compwrapper<DVL, less> (less()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c stable_sort for @c DataVector/List.
 * @param beg The start reverse_iterator for the sort.
 * @param end The end reverse_iterator for the sort.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class Compare>
void stable_sort (std::reverse_iterator<DataModel_detail::iterator <DVL> > beg,
                  std::reverse_iterator<DataModel_detail::iterator <DVL> > end,
                  Compare comp)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::stable_sort (ri (beg.base().base()), ri (end.base().base()),
                    typename DataModel_detail::Compwrapper<DVL, Compare>(comp));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c partial_sort for @c DataVector/List.
 * @param beg The start iterator for the sort.
 * @param mid The middle iterator for the sort.
 * @param end The end iterator for the sort.
 *
 * @c beg, @c mid, and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL>
void partial_sort (DataModel_detail::iterator<DVL> beg,
                   DataModel_detail::iterator<DVL> mid,
                   DataModel_detail::iterator<DVL> end)
{
  // Wrap the default comparison object,
  // in case someone's overridden operator< for pointers.
  typedef std::less<typename DVL::BaseContainer::value_type> less;
  std::partial_sort (beg.base(), mid.base(), end.base(),
                     DataModel_detail::Compwrapper<DVL, less> (less()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c partial_sort for @c DataVector/List.
 * @param beg The start iterator for the sort.
 * @param mid The middle iterator for the sort.
 * @param end The end iterator for the sort.
 *
 * @c beg, @c mid, and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class Compare>
void partial_sort (DataModel_detail::iterator<DVL> beg,
                   DataModel_detail::iterator<DVL> mid,
                   DataModel_detail::iterator<DVL> end,
                   Compare comp)
{
  std::partial_sort (beg.base(), mid.base(), end.base(),
                     DataModel_detail::Compwrapper<DVL, Compare> (comp));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c partial_sort for @c DataVector/List.
 * @param beg The start reverse_iterator for the sort.
 * @param mid The middle reverse_iterator for the sort.
 * @param end The end reverse_iterator for the sort.
 *
 * @c beg, @c mid, and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL>
void partial_sort (std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
                   std::reverse_iterator<DataModel_detail::iterator<DVL> > mid,
                   std::reverse_iterator<DataModel_detail::iterator<DVL> > end)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  // Wrap the default comparison object,
  // in case someone's overridden operator< for pointers.
  typedef std::less<typename DVL::BaseContainer::value_type> less;
  std::partial_sort (ri (beg.base().base()),
                     ri (mid.base().base()),
                     ri (end.base().base()),
                     typename DataModel_detail::Compwrapper<DVL, less>
                       (less()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c partial_sort for @c DataVector/List.
 * @param beg The start reverse_iterator for the sort.
 * @param mid The middle reverse_iterator for the sort.
 * @param end The end reverse_iterator for the sort.
 *
 * @c beg, @c mid, and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the sort in a way that doesn't run afoul
 * of @c DataVector/List's object ownership rules.
 */
template <class DVL, class Compare>
void partial_sort (std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
                   std::reverse_iterator<DataModel_detail::iterator<DVL> > mid,
                   std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
                   Compare comp)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::partial_sort (ri (beg.base().base()),
                     ri (mid.base().base()),
                     ri (end.base().base()),
                     typename DataModel_detail::Compwrapper<DVL, Compare>
                       (comp));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c random_shuffle for @c DataVector/List.
 * @param beg The start iterator for the shuffle operation.
 * @param end The end iterator for the shuffle operation.
 *
 * @c beg and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL>
void random_shuffle (typename DataModel_detail::iterator<DVL> beg,
                     typename DataModel_detail::iterator<DVL> end)
{
  std::random_shuffle (beg.base(), end.base());
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c random_shuffle for @c DataVector/List.
 * @param beg The start iterator for the shuffle operation.
 * @param end The end iterator for the shuffle operation.
 * @param rand The random generator.
 *
 * @c beg and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class Random>
void random_shuffle (typename DataModel_detail::iterator<DVL> beg,
                     typename DataModel_detail::iterator<DVL> end,
                     Random& rand)
{
  std::random_shuffle (beg.base(), end.base(), rand);
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c random_shuffle for @c DataVector/List.
 * @param beg The start reverse_iterator for the shuffle operation.
 * @param end The end reverse_iterator for the shuffle operation.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL>
void
random_shuffle
  (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
   typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::random_shuffle (ri(beg.base().base()),
                       ri(end.base().base()));
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c random_shuffle for @c DataVector/List.
 * @param beg The start reverse_iterator for the shuffle operation.
 * @param end The end reverse_iterator for the shuffle operation.
 * @param rand The random generator.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class Random>
void
random_shuffle
  (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
   typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
   Random& rand)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::random_shuffle (ri(beg.base().base()),
                       ri(end.base().base()),
                       rand);
  DataModel_detail::resortAux (beg, end);
}
                          

/**
 * @brief Specialization of @c shuffle for @c DataVector/List.
 * @param beg The start iterator for the shuffle operation.
 * @param end The end iterator for the shuffle operation.
 * @param g The uniform random number generator.
 *
 * @c beg and @c end should both be iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class UniformRandom>
void shuffle (typename DataModel_detail::iterator<DVL> beg,
              typename DataModel_detail::iterator<DVL> end,
              UniformRandom& g)
{
  std::shuffle (beg.base(), end.base(), g);
  DataModel_detail::resortAux (beg, end);
}


/**
 * @brief Specialization of @c shuffle for @c DataVector/List.
 * @param beg The start reverse_iterator for the shuffle operation.
 * @param end The end reverse_iterator for the shuffle operation.
 * @param g The uniform random number generator.
 *
 * @c beg and @c end should both be reverse_iterators
 * from the same @c DataVector/List.
 * This performs the operation in a way
 * that doesn't run afoul of @c DataVector/List's
 * object ownership rules.
 */
template <class DVL, class UniformRandom>
void
shuffle
  (typename std::reverse_iterator<DataModel_detail::iterator<DVL> > beg,
   typename std::reverse_iterator<DataModel_detail::iterator<DVL> > end,
   UniformRandom& g)
{
  typedef typename DVL::BaseContainer::reverse_iterator ri;
  std::shuffle (ri(beg.base().base()),
                ri(end.base().base()),
                g);
  DataModel_detail::resortAux (beg, end);
}


} // namespace std



#endif // not ATHCONTAINERS_TOOLS_DVL_ALGORITHMS_H
