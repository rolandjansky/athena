// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tcls_common.h,v 1.4 2009-01-14 16:04:31 ssnyder Exp $
/**
 * @file tcls_common.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Classes for @c TConvertingBranchElement regression tests.
 *
 * This header defines classes that are common between the reading
 * and writing sides of the regression tests.
 */


#include <vector>
#include <list>
#include <set>
#include <map>
#include "TObject.h"
#include "TClonesArray.h"


bool operator< (const T1& a, const T1& b);


class C
{
public:
  T2 u;
  int cc;
};

class E
  : public T3
{
public:
  int h;
};


class FX // Note: `F' triggers a reflex bug as of 5.18 (bug 32916)
  : public D, public T1
{
public:
  int j;
};


class G
  : public T1
{
public:
  int k;
  T1 t;
};


class I
{
public:
  int n;
  H i;
};


class J
{
public:
  int ij;
  A a;
};


class K
{
public:
  int ik;
  J j;
};


class L
{
public:
  int il;
  T5 t5;
};


class M
  : public E
{
public:
  int im;
};


class N
  : public B
{
public:
  int in;
};


class O
  : public N
{
public:
  int io;
};


class P
  : public T6
{
public:
  int ip;
};


class R
  : public A
{
};

class SX // Note: `S' triggers a reflex bug as of 5.18 (bug 32916)
{
public:
  int is;
  T3 t3;
};

class U
{
public:
  int iu;
  B b;
};

class V
{
public:
  int iv;
  E e;
  T9 t9;
};
bool operator< (const V& a, const V& b);


class W
{
public:
  T1 t1[3];
  int iw;
};


class XX
{
public:
  std::vector<T1> t1;
  int ix;
};


class YY
{
public:
  int iy;
  std::vector<I> vi;
};


class ZZ
{
public:
  int iz;
  YY yy;
};


class AA
{
public:
  int iaa;
  std::vector<YY> vy;
};


class BB
{
public:
  int ibb;
  std::vector<V> vv;
};


class CC
{
public:
  T9 t9;
};


class DD
{
public:
  int idd;
  std::vector<CC> vcc;
};


class EE
{
public:
  T12 t12;
};


class FF
{
public:
  int iff;
  std::vector<EE> vee;
};


class GG
{
public:
  int igg;
  std::vector<T8> vt8;
};


class HH
{
public:
  int ihh;
  std::list<T8> lt8;
};
// Needed to make dict for list<T8>
inline bool operator< (const T8&, const T8&) { return false; }
inline bool operator== (const T8&, const T8&) { return false; }


class II
{
public:
  int iii;
  std::set<T1> st1;
};


class JJ
{
public:
  int ijj;
  std::set<V> sv;
};


class KK
{
public:
  int ikk;
  std::map<int, T1> mt1;
};


class LL
  : public TObject
{
public:
  int ill;
  T1 t1;
  ClassDef (LL, 1);
};


class MM
{
public:
  MM (int size = 0);
  int imm;
  TClonesArray mo;
};

class NN
{
public:
  NN (int size = 0);
  int inn;
  TClonesArray no;
};


class OO
  : public TObject, public V
{
public:
  int ioo;
  ClassDef (OO, 1);
};


class PP
{
public:
  PP (int size = 0);
  int ipp;
  TClonesArray po;
};


class QQ
  : public T2, public std::vector<T1>
{
public:
  int iqq;
  T3 t3;
};


class RR
{
public:
  RR();
  ~RR();
  int irr;
  T1* t1;
};


class SS
{
public:
  ~SS();
  void clear();
  int iss;
  std::vector<T1*> vt1;
};



std::ostream& operator<< (std::ostream& s, const C& c);
std::ostream& operator<< (std::ostream& s, const E& c);
std::ostream& operator<< (std::ostream& s, const FX& c);
std::ostream& operator<< (std::ostream& s, const G& c);
std::ostream& operator<< (std::ostream& s, const I& c);
std::ostream& operator<< (std::ostream& s, const J& c);
std::ostream& operator<< (std::ostream& s, const K& c);
std::ostream& operator<< (std::ostream& s, const L& c);
std::ostream& operator<< (std::ostream& s, const M& c);
std::ostream& operator<< (std::ostream& s, const N& c);
std::ostream& operator<< (std::ostream& s, const O& c);
std::ostream& operator<< (std::ostream& s, const P& c);
std::ostream& operator<< (std::ostream& s, const R& c);
std::ostream& operator<< (std::ostream& s, const SX& c);
std::ostream& operator<< (std::ostream& s, const U& c);
std::ostream& operator<< (std::ostream& s, const V& c);
std::ostream& operator<< (std::ostream& s, const W& c);
std::ostream& operator<< (std::ostream& s, const XX& c);
std::ostream& operator<< (std::ostream& s, const YY& c);
std::ostream& operator<< (std::ostream& s, const ZZ& c);
std::ostream& operator<< (std::ostream& s, const AA& c);
std::ostream& operator<< (std::ostream& s, const BB& c);
std::ostream& operator<< (std::ostream& s, const CC& c);
std::ostream& operator<< (std::ostream& s, const DD& c);
std::ostream& operator<< (std::ostream& s, const EE& c);
std::ostream& operator<< (std::ostream& s, const FF& c);
std::ostream& operator<< (std::ostream& s, const GG& c);
std::ostream& operator<< (std::ostream& s, const HH& c);
std::ostream& operator<< (std::ostream& s, const II& c);
std::ostream& operator<< (std::ostream& s, const JJ& c);
std::ostream& operator<< (std::ostream& s, const KK& c);
std::ostream& operator<< (std::ostream& s, const LL& c);
std::ostream& operator<< (std::ostream& s, const MM& c);
std::ostream& operator<< (std::ostream& s, const NN& c);
std::ostream& operator<< (std::ostream& s, const OO& c);
std::ostream& operator<< (std::ostream& s, const PP& c);
std::ostream& operator<< (std::ostream& s, const QQ& c);
std::ostream& operator<< (std::ostream& s, const RR& c);
std::ostream& operator<< (std::ostream& s, const SS& c);

