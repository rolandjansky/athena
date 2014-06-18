/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include "IOTimeBase.hh"
#include "IBoardBase.hh"

// #define SecStageTF_T_IN 1
// #define SecStageTF_T_OUT 0
// #define SecStageTF_T_PROC 1
// #define SecStageTF_T_DELAY 10

class SecStageTrackFitter : public IOTimeBase,  public IBoardBase {


public:
  SecStageTrackFitter() {}

  SecStageTrackFitter(int bd_index)
    : IOTimeBase(bd_index, SecStageTF_T_IN, SecStageTF_T_OUT, SecStageTF_T_PROC, SecStageTF_T_DELAY) 
  {

  }

  SecStageTrackFitter(int bd_index, double bd_t_in, double bd_t_out, double bd_t_proc, double bd_t_delay)
    : IOTimeBase(bd_index, bd_t_in, bd_t_out, bd_t_proc, bd_t_delay)
  {
  }
  
  SecStageTrackFitter(IOTimeBase& iotime){}

  void Calc();

  ClassDef(SecStageTrackFitter,2)
};
