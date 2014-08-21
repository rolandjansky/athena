/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tcls_common.cxx,v 1.5 2009-01-21 04:34:40 ssnyder Exp $
/**
 * @file tcls_common.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Classes for @c TConvertingBranchElement regression tests.
 *
 * This file has the implementations for the classes common between
 * the reading and writing sides of the regression tests.
 */


std::ostream& operator<< (std::ostream& s, const C& c)
{
  s << c.cc << " " << c.u;
  return s;
}

std::ostream& operator<< (std::ostream& s, const E& c)
{
  s << (const T3&)c << " " << c.h;
  return s;
}

std::ostream& operator<< (std::ostream& s, const FX& c)
{
  s << (const D&)c << " " << (const T1&)c << " " << c.j;
  return s;
}

std::ostream& operator<< (std::ostream& s, const G& c)
{
  s << (const T1&)c << " " << c.k << " " << c.t;
  return s;
}

std::ostream& operator<< (std::ostream& s, const I& c)
{
  s << c.n << " " << c.i;
  return s;
}

std::ostream& operator<< (std::ostream& s, const J& c)
{
  s << c.ij << " " << c.a;
  return s;
}

std::ostream& operator<< (std::ostream& s, const K& c)
{
  s << c.ik << " " << c.j;
  return s;
}

std::ostream& operator<< (std::ostream& s, const L& c)
{
  s << c.il << " " << c.t5;
  return s;
}

std::ostream& operator<< (std::ostream& s, const M& c)
{
  s << c.im << " " << (const E&)c;
  return s;
}

std::ostream& operator<< (std::ostream& s, const N& c)
{
  s << c.in << " " << (const B&)c;
  return s;
}

std::ostream& operator<< (std::ostream& s, const O& c)
{
  s << c.io << " " << (const N&)c;
  return s;
}

std::ostream& operator<< (std::ostream& s, const P& c)
{
  s << c.ip << " " << (const T6&)c;
  return s;
}

std::ostream& operator<< (std::ostream& s, const R& c)
{
  s << (const A&)c;
  return s;
}

std::ostream& operator<< (std::ostream& s, const SX& c)
{
  s << c.is << " " << c.t3;
  return s;
}

std::ostream& operator<< (std::ostream& s, const U& c)
{
  s << c.iu << " " << c.b;
  return s;
}

std::ostream& operator<< (std::ostream& s, const V& c)
{
  s << c.iv << " " << c.e << " " << c.t9;
  return s;
}

std::ostream& operator<< (std::ostream& s, const W& c)
{
  s << c.iw;
  for (int i=0; i<3; i++)
    s << " " << c.t1[i];
  return s;
}

std::ostream& operator<< (std::ostream& s, const XX& c)
{
  s << c.ix;
  for (unsigned i=0; i<c.t1.size(); i++)
    s << " " << c.t1[i];
  return s;
}

std::ostream& operator<< (std::ostream& s, const YY& c)
{
  s << c.iy;
  for (unsigned i=0; i<c.vi.size(); i++)
    s << " " << c.vi[i];
  return s;
}

std::ostream& operator<< (std::ostream& s, const ZZ& c)
{
  s << c.iz << " " << c.yy;
  return s;
}

std::ostream& operator<< (std::ostream& s, const AA& c)
{
  s << c.iaa;
  for (unsigned i=0; i<c.vy.size(); i++)
    s << " " << c.vy[i];
  return s;
}

std::ostream& operator<< (std::ostream& s, const BB& c)
{
  s << c.ibb;
  for (unsigned i=0; i<c.vv.size(); i++)
    s << " " << c.vv[i];
  return s;
}


std::ostream& operator<< (std::ostream& s, const CC& c)
{
  s << c.t9;
  return s;
}


std::ostream& operator<< (std::ostream& s, const DD& c)
{
  s << c.idd;
  for (unsigned i=0; i<c.vcc.size(); i++)
    s << " " << c.vcc[i];
  return s;
}


std::ostream& operator<< (std::ostream& s, const EE& c)
{
  s << c.t12;
  return s;
}


std::ostream& operator<< (std::ostream& s, const FF& c)
{
  s << c.iff;
  for (unsigned i=0; i<c.vee.size(); i++)
    s << " " << c.vee[i];
  return s;
}


