// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaROOTAccess/TestTypes.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2014
 * @brief Types for unit testing in AthenaROOTAccess.
 */


#ifndef ATHENAROOTACCESS_TESTTYPES_H
#define ATHENAROOTACCESS_TESTTYPES_H


#include "AthContainers/DataVector.h"
#include "AthenaKernel/BaseInfo.h"


namespace ARATest {


struct A
{
  A (int the_x) : x (the_x) {}
  virtual ~A() {}
  int x;
};


struct B
{
  B (int the_x) : b(the_x) {} 
  int b;
};


struct C
  : public B, public A
{
  C (int x) : B(x+10), A(x), c(x+20) {}
  int c;
};


struct D
{
  D (int the_x) : x (the_x) {}
  virtual ~D() {}
  int x;
};


struct E
  : virtual public D
{
  E(int x) : D(x), e(x+10) {}
  int e;
};


struct F
  : virtual public D
{
  F(int x) : D(x), f(x+20) {}
  int f;
};


struct H
{
  H(int x) : h(x) {}
  virtual ~H() {}
  int h;
};


struct G
  : virtual public E, virtual public F, public H
{
  G(int x) : D(x), E(x+10), F(x+20), H(x+40), g(x+30) {}

  // Avoid gcc4.8 warning
  G(const G& other) : D(other), E(other), F(other), H(other), g(other.g) {}
  G(G&& other) : D(other), E(other), F(other), H(other), g(other.g) {}
  G& operator= (const G& other)
  { if (this != &other) { x = other.x; e = other.e; f = other.f; g = other.g; 
                          h = other.h; }
    return *this;
  }
  G& operator= (G&& other)
  { if (this != &other) { x = other.x; e = other.e; f = other.f; g = other.g; 
                          h = other.h ;}
    return *this;
  }

  int g;
};


} // namespace ARATest


DATAVECTOR_BASE (ARATest::C, ARATest::A);

DATAVECTOR_VIRTBASES1 (ARATest::E, ARATest::D);
DATAVECTOR_VIRTBASES1 (ARATest::F, ARATest::D);
DATAVECTOR_VIRTBASES2 (ARATest::G, ARATest::E, ARATest::F);

SG_ADD_BASE (ARATest::G, ARATest::H);

namespace ARATest {


typedef DataVector<A> AVec;
typedef DataVector<C> CVec;
typedef DataVector<D> DVec;
typedef DataVector<E> EVec;
typedef DataVector<F> FVec;
typedef DataVector<G> GVec;
typedef DataVector<H> HVec;

class EDer : public EVec
{
public:
  int m_dummy[20];
};


} // namespace ARATest


#endif // not ATHENAROOTACCESS_TESTTYPES_H
