/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TEST_FOOBAR_H
# define TEST_FOOBAR_H

#include "SGTools/CLASS_DEF.h"
class Foo{
private:
  double m_a;
public:
  Foo(): m_a(0.0) { 
    //    std::cout << "Foo() called @" << this <<std::endl;
  }
  Foo(double a): m_a(a) {
    //    std::cout << "Foo() called @" << this << std::endl;
  }
  ~Foo() {
    //    std::cout << "~Foo() called @" << this << std::endl;
  }
  void setA(double a) { m_a=a; }
  double a() const { return m_a; }
  void doNothing() const {};
};
CLASS_DEF( Foo, 8101, 0) 
struct Bar{
  void doNothing() const {};
};
CLASS_DEF( Bar, 8107, 0) 

struct Fee{
  void doNothing() const {};
};
CLASS_DEF( Fee, 8108, 0) 

#endif // TEST_FOOBAR_H
