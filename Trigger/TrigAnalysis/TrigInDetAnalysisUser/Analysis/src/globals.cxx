//
//   @file    globals.cxx         
//   
//
//   @author M Sutton
//
//   $Id: globals.cxx, v0.0   Sun 12 Mar 2017 15:21:12 CET sutt $
//
//   Copyright (C) 2017 M Sutton (sutt@cern.ch)    

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/TIDAEvent.h"

TIDA::Event*       gevent = 0;
TIDARoiDescriptor*   groi = 0;


int  Nvtxtracks = 0;
int  NvtxCount  = 0;

int  NMod = 14;

int   r = 0;
int  lb = 0;
int  ts = 0;
int  ev = 0;

double a0 = 1.5;

bool hipt     = false;
bool dumpflag = false;


