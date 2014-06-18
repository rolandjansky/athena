/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>

#include "IOTimeBase.hh"
#include "IBoardBase.hh"

#define DF_T_IN 0
#define DF_T_OUT 0
#define DF_T_PROC 0
#define DF_T_DELAY 0

class DataFormatter : public IOTimeBase,  public IBoardBase {


public:
  DataFormatter() {}

  DataFormatter(int bd_index)
    : IOTimeBase(bd_index, DF_T_IN, DF_T_OUT, DF_T_PROC, DF_T_DELAY) 
  {
  }

  DataFormatter(int bd_index, double bd_t_in, double bd_t_out, double bd_t_proc, double bd_t_delay)
    : IOTimeBase(bd_index, bd_t_in, bd_t_out, bd_t_proc, bd_t_delay)
  {
  }



  void Calc();

  ClassDef(DataFormatter, 2)
};
