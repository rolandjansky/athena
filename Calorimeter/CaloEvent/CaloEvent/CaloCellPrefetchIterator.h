// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloEvent/CaloCellPrefetchIterator.h
 * @author scott snyder
 * @date Aug 2008
 * @brief To iterate over @c CaloCell's, prefetching the detector description.
 */


#ifndef CALOEVENT_CALOCELLPREFETCHITERATOR_H
#define CALOEVENT_CALOCELLPREFETCHITERATOR_H


#include "CaloEvent/CaloCell.h"
#include "Navigation/NavigationToken.h"
#include <iterator>


// Forward declaration.
template <class T>
class CaloCellPrefetchIterator;


namespace CaloEvent_detail {


/**
 * @brief Interface to @c NavigableToken weights.  No-weight version.
 *
 * We want our iterator to work with both @c NavigableToken iterators
 * and normal iterators.  The key difference is that @c NavigableToken
 * also has a weight attached to each element.  In that case, we need
 * to save weights in the iterator, like we do below for cell pointers.
 * However, we don't want to do that in the case where we don't
 * actually have weights.
 *
 * Thus, we break out the part handing the weights into a base class
 * of @c CaloCellPrefetchIterator.  We recognize the @c NavigableToken
 * case because its iterator has a special @c iterator_category.
 * So, we have a specialized version of the @c paramholder class
 * for this case.  The unspecialized case handles the case
 * where there are no weights.  It's empty, so when used as a base
 * class, should not increase the size of the iterator.
 *
 * This is the unspecialized case.
 */
template <class T, class TAG>
struct paramholder
{
  /// Type for the parameter (a dummy here).
  typedef int param_t;

  /// Return the current parameter (will always be 1).
  param_t getParameter() const;


private:
  friend class CaloCellPrefetchIterator<T>;

  /// Move to the next weight (a no-op).
  void next ();

  /// Fetch the next weight from the iterator (a no-op).
  void fetch (const T& it);
};


/**
 * @brief Interface to @c NavigableToken weights.  Weighted version.
 */
template <class T>
struct paramholder<T, NavigationTokenIteratorTag>
{
  /// Type for the parameter (from the token iterator).
  typedef typename T::tokenParameter param_t;

  /// Return the current parameter.
  param_t getParameter() const;


private:
  friend class CaloCellPrefetchIterator<T>;

  /// Move to the next weight.
  void next ();

  /// Fetch the next weight from the iterator (a no-op).
  void fetch (const T& it);

  /// The current parameter.
  param_t m_param;

  /// The next parameter.
  param_t m_next_param;
};


} // namespace CaloEvent_detail



/*
 * @brief To iterate over @a CaloCell's, prefetching the detector description.
 *
 * A typical use of @a CaloCell's is to iterate over a container of them;
 * for each cell, we then call methods on the detector descriptor element
 * for that cell.  It will often be the case, though, that the DDE data
 * are not in cache when first accessed.  This will show up in profiles
 * as the first DDE accessor method taking much longer than subsequent ones.
 *
 * A way to mitigate this is to use the prefetch feature of the x86 CPUs.
 * At the start of the loop, we look ahead at the next cell to be processed
 * and issue a prefetch for its DDE.  The prefetch itself won't stall, and
 * when we come around to the next iteration of the loop, it is likely
 * that the DDE data are now in cache.
 *
 * Here is an example of how to use this:
 *
 *@code
   CONTAINER container = ...;
   CaloCellPrefetchIterator<CONTAINER::const_iterator> it
     (container.begin(), container.end());
   while (it.next()) {
     const CaloCell* cellp = *it;
   }
 @endcode
 *
 * If the container is a @a NavigableToken, you can also get the parameter
 * with <code>it.getParameter()</code>.
 *
 * As a convenience, you can also say
 *
 *@code
   CaloCellPrefetchIterator<CONTAINER::const_iterator> it  (container);
 @endcode
 *
 * Note that there must be no null pointers in the collection.
 *
 * Also note that the prefetch instructions won't actually be generated
 * unless -msse is used during compilation.  (This is for x86; other
 * architectures may require other switches.)
 *
 * (Originally, the intention was to make this be a STL-style iterator
 * so that it could be a drop-in replacement.  It proved impractical
 * to do it that way and have it also be efficient and robust.)
 *
 * Because iterators may be heavy-weight (such as for
 * @c NavigableTokenIterator), we keep only the current and end
 * iterators here.  We separately cache the cell pointers (and weights)
 * for the current and next elements.  We're careful to only
 * do the iterator comparison once per loop.
 */
template <class T>
class CaloCellPrefetchIterator
  : public CaloEvent_detail::paramholder<T, typename T::iterator_category>
{
public:
  /// The base iterator, on which we're templated.
  typedef T base_iterator;

  using iterator_category = std::forward_iterator_tag;
  using value_type = const CaloCell*;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;

  /// The base paramholder class.
  typedef CaloEvent_detail::paramholder<T, typename T::iterator_category>
    paramholder;


  /**
   * @brief Constructor.
   * @param begin Underlying begin iterator.
   * @param end Underlying end iterator.
   */
  CaloCellPrefetchIterator (const base_iterator& begin,
                            const base_iterator& end);


  /**
   * @brief Constructor, from a container.
   * @param cont Container over which to iterate.
   */
  template <class CONTAINER>
  CaloCellPrefetchIterator (const CONTAINER& cont);


  /**
   * @brief Move to the next element.
   *
   * Returns true if more elements, false otherwise.
   * This should be called before processing the first element.
   */
  bool next();


  /**
   * @brief Dereference the iterator.
   */
  const CaloCell* operator*() const;

  /// Get this from the base class.
  using paramholder::getParameter;


private:
  /// Initialize before first use.
  void init();

  /// Current cell pointer.
  const CaloCell* m_cellp;

  /// Next cell pointer.
  const CaloCell* m_next_cellp;

  /// Current iterator.  Points at the next element.
  T m_it;

  /// End iterator.
  T m_end;
};


#include "CaloEvent/CaloCellPrefetchIterator.icc"


#endif // not CALOEVENT_CALOCELLPREFETCHITERATOR_H
