/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tcls1.cxx,v 1.6 2009-01-21 04:34:40 ssnyder Exp $
/**
 * @file tcls1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Classes for @c TConvertingBranchElement regression tests.
 *
 * This file has the implementations for the classes used for the writing side
 * of the regression tests.
 */


#include "tcls1.h"


std::ostream& operator<< (std::ostream& s, const A& a)
{
  s << a.a << " " << a.t;
  return s;
}


std::ostream& operator<< (std::ostream& s, const B& b)
{
  s << b.d << " " << (const T1&)b;
  return s;
}


std::ostream& operator<< (std::ostream& s, const D& d)
{
  s << d.g;
  return s;
}


std::ostream& operator<< (std::ostream& s, const D2& d)
{
  s << d.gg;
  return s;
}


std::ostream& operator<< (std::ostream& s, const H& c)
{
  s << c.m << " " << c.t4;
  return s;
}

std::ostream& operator<< (std::ostream& s, const QX& c)
{
  s << (const D2&)c << " " << c.t1;
  return s;
}

std::ostream& operator<< (std::ostream& s, const T1& t)
{
  s << t.a << " " << t.b << " " << t.c;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T2& t)
{
  s << t.t << " " << t.e;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T3& t)
{
  s << (const D&)t << " " << t.f;
  return s;
}

std::ostream& operator<< (std::ostream& s, const T4& t)
{
  s << t.l << " " << t.d;
  return s;
}

std::ostream& operator<< (std::ostream& s, const T5& t)
{
  s << t.i5 << " " << t.a;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T6& t)
{
  s << t.i6 << " " << (const B&)t;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T7& t)
{
  s << t.i7 << " " << (const QX&)t;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T8& t)
{
  s << t.a << " " << t.h << " " << t.i8 << " "
    << (const T3&)t << " " << (const T7&)t;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T9& t)
{
  s << t.i9 << " " << t.b;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T10& t)
{
  s << t.a << " " << t.b << " " << t.c;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T11a& t)
{
  s << t.a;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T11b& t)
{
  s << t.b;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T11& t)
{
  s << (const T11a&)t << " " << (const T11b&)t << " " << t.c;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T12& t)
{
  s << t.i12 << " " << t.d;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T13& t)
{
  s << t.a << " " << t.b << " " << t.c;
  return s;
}


std::ostream& operator<< (std::ostream& s, const T14& t)
{
  s << "[";
  for (unsigned i=0; i < t.a.size(); i++)
    s << t.a[i] << " ";
  s << "][";
  for (unsigned i=0; i < t.b.size(); i++)
    s << t.b[i] << " ";
  s << "][";
  for (unsigned i=0; i < t.c.size(); i++)
    s << t.c[i] << " ";
  s << "][";
  for (unsigned i=0; i < t.d.size(); i++)
    s << t.d[i] << " ";
  s << "]";
  return s;
}


std::ostream& operator<< (std::ostream& s, const T15& t)
{
  s << *t.a;
  return s;
}


#include "tcls_common.cxx"



