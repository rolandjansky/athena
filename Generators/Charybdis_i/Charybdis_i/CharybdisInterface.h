/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
@class CharybdisInterface

@author Nick Brett [ n.brett1@physics.ox.ac.uk ] - 03-02-05

 a set of c++ functions that act as an interface between
 Athenas C++ herwig command sting handeler and charybdis
 (a FORTRAN program for simulation of Micro Black Holes)
*/
#ifndef CHARYBDISINTERFACE_H
#define CHARYBDISINTERFACE_H

extern void WriteCharybdisParam(int,int,double);
extern "C" void readcharybdisparamint_(int*,int*);
extern "C" void readcharybdisparamdbl_(int*,double*);

#endif


