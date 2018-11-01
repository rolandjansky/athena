/* Dear emacs, this is -*-c++-*- */
#ifndef _ObjectAccessor_H_
#define _ObjectAccessor_H_

#include <array>
#include <ostream>

class MsgStream;

namespace Trk {
class ObjectAccessor
{
public:
  typedef int value_type;
  using const_iterator = std::array<value_type, 99>::const_iterator ; // 99 is a dummy dimension. For all N the type of
                                                                      // std::array<int, N>::const_iterator is the same

  template <size_t DIM>
  ObjectAccessor( const std::array<int, DIM> &a, bool is_reverse=false)
    : m_begin(a.begin()),
      m_end(a.end()),
      m_inverseRetrieval(is_reverse)
  {}

  template <size_t DIM>
  ObjectAccessor( const std::pair<std::array<int, DIM> , bool>  &a)
    : m_begin(a.first.begin()),
      m_end(a.first.end()),
      m_inverseRetrieval(a.second)
  {}

  ObjectAccessor(ObjectAccessor::const_iterator the_begin,
                 ObjectAccessor::const_iterator the_end,
                 bool inverse_retrieval=false)
    : m_begin(the_begin),
      m_end(the_end),
      m_inverseRetrieval(inverse_retrieval)
  {}

  ObjectAccessor::const_iterator begin() const { return m_begin; }
  ObjectAccessor::const_iterator end()   const { return m_end; }

  bool inverseRetrieval() const { return m_inverseRetrieval; }

private:
  ObjectAccessor::const_iterator m_begin;
  ObjectAccessor::const_iterator m_end;
  bool m_inverseRetrieval;
};

MsgStream& operator << ( MsgStream& sl, const ObjectAccessor& oac);
std::ostream& operator << ( std::ostream& sl, const ObjectAccessor& oac);

}
#endif
