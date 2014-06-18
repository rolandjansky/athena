/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include "IOTimeBase.hh"
#include "IBoardBase.hh"

// #define SecDO_T_IN 0
// #define SecDO_T_OUT 0
// //#define SecDO_T_PROC 25   // 40 MHz
// #define SecDO_T_PROC 10   // 40 MHz
// #define SecDO_T_PROC 0   // 40 MHz
// #define SecDO_T_DELAY 88   // 3-4 clock by 40MHz 3.5->87.5 ns

class SecDataOrganizer : public IOTimeBase,  public IBoardBase {

public:
  SecDataOrganizer() {}

  SecDataOrganizer(int bd_index)
    : IOTimeBase(bd_index, SecDO_T_IN, SecDO_T_OUT, SecDO_T_PROC, SecDO_T_DELAY) 
  {
  }

  SecDataOrganizer(int bd_index, double bd_t_in, double bd_t_out, double bd_t_proc, double bd_t_delay)
    : IOTimeBase(bd_index, bd_t_in, bd_t_out, bd_t_proc, bd_t_delay)
  {
  }
  void Calc();
 
  ClassDef(SecDataOrganizer, 2)
};
