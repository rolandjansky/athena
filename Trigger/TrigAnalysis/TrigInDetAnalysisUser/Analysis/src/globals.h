/* emacs: this is -*- c++ -*- */
/**
 **     @file    globals.h
 **
 **     @author  mark sutton
 **     @date    Sun 12 Mar 2017 15:21:08 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  GLOBALS_H
#define  GLOBALS_H

/// all these externals initialised in globals.cxx

extern TIDARoiDescriptor* groi; 
extern TIDA::Event*       gevent;

extern int Nvtxtracks; 
extern int NvtxCount; 

extern int NMod; 

extern bool hipt; 
extern bool dumpflag; 

extern int r; 
extern int lb;
extern int ts;
extern int ev;

extern double a0;

#endif  // GLOBALS_H 










