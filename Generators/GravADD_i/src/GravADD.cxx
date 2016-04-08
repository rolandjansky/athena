/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// a set of c++ functions that act as an interface between 
// Athena C++ PythiaExo and GravADD,

#ifndef GRAVADD_I
#define GRAVADD_I

#include <cstdlib>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include "GravADD_i/GravADD.h"


int WriteGravParamInit=0;

struct GravADDParams
{
  int TOTDIM;    // 4 + n dimensions
  double CMEN;   // center of mass energy
  double MSPRAT;  // MS / MP
  double MEFF;
  int PLDEF;
  int GSONLY;
 
};

struct GravADDParams p;


// This function should be called from c++ :
extern void writegravaddparam( int index, int iparameter, double dparameter)
{


  // Write default values into GravParams when function is run for the first time:
   if(WriteGravParamInit < 1)
  {
    WriteGravParamInit = 5;
    p.TOTDIM = 10;        //1, total number of dimensions	
    p.CMEN =7000.0 ;      //2, Center of Mass Energy
    p.MSPRAT = 1.0;       //3, Ms/Mp
    p.MEFF = 1000.0;      //4, Effective mass
    p.PLDEF = 1;           //5, 1: Planck mass defined in hep-ph/0608210, 2: Planck mass defined in PDG 
    p.GSONLY = 0;          //5, 1: only GS (no BH), 0: BH+GS
  } 

  if (index == 1)
 {
    p.TOTDIM = iparameter;
 }
  else if (index == 2 )
 {
    p.CMEN = dparameter;
 }  
  else if (index == 3)
 {
    p.MSPRAT = dparameter;
 }
  else if (index == 4)
 {
    p.MEFF = dparameter;
 }
  else if (index == 5)
 {
    p.PLDEF = iparameter;
 }  
   else if (index == 6)
 {
    p.GSONLY = iparameter;
 }  
  

}


// This function should be called from FORTRAN code and is used to read parameters
// stored by the WriteGravADDParam function:
extern "C" void readgravaddparamint_(int* index, int* parameter)
{

   // Write default values into GravParams when function is run for the first time:
   if(WriteGravParamInit < 1)
  {
    WriteGravParamInit = 6;
    p.TOTDIM = 10;         //1
    p.CMEN =7000.0 ;      //2
    p.MSPRAT = 1.0;        //3
    p.MEFF = 1000.0;       //4
    p.PLDEF = 1;            // 5
    p.GSONLY = 0;
  } 

  if(*index == 1)
  {
    *parameter = p.TOTDIM;
  }  
  if(*index == 5)
  {
    *parameter = p.PLDEF;
  }  	
   
  if(*index == 6)
    {
      *parameter = p.GSONLY;
    }  	
    
}

extern "C" void readgravaddparamdbl_(int* index, double* parameter)
{
 
  // Write default values into GravParams when function is run for the first time:
   if(WriteGravParamInit < 1)
  {
    WriteGravParamInit = 6;  
    p.TOTDIM = 10;         //1
    p.CMEN =7000.0 ;      //2
    p.MSPRAT = 1.0;        //3
    p.MEFF = 1000.0;       //4
    p.PLDEF = 1;            // 5 
    p.GSONLY = 0;            // 6
  } 

  if(*index == 2)
  {
    *parameter = (double)(p.CMEN);
  }
  else if(*index == 3)
  {
    *parameter = (double)(p.MSPRAT);
  }
  else if(*index == 4)
  {
    *parameter = (double)(p.MEFF);
  }
  
}

#endif
