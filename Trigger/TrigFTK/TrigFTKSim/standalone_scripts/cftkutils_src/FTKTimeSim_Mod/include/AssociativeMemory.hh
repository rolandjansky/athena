/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>

#include "IOTimeBase.hh"
#include "IBoardBase.hh"

// #define AM_T_IN 1  // 85 MHz * 12 port i.e. 1G Hz
// #define AM_T_OUT 0
// #define AM_T_PROC 25  // 40 MHz
// #define AM_T_DELAY 250 // 10 clock by 40MHz

class AssociativeMemory : public IOTimeBase,  public IBoardBase {


public:
  AssociativeMemory() {}

  AssociativeMemory(int bd_index)
    : IOTimeBase(bd_index, AM_T_IN, AM_T_OUT, AM_T_PROC, AM_T_DELAY) 
  {

  }

  AssociativeMemory(int bd_index, double bd_t_in, double bd_t_out, double bd_t_proc, double bd_t_delay)
    : IOTimeBase(bd_index, bd_t_in, bd_t_out, bd_t_proc, bd_t_delay)
  {
  }


  void Calc();

  ClassDef(AssociativeMemory,2)
};
