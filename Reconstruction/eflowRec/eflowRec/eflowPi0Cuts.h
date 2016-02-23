/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWPI0CUTS_H
#define EFLOWPI0CUTS_H

class eflowPi0Cuts {

 public:
  eflowPi0Cuts() {};
  ~eflowPi0Cuts() {};
  void isPi0(bool& result, float eta, float et, float pi0Score, int numProng);


};
#endif
