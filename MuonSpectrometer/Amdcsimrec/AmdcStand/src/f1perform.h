/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef f1perform_H
#define f1perform_H
extern "C" void f1performdirectdisplacanddeform_(     char* STANAME, int& Jff, int& Jzz,           double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout );
extern "C" void f1performdirectdisplacanddeformbase_( char* STANAME, int& Jff, int& Jzz, int& Job, double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout );
extern "C" void f1performbackdisplacanddeform_(       char* STANAME, int& Jff, int& Jzz,           double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout );
extern "C" void f1performbackdisplacanddeformbase_(   char* STANAME, int& Jff, int& Jzz, int& Job, double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout );
#endif
