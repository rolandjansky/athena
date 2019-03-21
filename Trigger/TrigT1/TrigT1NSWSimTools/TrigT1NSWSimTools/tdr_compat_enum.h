// -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NSW_TDR_COMPAT_ENUM_H
#define NSW_TDR_COMPAT_ENUM_H
#include <vector>
namespace NSWL1 {

    const double c_inverse = 3.33564095198e-3;  // unit here is  [ns/mm]
    const float PI=3.14159265;
    const int STGC_LAYER_1=1, STGC_LAYER_2=2, STGC_LAYER_3=3, STGC_LAYER_4=4;
    const int SMALL_PIVOT=0, SMALL_CONFIRM=1, LARGE_PIVOT=2, LARGE_CONFIRM=3;
    const int MAXLAYERSHIT=4;
    enum SECTYPE{SMALL,LARGE};
    
    const std::vector<size_t>  SECTORS={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    const std::vector<size_t> SIDES={0,1};
    
}

#endif

