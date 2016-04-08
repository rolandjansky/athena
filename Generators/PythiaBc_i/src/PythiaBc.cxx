/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PythiaBc.cxx
//
// Konstantin Toms <ktoms@mail.cern.ch>
//

#ifndef PYTHIABC_I
#define PYTHIABC_I

#include <cstdlib>
#include <cstdio>

#include "PythiaBc_i/PythiaBc.h"

int WritePythiaBcParamInit=0;

struct PythiaBcParams
{
  int i_par1;
  int i_par2;
  double ECM;
  double d_par1;
  double PTCUT;
  double d_par2;
  double d_par3;
};

struct PythiaBcParams p;

// This function should be called from C++ :
extern void WritePythiaBcParam(int index, int iparameter, double dparameter)
{

  // Write default values into PythiaBcParams when function is run for the first time:
  if(WritePythiaBcParamInit < 1) {

    WritePythiaBcParamInit = 10;
    p.i_par1 = 0;       //1
    p.i_par2 = 0;       //2
    p.ECM    = 7000;    //3
    p.d_par1 = 0;       //4
    p.PTCUT  = 10;      //5
    p.d_par2 = 1.5;     //6
    p.d_par3 = 4.8;     //7
  } 

  switch ( index ) {

  case 1 : 
    p.i_par1 = iparameter;
    break;
  case 2 : 
    p.i_par2 = iparameter;
    break;
  case 3 :
    p.ECM = dparameter;
	break;
  case 4 :
    p.d_par1 = dparameter;
	break;
  case 5 :
    p.PTCUT = dparameter;
	break;
  case 6 :
    p.d_par2 = dparameter;
	break;
  case 7 :
    p.d_par3 = dparameter;
	break;
  default : 
	break;
  }
}


// This function should be called from FORTRAN code and is used to read parameters
// stored by the WritePythiaBcParam function:
extern "C" void readpythiabcparamint_(int* index, int* parameter)
{

   // Write default values into PythiaBcParams when function is run for the first time:
  if(WritePythiaBcParamInit < 1)
  {
    WritePythiaBcParamInit = 10;
    p.i_par1 = 0;    //1
    p.i_par2 = 0;    //2
    p.ECM = 7000;    //3
    p.d_par1 = 0;    //4
    p.PTCUT = 10;    //5
    p.d_par2 = 1.5;  //6
    p.d_par3 = 4.8;  //7
  } 


  if(*index == 1)
  {
    *parameter = p.i_par1;
  }  
  else if(*index == 2)
  {  
    *parameter = p.i_par2;
  }  
    //else
    //log << MSG:: INFO << " PythiaBc Interface -> Incorrect type cast for PythiaBc variable OR Incorrect index\n"  << endreq;
}


extern "C" void readpythiabcparamdbl_(int* index, double* parameter)
{
  
   // Write default values into PythiaBcParams when function is run for the first time:
  if(WritePythiaBcParamInit < 1)
  {
    WritePythiaBcParamInit = 10;
    p.i_par1 = 0;    //1
    p.i_par2 = 0;    //2
    p.ECM = 7000;    //3
    p.d_par1 = 0;    //4
    p.PTCUT = 10;    //5
    p.d_par2 = 1.5;  //6
    p.d_par3 = 4.8;  //7
  } 

  if(*index == 3)
  {
    *parameter = (double)(p.ECM);
  }
  else if(*index == 4)
  {
    *parameter = (double)(p.d_par1);
  }
  else if(*index == 5)
  {
    *parameter = (double)(p.PTCUT);
  }
  else if(*index == 6)
  {
    *parameter = (double)(p.d_par2);
  }
  else if(*index == 7)
  {
    *parameter = (double)(p.d_par3);
  }

}

#endif
