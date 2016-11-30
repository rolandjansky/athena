/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>

#include "IOTimeBase.hh"
#include "IBoardBase.hh"
//#include "Region.hh"

// #define DO_T_IN 0
// #define DO_T_OUT 1
// #define DO_T_PROC 25   // 40 MHz (ns)
// #define DO_T_DELAY 88   // 3-4 clock by 40MHz 3.5->87.5 ns


//#define NASSOCIATIVEMEMORY 12
//#define NDATAORGANIZER 6


class DataOrganizer : public IOTimeBase,  public IBoardBase {

public:
  DataOrganizer() {}
  //  int layer_layout[] = {0,1, 3,10, 4,9, 5,8, 6,7, 2}; // 0-2:pixcel 3-:SCT to 6 DO  // cannot...

  DataOrganizer(int bd_index)
    : IOTimeBase(bd_index, DO_T_IN, DO_T_OUT, DO_T_PROC, DO_T_DELAY) 
  {
//     for(int idx=0;idx<NASSOCIATIVEMEMORY;idx++) {
//       AssociativeMemory sAM(idx);
//       cAM.push_back(sAM);
//     }
//     make_heap(cAM.begin(), cAM.end(), CompEwOut());
  }

  DataOrganizer(int bd_index, double bd_t_in, double bd_t_out, double bd_t_proc, double bd_t_delay)
    : IOTimeBase(bd_index, bd_t_in, bd_t_out, bd_t_proc, bd_t_delay)
  {
  }



//   int GetNAssociativeMemory() {return NASSOCIATIVEMEMORY;}
//   AssociativeMemory GetAssociativeMemory(int index) {return this->cAM[index];}

  void Calc();

  //  ClassDef(DataOrganizer,2)
};
