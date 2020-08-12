/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TEST_FOOBAR_H
# define TEST_FOOBAR_H

#include "xAODCore/AuxContainerBase.h"
#include "AthContainers/AuxElement.h"
#include "AthenaKernel/CLASS_DEF.h"


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

class Baz
  : public SG::AuxElement
{
};
SG_BASE (Baz, SG::AuxElement);
CLASS_DEF( Baz, 8111, 0)


class BazAuxContainer
  : public xAOD::AuxContainerBase
{
};
SG_BASE (BazAuxContainer, xAOD::AuxContainerBase);
CLASS_DEF( BazAuxContainer, 8112, 0)


#endif // TEST_FOOBAR_H
