/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTCLASSWITHTRANSIENTS_H
#define TESTCLASSWITHTRANSIENTS_H

class TestClassTransient;

class TransientStruct {
public:
  TransientStruct() : data1( 0 ), data2( 0 ), data3(0) {}
  virtual ~TransientStruct() {}
  int data1;
  double data2;
  short data3;
};

class TransientObject {
public:
  TransientObject() : data1( 0 ), data2( 0 ) {}
  virtual ~TransientObject() {}
  int data1;
  float data2;
};

class TestClassWithTransients {
public:
  TestClassWithTransients();
  virtual ~TestClassWithTransients();

  int data1;
  TransientStruct transientData[4];
  TestClassTransient* transientPointer;
  TransientObject transientObject;
  int transientIntArray[4];
  TransientObject transientObjectArray[4];
  double data2;
};



inline
TestClassWithTransients::TestClassWithTransients():
  data1( 0 ),
  transientData(),
  transientPointer( 0 ),
  transientObject(),
  data2( 0 )
{
  for ( int i = 0; i < 4; ++i ) {
    transientData[i] = TransientStruct();
    transientIntArray[i] = 0;
  }
}

inline
TestClassWithTransients::~TestClassWithTransients()
{}

#endif
