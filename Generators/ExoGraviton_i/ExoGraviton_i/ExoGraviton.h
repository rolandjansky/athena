/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ExoGraviton.h
//
// Reyhaneh Rezvani <reyhaneh.rezvani@cern.ch> 13-03-2009
//
//a set of c++ functions that act as an interface between
//Athenas C++ Pythia and ExoGraviton
//(a FORTRAN program for simulation of "Real Graviton Emission")

#ifndef EXOGRAVITON_H
#define EXOGRAVITON_H

extern void WriteGravParam(int,int,double);
extern "C" void readgravparamint_(int*,int*);
extern "C" void readgravparamdbl_(int*,double*);
#endif
