// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOPREFETCH_H
#define CALOEVENT_CALOPREFETCH_H
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//------------------------------------------------------------------------

#include "CxxUtils/prefetch.h"
#include "CaloEvent/CaloCell.h"
#include "AthAllocators/ArenaHandle.h"
#include <cstddef>

//		---------------------
// 		-- Class Interface --
//		---------------------

/**
 *
 *  This software was developed for the ATLAS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andy Salnikov
 */

namespace CaloPrefetch {


/**
 *  @brief Prefetch next CaloDDE.
 *
 *  This prefetch method should be used with the collection of CaloCell pointers.
 *  It prefetches CaloCell objects two iterations ahead and CaloDDE object from
 *  CaloCell for the next iteration. On input iter must not be equal to endIter
 *  (this is not checked).
 *
 */
template <typename Iter>
inline
void nextDDE(Iter iter, Iter endIter)
{
  if (++iter != endIter) {
    // Get DDE, cell itself is expected to be in the cache already
    CxxUtils::prefetchObj((*iter)->caloDDE());
    if (++iter != endIter) {
      // prefetch cell two steps ahead
      CxxUtils::prefetchObj<CaloCell>(*iter);
    }
  }
}

/**
 *  @brief Prefetch few next CaloDDEs.
 *
 *  More generic version of the DDE prefetching mechanism intended for use in
 *  tighter loops when pre-fetching one step ahead is not enough. Argument
 *  specifies how many DDEs need to be prefetched.
 *
 *  Above specialization for one-iteration prefetch is still useful, this
 *  generic method is more complex and may have issues with inlining.
 *
 */
template <typename Iter>
inline
void nextDDE(Iter iter, Iter endIter, int count)
{
  for (int i = 0; i != count; ++ i) {
    if (++iter != endIter) {
      // Get DDE, cell itself is expected to be in the cache already
      CxxUtils::prefetchObj((*iter)->caloDDE());
    } else {
      return;
    }
  }
  for (int i = 0; i != count; ++ i) {
    if (++iter != endIter) {
      // prefetch cell two steps ahead
      CxxUtils::prefetchObj<CaloCell>(*iter);
    } else {
      return;
    }
  }
}

/**
 *  @brief Wrapper for SG::ArenaHandle with pre-fetching.
 *
 *  This class wraps SG::ArenaHandle and adds pre-fetching capability.
 *  Number of prefetched allocations is specified as a template argument
 *  so it must be a compile-time constant. By default a single allocation
 *  is pre-fetched, for tighter loops or if one allocates more than one
 *  item in the loop larger count can be specified.
 */
template <class T, class ALLOC, int COUNT=1>
class ArenaHandlePrefetch : public SG::ArenaHandle<T, ALLOC>{

  typedef SG::ArenaHandle<T, ALLOC> Base;

public:

  /**
   *  @brief Make
   */
  ArenaHandlePrefetch() : Base()
  {
    for (int i = 0; i != COUNT; ++ i) {
      m_buf[i] = Base::allocate();
    }
    for (int i = 0; i != COUNT; ++ i) {
      CxxUtils::prefetchN<sizeof(T)>(m_buf[i]);
    }
  }

  /**
   *  @brief Return space for new element, then allocate and prefetch one more.
   */
  void* allocate()
  {
    void* res = m_buf[0];
    std::move(m_buf + 1, m_buf + COUNT, m_buf);
    m_buf[COUNT-1] = Base::allocate();
    CxxUtils::prefetchN<sizeof(T)>(m_buf[COUNT-1]);
    return res;
  }

  /**
   *  @brief Free all allocated elements.
   */
  void erase() {
    Base::erase();
    for (int i = 0; i != COUNT; ++ i) {
      m_buf[i] = Base::allocate();
    }
    for (int i = 0; i != COUNT; ++ i) {
      CxxUtils::prefetchN<sizeof(T)>(m_buf[i]);
    }
  }

private:
  void* m_buf[COUNT];
};

} // namespace CaloPrefetch


#endif // CALOEVENT_CALOPREFETCH_H
