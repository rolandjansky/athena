/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATRIG_FOOT_H
#define ATRIG_FOOT_H


#ifndef WINDOWS_H
#include "windows.h"
#endif


static const float Foot_win[THRES][NBINS][BOUND]={

{                                             

    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -1.078
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -1.034
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.946
    {  -14.32 ,  26.11  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.902
    {  -13.36 ,  22.81  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.858
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.814
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.726
    {  -11.75 ,  21.88  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.682
    {  -11.95 ,  21.54  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.638
    {  -11.29 ,  18.72  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.594
    {  -12.95 ,  19.49  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.550
    {  -13.03 ,  15.81  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.506
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.462
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.374
    {   -9.89 ,  14.45  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.330
    {  -10.08 ,  12.40  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.286
    {  -10.68 ,  11.08  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.242
    {  -10.69 ,  11.20  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.198
    {  -10.10 ,  13.71  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.154
    {   -9.59 ,  12.59  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.110
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.066
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta = -0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.066
    {  -12.47 ,   9.81  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.110
    {  -13.85 ,  10.16  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.154
    {  -11.23 ,  11.08  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.198
    {  -11.15 ,  10.62  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.242
    {  -12.45 ,  10.16  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.286
    {  -14.42 ,   9.76  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.330
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.374
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.462
    {  -14.29 ,  13.64  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.506
    {  -19.05 ,  13.40  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.550
    {  -18.98 ,  12.06  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.594
    {  -21.18 ,  12.13  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.638
    {  -21.79 ,  11.85  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.682
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.726
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.814
    {  -20.66 ,  13.36  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.858
    {  -26.28 ,  14.37  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.902
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.946
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres =  6.0 GeV,  eta =  1.034
    {    0.00 ,   0.00  }   // Feet  ,  thres =  6.0 GeV,  eta =  1.078

},
//===========================================================================//
{

    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -1.078
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -1.034
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.946
    {  -10.23 ,  12.84  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.902
    {   -9.34 ,  12.05  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.858
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.814
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.726
    {   -8.58 ,   9.98  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.682
    {   -8.08 ,  11.24  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.638
    {   -7.83 ,   9.93  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.594
    {   -8.30 ,  10.28  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.550
    {   -8.16 ,   9.84  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.506
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.462
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.374
    {   -6.77 ,   7.18  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.330
    {   -6.65 ,   7.72  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.286
    {   -6.73 ,   6.64  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.242
    {   -6.21 ,   6.27  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.198
    {   -6.30 ,   7.44  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.154
    {   -6.29 ,   7.45  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.110
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.066
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta = -0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.066
    {   -7.43 ,   6.33  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.110
    {   -7.49 ,   6.30  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.154
    {   -6.32 ,   6.23  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.198
    {   -6.70 ,   6.80  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.242
    {   -7.68 ,   6.68  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.286
    {   -7.19 ,   6.78  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.330
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.374
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.462
    {   -9.55 ,   8.32  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.506
    {  -10.20 ,   8.28  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.550
    {   -9.88 ,   7.90  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.594
    {  -11.15 ,   8.09  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.638
    {  -10.99 ,   8.60  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.682
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.726
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.814
    {  -11.88 ,   9.33  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.858
    {  -12.91 ,  10.30  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.902
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.946
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres =  8.0 GeV,  eta =  1.034
    {    0.00 ,   0.00  }   // Feet  ,  thres =  8.0 GeV,  eta =  1.078

},
//===========================================================================//
{                                             

    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -1.078
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -1.034
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.946
    {   -7.75 ,   9.42  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.902
    {   -6.94 ,   8.79  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.858
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.814
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.726
    {   -5.55 ,   7.04  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.682
    {   -6.21 ,   7.74  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.638
    {   -5.50 ,   7.02  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.594
    {   -6.35 ,   7.15  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.550
    {   -5.72 ,   6.75  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.506
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.462
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.374
    {   -4.53 ,   4.72  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.330
    {   -4.97 ,   5.70  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.286
    {   -5.06 ,   5.11  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.242
    {   -4.88 ,   5.13  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.198
    {   -4.86 ,   5.18  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.154
    {   -4.92 ,   5.10  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.110
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.066
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta = -0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.066
    {   -5.16 ,   5.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.110
    {   -5.09 ,   4.93  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.154
    {   -5.17 ,   4.86  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.198
    {   -5.11 ,   5.06  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.242
    {   -5.61 ,   4.97  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.286
    {   -4.73 ,   4.63  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.330
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.374
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.462
    {   -6.66 ,   5.76  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.506
    {   -7.24 ,   6.30  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.550
    {   -6.98 ,   5.91  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.594
    {   -7.66 ,   6.30  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.638
    {   -7.06 ,   5.55  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.682
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.726
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.814
    {   -8.62 ,   6.89  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.858
    {   -9.33 ,   7.81  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.902
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.946
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres = 10.0 GeV,  eta =  1.034
    {    0.00 ,   0.00  }   // Feet  ,  thres = 10.0 GeV,  eta =  1.078

},
//===========================================================================//
{

    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -1.078
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -1.034
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.946
    {  -21.26 ,  18.20  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.902
    {  -21.01 ,  16.64  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.858
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.814
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.726
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.682
    {  -15.84 ,  13.64  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.638
    {  -14.68 ,  13.12  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.594
    {  -13.76 ,  12.82  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.550
    {  -13.33 ,  12.26  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.506
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.462
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.374
    {  -11.20 ,  10.65  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.330
    {  -11.51 ,  11.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.286
    {  -11.00 ,   9.56  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.242
    {  -10.74 ,   9.86  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.198
    {  -10.19 ,  10.90  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.154
    {  -12.01 ,  11.27  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.110
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.066
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta = -0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.066
    {  -11.33 ,  12.02  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.110
    {  -10.91 ,  10.15  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.154
    {   -9.87 ,  10.70  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.198
    {   -9.56 ,  11.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.242
    {   -9.79 ,  11.70  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.286
    {  -10.80 ,  11.39  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.330
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.374
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.462
    {  -12.24 ,  13.21  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.506
    {  -12.66 ,  13.79  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.550
    {  -13.08 ,  14.77  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.594
    {  -13.60 ,  16.21  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.638
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.682
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.726
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.814
    {  -16.50 ,  21.04  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.858
    {  -18.13 ,  21.19  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.902
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.946
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres = 20.0 GeV,  eta =  1.034
    {    0.00 ,   0.00  }   // Feet  ,  thres = 20.0 GeV,  eta =  1.078

},
//===========================================================================//
{

    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -1.078
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -1.034
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.946
    {   -9.85 ,   9.18  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.902
    {   -9.95 ,   8.78  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.858
    {   -7.40 ,   8.18  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.814
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.726
    {   -8.13 ,   7.01  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.682
    {   -7.58 ,   7.31  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.638
    {   -7.21 ,   6.96  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.594
    {   -6.37 ,   6.64  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.550
    {   -6.74 ,   6.26  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.506
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.462
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.374
    {   -6.11 ,   5.82  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.330
    {   -5.88 ,   5.89  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.286
    {   -4.40 ,   5.98  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.242
    {   -4.35 ,   6.18  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.198
    {   -5.87 ,   4.67  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.154
    {   -5.55 ,   5.02  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.110
    {   -5.09 ,   5.09  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.066
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta = -0.022
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.022
    {   -5.09 ,   5.08  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.066
    {   -5.03 ,   5.54  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.110
    {   -4.64 ,   5.86  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.154
    {   -6.18 ,   4.35  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.198
    {   -6.01 ,   4.39  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.242
    {   -5.78 ,   5.82  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.286
    {   -5.80 ,   6.12  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.330
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.374
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.418
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.462
    {   -6.25 ,   6.67  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.506
    {   -6.63 ,   6.37  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.550
    {   -6.99 ,   7.22  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.594
    {   -7.37 ,   7.47  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.638
    {   -6.91 ,   8.14  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.682
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.726
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.770
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.814
    {   -8.63 ,   9.87  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.858
    {   -9.17 ,   9.84  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.902
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.946
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  0.990
    {    0.00 ,   0.00  },  // Feet  ,  thres = 40.0 GeV,  eta =  1.034
    {    0.00 ,   0.00  }   // Feet  ,  thres = 40.0 GeV,  eta =  1.078

}

};


#endif









