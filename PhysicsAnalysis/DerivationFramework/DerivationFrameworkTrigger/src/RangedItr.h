/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKTRIGGER_RANGEDITR_H
#define DERIVATIONFRAMEWORKTRIGGER_RANGEDITR_H

#include <vector>
#include <iterator>
#include <type_traits>

namespace DerivationFramework { namespace TriggerMatchingUtils {
/**
 * @brief utility class that acts wraps a bidirectional iterator.
 * This class acts as both and range and an iterator.
 */
template <typename T>
  class RangedItr {
    protected:
      using tr = std::iterator_traits<T>;
      static_assert(std::is_base_of<std::bidirectional_iterator_tag, typename tr::iterator_category>::value, "Templated type must be a random access iterator");
    public:
      // A full implementation of this idea would allow for any input
      // iterator type. For this version, just require at least bidirectional

      using iterator_category = std::bidirectional_iterator_tag;
      using value_type      = typename tr::value_type;
      using reference       = typename tr::reference;
      using pointer         = typename tr::pointer;
      using difference_type = typename tr::difference_type;

      /// Default Constructor
      RangedItr() = default;

      /// Construct from a beginning and an end
      RangedItr(const T& begin, const T& end) : 
        RangedItr(begin, end, begin) {}

      /// Construct from a beginning, an end and a starting position
      RangedItr(const T& begin, const T& end, const T& position) :
        m_begin(begin), m_end(end), m_position(position) {}

      /// Vector interface
      reference operator*() { return m_position.operator*(); }
      pointer operator->() { return m_position.operator->(); }

      RangedItr& operator++() {
        ++m_position;
        return *this;
      }
      RangedItr& operator++(int) {
        RangedItr ret = *this;
        ++m_position;
        return ret;
      }

      RangedItr& operator--() {
        --m_position;
        return *this;
      }
      RangedItr& operator--(int) {
        RangedItr ret = *this;
        --m_position;
        return ret;
      }

      bool operator==(const RangedItr& other) {
        return m_begin == other.m_begin &&
                m_end == other.m_end &&
                m_position == other.m_position;
      }

      bool operator!=(const RangedItr& other) {
        return !(*this == other);
      }

      /// Is this iterator exhausted?
      bool exhausted() const { return m_position == m_end; }

      /// Reset this iterator to its start
      void restart() { m_position = m_begin; }

      /// Make this act as a range
      RangedItr begin() { return RangedItr(m_begin, m_end, m_begin); }
      RangedItr end() { return RangedItr(m_begin, m_end, m_end); }
    private:
      T m_begin;
      T m_end;
      T m_position;

  }; //> end class RangedItr
} } //> end namespace DerivationFramework::TriggerMatchingUtils
#endif //> !DERIVATIONFRAMEWORKTRIGGER_RANGEDITR_H
