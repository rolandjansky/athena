/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//a set of c++ functions that act as an interface between
//Athenas C++ Pythia and GravADD
//(a FORTRAN program for simulation of gravitational scattering and black hole formation in large extra dimensions)

#ifndef GRAVADD_H
#define GRAVADD_H
extern void writegravaddparam(int,int,double);
extern "C" void readGravADDparamint_(int*,int*);
extern "C" void readGravADDparamdbl_(int*,double*);

#endif


