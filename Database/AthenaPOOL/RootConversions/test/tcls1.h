// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: tcls1.h,v 1.5 2009-01-14 16:04:31 ssnyder Exp $
/**
 * @file tcls1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Classes for @c TConvertingBranchElement regression tests.
 *
 * This header defines the classes used for the writing side
 * of the regression tests.
 */


#ifndef TCLS1_HH
#define TCLS1_HH

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
  int b;
  int c;
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
  int e;
};

class T3
  : public D
{
public:
  int f;
};

class T4
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
  int i5;
  A a;
};

class T6
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
  int i7;
};

class T8
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
  int i9;
};

class T10
{
public:
  int a;
  int b;
  int c;
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

class T11
  :  public T11a, public T11b
{
public:
  int c;
};

class T12
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
  int b;
  int c;
  ClassDef(T13, 1);
};

class T14
{
public:
  std::vector<double> a;
  std::vector<double> b;
  std::vector<unsigned long> c;
  std::vector<unsigned long long> d;
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
std::ostream& operator<< (std::ostream& s, const T11a& t);
std::ostream& operator<< (std::ostream& s, const T11b& t);
std::ostream& operator<< (std::ostream& s, const T11& t);
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

//done:
//  1 changed obj
//  2 obj containing changed obj
//  3 obj deriving from changed obj
//  4 obj containing changed obj contained changed obj
//  5 obj deriving from changed obj deriving from obj
//  6 obj deriving from obj and changed obj
//  7 obj (deriving from changed obj) containing changed obj
//  8 obj containing obj containing changed obj containing obj
//  9 obj C obj C obj C changed obj
// 10 obj C changed obj C obj C changed obj
// 11 obj D obj D changed obj D obj
// 12 obj D obj D obj D changed obj
// 13 obj D changed obj D obj D changed obj
// 14 obj D (obj C changed obj)
// 15 changed obj C (obj C changed obj)
//                C (obj C changed obj C obj)
//                D (changed obj D obj)
//                D (changed obj D (obj C changed obj) D obj)
// 16 obj C (changed obj D obj)
// 17 obj C (obj D changed obj)
// 18 obj C (obj D changed obj D obj) C (changed obj C obj D changed obj)
// 19 obj -> obj C obj D obj
// 20 obj (D obj) (D obj) -> obj C obj C obj

//todo:
#endif
