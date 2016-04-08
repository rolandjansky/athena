/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ExoGraviton.cxx
//
// Reyhaneh Rezvani <reyhaneh.rezvani@cern.ch> 13-03-2009
//
// a set of c++ functions that act as an interface between 
// Athena C++ Pythia and ExoGraviton,
// (a FORTRAN program for simulation of "Real Graviton Emission")

#ifndef EXOGRAVITON_I
#define EXOGRAVITON_I

#include <cstdlib>
#include <cstdio>

#include "ExoGraviton_i/ExoGraviton.h"


int WriteGravParamInit=0;

struct ExoGravitonParams
{
  int ndim;
  int iproc;
  int icutoff;
  double ECM;
  double AMD;
  double PTCUT;
  double GravMaxMass;
  double SIGMAX;
};

struct ExoGravitonParams p;


// This function should be called from c++ :
extern void WriteGravParam( int index, int iparameter, double dparameter)
{


  // Write default values into ExoGravitonParams when function is run for the first time:
   if(WriteGravParamInit < 1)
  {
    WriteGravParamInit = 10;
    p.ndim = 2;           //1
    p.iproc = 1111;       //2
    p.ECM = 7000;         //3
    p.AMD = 3500;         //4
    p.PTCUT = 350;        //5
    p.GravMaxMass = 3500; //6
    p.SIGMAX = 2.5E+1;    //7
    p.icutoff = 0;         //8
  } 

  if (index == 1)
 {
    p.ndim = iparameter;
 }
  else if (index == 2 )
 {
    p.iproc = iparameter;
 }  
  else if (index == 3)
 {
    p.ECM = dparameter;
 }
  else if (index == 4)
 {
    p.AMD = dparameter;
 }
 else if (index == 5)
 {
    p.PTCUT = dparameter;
 }
 else if (index == 6)
 {
    p.GravMaxMass = dparameter;
 }
 else if (index == 7)
 {
    p.SIGMAX = dparameter;
 }
 else if (index == 8)
 {
    p.icutoff = iparameter;
 }
}


// This function should be called from FORTRAN code and is used to read parameters
// stored by the WriteGravParam function:
extern "C" void readgravparamint_(int* index, int* parameter)
{

   // Write default values into ExoGravitonParams when function is run for the first time:
   if(WriteGravParamInit < 1)
  {
    WriteGravParamInit = 10;
    p.ndim = 2;           //1
    p.iproc = 1111;       //2
    p.ECM = 7000;        //3
    p.AMD = 3500;         //4
    p.PTCUT = 350;        //5
    p.GravMaxMass = 3500; //6
    p.SIGMAX = 2.5E+1;    //7
    p.icutoff = 0;    //8
  } 


  if(*index == 1)
  {
    *parameter = p.ndim;
  }  
  else if(*index == 2)
  {  
    *parameter = p.iproc;
  } 
  else if(*index == 8)
  {  
    *parameter = p.icutoff;
  }  
    //else
    //log << MSG:: INFO << " ExoGraviton Interface -> Incorrect type cast for ExoGraviton variable OR Incorrect index\n"  << endreq;
    
}



extern "C" void readgravparamdbl_(int* index, double* parameter)
{


  
  // Write default values into ExoGravitonParams when function is run for the first time:
   if(WriteGravParamInit < 1)
  {
    WriteGravParamInit = 10;
    p.ndim = 2;           //1
    p.iproc = 1111;       //2
    p.ECM = 7000;         //3
    p.AMD = 3500;         //4
    p.PTCUT = 350;        //5
    p.GravMaxMass = 3500; //6
    p.SIGMAX = 2.5E+1;    //7
    p.icutoff = 0;         //8
  } 

  if(*index == 3)
  {
    *parameter = (double)(p.ECM);
  }
  else if(*index == 4)
  {
    *parameter = (double)(p.AMD);
  }
  else if(*index == 5)
  {
    *parameter = (double)(p.PTCUT);
  }
  else if(*index == 6)
  {
    *parameter = (double)(p.GravMaxMass);
  }
  else if(*index == 7)
  {
    *parameter = (double)(p.SIGMAX);
  }

}

#endif
