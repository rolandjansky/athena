/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
using namespace std;

#include "IBoardBase.hh"

class test : public IOTimeBase, public IBoardBase {
//class test : public IOTimeBase {
  //class test {


public:
  int iii;
  virtual void aaa();

};
