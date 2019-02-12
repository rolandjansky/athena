/**
 **     @file    globals.cxx
 **
 **     @author  mark sutton
 **     @date    Sun 12 Mar 2017 15:21:12 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

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


