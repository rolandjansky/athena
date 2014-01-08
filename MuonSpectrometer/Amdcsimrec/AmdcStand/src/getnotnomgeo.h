/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef getnotnomgeo_H
#define getnotnomgeo_H
extern "C" void getdisplacementfromamdcjobn_(                   char* STANAME, int& Jff, int& Jzz,                       double& TranslatS, double& TranslatZ, double& TranslatT, double& RotatS, double& RotatZ, double& RotatT );
extern "C" void getdisplacementfromamdc_(                       char* STANAME, int& Jff, int& Jzz, int& Jobj,            double& TranslatS, double& TranslatZ, double& TranslatT, double& RotatS, double& RotatZ, double& RotatT );
extern "C" void getdisplacementfromamdcjadjust_(     int& Jadjust,  int& Jtyp, int& Jff, int& Jzz, int& Jobj,            double& TranslatS, double& TranslatZ, double& TranslatT, double& RotatS, double& RotatZ, double& RotatT );
extern "C" void getdeformationfromamdcjobn_(                    char* STANAME, int& Jff, int& Jzz,                       double& bz, double& bp, double& bn, double& sp, double& sn, double& tw, double& pg, double& tr, double& eg, double& ep, double& en );
extern "C" void getdeformationfromamdc_(                        char* STANAME, int& Jff, int& Jzz, int& Jobj,            double& bz, double& bp, double& bn, double& sp, double& sn, double& tw, double& pg, double& tr, double& eg, double& ep, double& en );
extern "C" void getdeformationfromamdcjdeform_(      int& Jdeform,  int& Jtyp, int& Jff, int& Jzz, int& Jobj,            double& bz, double& bp, double& bn, double& sp, double& sn, double& tw, double& pg, double& tr, double& eg, double& ep, double& en );
extern "C" void getinternalalignmentfromamdcjdwnlay_(int& Jdwnlay,  int& Jtyp, int& Jff, int& Jzz, int& Jobj, int& Jlay, double& TranslatS, double& TranslatZ, double& TranslatT, double& RotatS, double& RotatZ, double& RotatT );
#endif
