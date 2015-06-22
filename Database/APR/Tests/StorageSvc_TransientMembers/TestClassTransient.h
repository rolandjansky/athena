/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTCLASSTRANSIENT_H
#define TESTCLASSTRANSIENT_H

class TestClassTransient {
public:
  TestClassTransient() : data(0) {}
  virtual ~TestClassTransient() {};
  int data;
};

#endif
