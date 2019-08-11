/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
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
  // @TODO ideally would use "using const_iterator std::array_base<value_type>::const_iterator" if it existed
  typedef const value_type* const_iterator;

  template<size_t DIM>
  ObjectAccessor(const std::array<value_type, DIM>& a, bool is_reverse = false)
    : m_begin(a.begin())
    , m_end(a.end())
    , m_inverseRetrieval(is_reverse)
  {}

  template<size_t DIM>
  ObjectAccessor(const std::pair<std::array<value_type, DIM>, bool>& a)
    : m_begin(a.first.begin())
    , m_end(a.first.end())
    , m_inverseRetrieval(a.second)
  {}

  ObjectAccessor::const_iterator begin() const { return m_begin; }
  ObjectAccessor::const_iterator end() const { return m_end; }

  bool inverseRetrieval() const { return m_inverseRetrieval; }

private:
  ObjectAccessor::const_iterator m_begin;
  ObjectAccessor::const_iterator m_end;
  bool m_inverseRetrieval;
};

MsgStream&
operator<<(MsgStream& sl, const ObjectAccessor& oac);
std::ostream&
operator<<(std::ostream& sl, const ObjectAccessor& oac);

}
#endif