std::ostream& operator<< (std::ostream& s, const GG& c)
{
  s << c.igg;
  for (unsigned i=0; i<c.vt8.size(); i++)
    s << " " << c.vt8[i];
  return s;
}


std::ostream& operator<< (std::ostream& s, const HH& c)
{
  s << c.ihh;
  for (std::list<T8>::const_iterator i = c.lt8.begin();
       i != c.lt8.end();
       ++i)
  {
    s << " " << *i;
  }
  return s;
}


std::ostream& operator<< (std::ostream& s, const II& c)
{
  s << c.iii;
  for (std::set<T1>::const_iterator i = c.st1.begin();
       i != c.st1.end();
       ++i)
  {
    s << " " << *i;
  }
  return s;
}


std::ostream& operator<< (std::ostream& s, const JJ& c)
{
  s << c.ijj;
  for (std::set<V>::const_iterator i = c.sv.begin();
       i != c.sv.end();
       ++i)
  {
    s << " " << *i;
  }
  return s;
}


std::ostream& operator<< (std::ostream& s, const KK& c)
{
  s << c.ikk;
  for (std::map<int, T1>::const_iterator i = c.mt1.begin();
       i != c.mt1.end();
       ++i)
  {
    s << " " << i->first << " " << i->second;
  }
  return s;
}


std::ostream& operator<< (std::ostream& s, const LL& c)
{
  s << c.ill << " " << c.t1;
  return s;
}


std::ostream& operator<< (std::ostream& s, const MM& c)
{
  s << c.imm;
  size_t n = c.mo.GetEntriesFast();
  for (size_t i = 0; i < n; i++) {
    if (c.mo[i])
      s << " " << *(LL*)(c.mo[i]);
  }
  return s;
}


std::ostream& operator<< (std::ostream& s, const NN& c)
{
  s << c.inn;
  size_t n = c.no.GetEntriesFast();
  for (size_t i = 0; i < n; i++) {
    if (c.no[i]) {
      T13* t13 = dynamic_cast<T13*>(c.no[i]);
      if (t13)
        s << " " << *t13;
    };
  }
  return s;
}


std::ostream& operator<< (std::ostream& s, const OO& c)
{
  s << c.ioo << " " << (const V&)c;
  return s;
}


std::ostream& operator<< (std::ostream& s, const PP& c)
{
  s << c.ipp;
  size_t n = c.po.GetEntriesFast();
  for (size_t i = 0; i < n; i++) {
    if (c.po[i]) {
      OO* oo = dynamic_cast<OO*>(c.po[i]);
      if (oo)
        s << " " << *oo;
    }
  }
  return s;
}


std::ostream& operator<< (std::ostream& s, const QQ& c)
{
  s << c.iqq << " " << (const T2&)c << " " << c.t3;
  for (unsigned i = 0; i < c.size(); i++)
    s << " " << c[i];
  return s;
}


std::ostream& operator<< (std::ostream& s, const RR& c)
{
  s << c.irr << " ";
  if (c.t1)
    s << *c.t1;
  else
    s << "(null)";
  return s;
}


std::ostream& operator<< (std::ostream& s, const SS& c)
{
  s << c.iss;
  for (unsigned i = 0; i < c.vt1.size(); i++) {
    if (c.vt1[i])
      s << " " << *c.vt1[i];
    else
      s << " (null)";
  }
  return s;
}


bool operator< (const T1& a, const T1& b)
{
  return a.a < b.a;
}


bool operator< (const V& a, const V& b)
{
  return a.iv < b.iv;
}


MM::MM (int size)
  : imm (0),
    mo ("LL", size)
{
}


NN::NN (int size)
  : inn (0),
    no ("T13", size)
{
}


PP::PP (int size)
  : ipp (0),
    po ("OO", size)
{
}


RR::RR ()
  : irr (0),
    t1 (0)
{
}


RR::~RR()
{
  delete t1;
}


void SS::clear()
{
  for (unsigned int i=0; i < vt1.size(); i++)
    delete vt1[i];
  vt1.clear();
}


SS::~SS()
{
  clear();
}




