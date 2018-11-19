// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigNavigation/TestTypesB.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief Types used for testing for which we need dictionaries.
 */


#ifndef TRIGNAVIGATION_TESTTYPESB_H
#define TRIGNAVIGATION_TESTTYPESB_H


#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"
#include "xAODCore/AuxContainerBase.h"


namespace TrigNavTest {


class TestB : public SG::AuxElement{
public:
  TestB() : b(0) {  }
  TestB(int v) : b(v) {  }
  ~TestB() { std::cerr << " deleting TestB: "<< b << std::endl; }
  int value() const { return b; }
  int b;
};


class TestBContainer : public DataVector<TestB> {
public:
  TestBContainer(SG::OwnershipPolicy policy = SG::OWN_ELEMENTS)
    : DataVector<TestB>(policy) { }
}; 


class TestBContainerView : public ViewVector<TestBContainer> {
 public:
  TestBContainerView() 
    : ViewVector<TestBContainer>(SG::VIEW_ELEMENTS){}
};


class TestAuxB : public xAOD::AuxContainerBase {
public:
  TestAuxB() { AUX_VARIABLE (detail); }
  std::vector<float> detail;
};
//HLT::AuxInit<TestAuxB> t;


} // namespace TrigNavTest


CLASS_DEF(TrigNavTest::TestB, 96424, 1)
CLASS_DEF(TrigNavTest::TestBContainer, 96422, 1)
CLASS_DEF(TrigNavTest::TestBContainerView, 96423, 1)
CLASS_DEF(TrigNavTest::TestAuxB, 642311, 1)
DATAVECTOR_BASE(TrigNavTest::TestAuxB,xAOD::AuxContainerBase);


#endif // not TRIGNAVIGATION_TESTTYPESB_H
