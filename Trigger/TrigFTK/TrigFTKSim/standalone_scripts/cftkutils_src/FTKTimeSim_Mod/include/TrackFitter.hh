/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include "IOTimeBase.hh"
#include "IBoardBase.hh"

// #define TF_T_IN 1
// #define TF_T_OUT 0
// #define TF_T_PROC 1
// #define TF_T_DELAY 10

class TrackFitter : public IOTimeBase,  public IBoardBase {


public:
  TrackFitter() {}

  TrackFitter(int bd_index)
    : IOTimeBase(bd_index, TF_T_IN, TF_T_OUT, TF_T_PROC, TF_T_DELAY) 
  {

  }

  TrackFitter(int bd_index, double bd_t_in, double bd_t_out, double bd_t_proc, double bd_t_delay)
    : IOTimeBase(bd_index, bd_t_in, bd_t_out, bd_t_proc, bd_t_delay)
  {
  }
  
  TrackFitter(IOTimeBase& iotime){}

  void Calc();

  ClassDef(TrackFitter,2)
};
