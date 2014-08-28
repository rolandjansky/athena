// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PyDumper/PySTLAdaptor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Helper for iterating over STL map classes.
 */


#ifndef PYDUMPER_PYSTLADAPTOR_H
#define PYDUMPER_PYSTLADAPTOR_H


namespace PyDumper {


/**
 * @brief Helper for iterating over STL map classes.
 *
 * It's difficult to generate proper reflex dictionaries for iterators
 * over STL map classes, as (as least with libstdc++) one also needs
 * dictionaries for bases of the iterators that have internal libstdc++
 * names.  This adaptor class provides simple iterators for which it
 * is easy to generate dictionaries.
 *
 * While the principal use of this is expected to be map classes,
 * it should work for any STL-compliant container type.
 */
template <class C>
class PySTLAdaptor
{
public:
  class iterator
  {
  public:
    iterator (typename C::const_iterator it) : m_it (it) {}
    typename C::const_iterator::value_type operator*() { return *m_it; }
    iterator& operator++() { ++m_it;  return *this; }
    bool operator!= (const iterator& other) const { return m_it != other.m_it; }
  private:
    typename C::const_iterator m_it;
  };


  PySTLAdaptor (const C& c) : m_c (&c) {}
  iterator begin() const { return m_c->begin(); }
  iterator end() const { return m_c->end(); }


private:
  const C* m_c;
};


} // namespace PyDumper


#endif // not PYDUMPER_PYSTLADAPTOR_H
