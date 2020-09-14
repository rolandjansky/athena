/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MYTESTCLASSES_H
#define MYTESTCLASSES_H

class MyTestClass {
public:
  MyTestClass():data( 0 ) {}
  virtual ~MyTestClass() {}
  int data;
};

#endif
