/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/KFromNItr.h"
#include <numeric>
#include <stdexcept>

namespace TrigCompositeUtils {
  KFromNItr::KFromNItr(std::size_t k, std::size_t N)
      : m_N(N),
        m_current(k, 0)
  {
    // Fill the iterator with the first combination
    reset();
  }

  void KFromNItr::reset()
  {
    // The first combination is just an ascending sequence starting from 0
    std::iota(m_current.begin(), m_current.end(), 0);
  }

  bool KFromNItr::exhausted() const
  {
    return size() == 0 || m_current.back() >= m_N;
  }

  KFromNItr::reference KFromNItr::operator*() const
  {
    if (exhausted())
      throw std::runtime_error("Dereferencing past-the-end iterator!");
    return m_current;
  }

  KFromNItr::pointer KFromNItr::operator->() const
  {
    if (exhausted())
      throw std::runtime_error("Dereferencing past-the-end iterator!");
    return &m_current;
  }

  KFromNItr &KFromNItr::operator++()
  {
    if (exhausted())
      // Don't iterate an iterator that is already past the end
      return *this;
    // All index combinations pointed to by this iterator are in ascending order.
    // In order to generate the next combination, go through the following steps,
    // starting with the highest (last) index
    //
    // 1. Increment the current index
    // 2. If it is less than its end point then go to step 4
    // 3. If it is equal to its end point then shift the current index to be the
    //    before this one and go to step 1.
    // 4. Set each index after the current one to larger than its preceding
    //    index's value
    //
    // For an example, consider picking 3 values from the range 0 - 4.
    // The maximum value here is '5'
    // The combination starts at  (0, 1, 2). For the first two increments only the
    // third index changes as the comparison in step 2 is always true and the
    // iterator yields the sequence (0, 1, 3), (0, 1, 4).
    // After this, the third index increments to 5 (the max value) and so we
    // increment the second index. As this is now less than its maximum value (4)
    // we reset all following indices in an ascending sequence, yielding (0, 2, 3)

    // Start from the last index
    auto backItr = m_current.rbegin();
    // the end point decreases as we go back through the sequence
    std::size_t end = m_N;
    while (backItr != m_current.rend())
    {
      // This statement increments the value pointed to by backItr, then checks
      // that it's still below its maximum value
      if (++(*backItr) < end)
      {
        // step 4 - this value is now OK. We have to set each following element
        // in increasing sequence
        // backItr.base() returns an iterator pointing to the element *after* this
        // one
        std::iota(backItr.base(), m_current.end(), *backItr + 1);
        break;
      }
      ++backItr;
      --end;
    }
    return *this;
  }

  KFromNItr KFromNItr::operator++(int)
  {
    /// Copy our current state
    KFromNItr ret(*this);
    // step us past this point
    this->operator++();
    return ret;
  }

  bool KFromNItr::operator==(const KFromNItr &other) const
  {
    // All past-the-end iterators compare equal
    if (exhausted() && other.exhausted())
      return true;
    // Otherwise make sure that the iterators point to the same combination
    return m_current == other.m_current;
  }

  bool KFromNItr::operator!=(const KFromNItr &other) const
  {
    return !(*this == other);
  }
} //> end namespace TrigCompositeUtils