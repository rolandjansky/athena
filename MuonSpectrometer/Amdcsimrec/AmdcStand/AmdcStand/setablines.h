/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef setablines_H
#define setablines_H                                                                 
extern "C" void stasetalines_(char* STANAME, int& JFF, int& JZZ, int& JOB,            double* A, int& ier);
extern "C" void stasetblines_(char* STANAME, int& JFF, int& JZZ, int& JOB,            double* B, int& ier);
extern "C" void stasetilines_(char* STANAME, int& JFF, int& JZZ, int& JOB, int& JLAY, double* B, int& ier);
extern "C" void seticsc_();
#endif
