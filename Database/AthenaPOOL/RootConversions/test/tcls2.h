// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tcls2.h,v 1.5 2009-01-14 16:04:31 ssnyder Exp $
/**
 * @file tcls2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Classes for @c TConvertingBranchElement regression tests.
 *
 * This header defines the classes used for the reading side
 * of the regression tests.
 */


#ifndef TCLS2_HH
#define TCLS2_HH

#include <ostream>
#include <vector>
#include "TObject.h"


class D
{
public:
  int g;
};

class D2
{
public:
  int gg;
};

class T1
{
public:
  virtual ~T1() {}
  int a;
  double bb;
  int c;
};

class T1_old
{
public:
  virtual ~T1_old() {}
  int a;
  int c; // note: swapped!
  int b;
};

class A
{
public:
  int a;
  T1 t;
};

class B
  : public T1
{
public:
  int d;
};

class T2
{
public:
  T1 t;
  double ee;
};

class T2_old
{
public:
  T1 t;
  int e;
};

class T3
  : public D
{
public:
  double ff;
};

class T3_old
  : public D
{
public:
  int f;
};

class T4
{
public:
  double ll;
  D d;
};

class T4_old
{
public:
  int l;
  D d;
};


class H
{
public:
  int m;
  T4 t4;
};


class T5
{
public:
  double ii5;
  A a;
};

class T5_old
{
public:
  int i5;
  A a;
};

class T6
  : public B
{
public:
  double ii6;
};

class T6_old
  : public B
{
public:
  int i6;
};

class QX // Note: `Q' triggers a reflex bug as of 5.18 (bug 32916)
  : public D2
{
public:
  T1 t1;
};

class T7
  : public QX
{
public:
  double ii7;
};

class T7_old
  : public QX
{
public:
  int i7;
};

class T8
  : public T3, public T7
{
public:
  A a;
  H h;
  double ii8;
};

class T8_old
  : public T3, public T7
{
public:
  A a;
  H h;
  int i8;
};

class T9
{
public:
  B b;
  double ii9;
};

class T9_old
{
public:
  B b;
  int i9;
};

class T10a
{
public:
  double aa;
};

class T10c
{
public:
  float cc;
};

class T10
  : public T10a
{
public:
  short bb;
  T10c c;
};

class T10_old
{
public:
  int a;
  int b;
  int c;
};

class T11xc
{
public:
  double cc;
};

class T11xb
{
public:
  float bb;
  T11xc c;
};

class T11
{
public:
  short aa;
  T11xb b;
};

class T11a
{
public:
  int a;
};

class T11b
{
public:
  int b;
};

class T11_old
  :  public T11a, public T11b
{
public:
  int c;
};

class T12
{
public:
  D d;
  double ii12;
};

class T12_old
{
public:
  D d;
  int i12;
};

class T13
  : public TObject
{
public:
  int a;
  double bb;
  int c;
  ClassDef(T13, 2);
};

class T13_old
  : public TObject
{
public:
  int a;
  int b;
  int c;
  ClassDef(T13_old, 1);
};

class T14
{
public:
  std::vector<float> a;
  std::vector<double> b;
  std::vector<unsigned long long> c;
  std::vector<unsigned long> d;
};

class T14_dum
{
public:
  std::vector<double> a;
};

class T15
{
public:
  T15() : a(0) {}
  T14* a;
};

std::ostream& operator<< (std::ostream& s, const T1& t);
std::ostream& operator<< (std::ostream& s, const T2& t);
std::ostream& operator<< (std::ostream& s, const T3& t);
std::ostream& operator<< (std::ostream& s, const T4& t);
std::ostream& operator<< (std::ostream& s, const T5& t);
std::ostream& operator<< (std::ostream& s, const T6& t);
std::ostream& operator<< (std::ostream& s, const T7& t);
std::ostream& operator<< (std::ostream& s, const T8& t);
std::ostream& operator<< (std::ostream& s, const T9& t);
std::ostream& operator<< (std::ostream& s, const T10& t);
std::ostream& operator<< (std::ostream& s, const T10a& t);
std::ostream& operator<< (std::ostream& s, const T10c& t);
std::ostream& operator<< (std::ostream& s, const T11& t);
std::ostream& operator<< (std::ostream& s, const T11xb& t);
std::ostream& operator<< (std::ostream& s, const T11xc& t);
std::ostream& operator<< (std::ostream& s, const T12& t);
std::ostream& operator<< (std::ostream& s, const T13& t);
std::ostream& operator<< (std::ostream& s, const T14& t);
std::ostream& operator<< (std::ostream& s, const T15& t);
std::ostream& operator<< (std::ostream& s, const A& a);
std::ostream& operator<< (std::ostream& s, const B& a);
std::ostream& operator<< (std::ostream& s, const D& d);
std::ostream& operator<< (std::ostream& s, const D2& d);
std::ostream& operator<< (std::ostream& s, const H& c);
std::ostream& operator<< (std::ostream& s, const QX& c);

#include "tcls_common.h"

#endif
