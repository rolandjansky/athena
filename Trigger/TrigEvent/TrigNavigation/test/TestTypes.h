/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_TESTTYPES_H
#define TRIGNAVIGATION_TESTTYPES_H

#include "AthContainers/AuxElement.h"
#include "AthContainers/ViewVector.h"
#include "xAODCore/AuxContainerBase.h"


#define TRIGSTORAGEDEF_MERGEPACKS
#include "TrigNavigation/TypeRegistration.h"

namespace TrigNavTest {
  class TestB;
  class TestBContainer;
  class TestAuxB;
}

#include "TrigNavigation/TestTypesB.h"

class TestA : public SG::AuxElement {
public:
  TestA(){a = 0;}
  TestA(int arg){a = arg;}
  ~TestA() { std::cerr << " deleting TestA: "<< a << std::endl; }
  int value() const { return a; }
  int a;
};


CLASS_DEF(TestA, 6421, 1)
CLASS_DEF(DataVector<TestA>, 64210, 1)
typedef DataVector<TestA> TestAContainer;


class TestAuxA : public xAOD::AuxContainerBase {
public:
  std::vector<float> detail;
};
DATAVECTOR_BASE(TestAuxA,xAOD::AuxContainerBase);
CLASS_DEF(TestAuxA, 642300, 1)


class TestC {
public:
  TestC() :c(0) {}
  TestC(int v) : c(v) {}
  ~TestC()  {std::cerr << "Deleting TestC" << std::endl; }
  int value() const { return c; }
  int c;
};

CLASS_DEF(TestC, 7800, 1)
CLASS_DEF(DataVector<TestC>, 78001, 1)

class TestCtypeContainer : public DataVector<TestC> { };
CLASS_DEF(TestCtypeContainer, 78002, 1)

class TestD {
public:
  TestD() : b(0) {  }
  TestD(int v) : b(v) {  }
  ~TestD() { std::cerr << " deleting TestD: "<< b << std::endl; }
  int value() const { return b; }
  int b;
};


class TestDContainer : public DataVector<TestD> {
public:

}; 

CLASS_DEF(TestDContainer, 96477, 1)

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(class TestA, class TestA, class DataVector<TestA>, class TestAuxA)
  HLT_REGISTER_TYPE(TrigNavTest::TestB, TrigNavTest::TestBContainer, TrigNavTest::TestBContainer, TrigNavTest::TestAuxB)
  HLT_REGISTER_TYPE(TrigNavTest::TestB, TrigNavTest::TestBContainerView, TrigNavTest::TestBContainerView)
  HLT_REGISTER_TYPE(class TestC, class TestC, class DataVector<TestC>)
  HLT_REGISTER_TYPE(class TestD, class TestDContainer, class TestDContainer)
HLT_END_TYPE_REGISTRATION(TrigNavigationUnitTest)

#define LTYPEMAPCLASS(name)\
  struct class_##name{\
    typedef TypeInfo_##name map;\
    static const std::string package_name;\
  };

LTYPEMAPCLASS(TrigNavigationUnitTest)

struct TypeInfo_EDM {
  typedef  HLT::TypeInformation::newlist
  ::add<class_TrigNavigationUnitTest>  ::go
  ::done map;
};
REGISTER_PACKAGE_WITH_NAVI(TrigNavigationUnitTest)

#endif // not TRIGNAVIGATION_TESTTYPES_H
