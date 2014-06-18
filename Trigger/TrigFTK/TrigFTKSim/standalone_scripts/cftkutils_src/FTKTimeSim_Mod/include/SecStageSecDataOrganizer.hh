/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include "IOTimeBase.hh"
#include "IBoardBase.hh"

// #define SecStageSecDO_T_IN 0
// #define SecStageSecDO_T_OUT 0
// //#define SecStageSecDO_T_PROC 25   // 40 MHz
// #define SecStageSecDO_T_PROC 10   // 40 MHz
// #define SecStageSecDO_T_DELAY 88   // 3-4 clock by 40MHz 3.5->87.5 ns

class SecStageSecDataOrganizer : public IOTimeBase,  public IBoardBase {

public:
  SecStageSecDataOrganizer() {}

  SecStageSecDataOrganizer(int bd_index)
    : IOTimeBase(bd_index, SecStageSecDO_T_IN, SecStageSecDO_T_OUT, SecStageSecDO_T_PROC, SecStageSecDO_T_DELAY) 
  {
  }

  SecStageSecDataOrganizer(int bd_index, double bd_t_in, double bd_t_out, double bd_t_proc, double bd_t_delay)
    : IOTimeBase(bd_index, bd_t_in, bd_t_out, bd_t_proc, bd_t_delay)
  {
  }
  void Calc();
 
  ClassDef(SecStageSecDataOrganizer, 2)
};
