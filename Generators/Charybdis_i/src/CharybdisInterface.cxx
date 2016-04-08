/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CharybdisInterface.cxx
//
// Nick Brett [ n.brett1@physics.ox.ac.uk ] - 03-02-05
//
// a set of c++ functions that act as an interface between 
// Athenas C++ herwig command sting handeler and charybdis 
// (a FORTRAN program for simulation of Micro Black Holes)

#ifndef CHARYBDIS_INTERFACE
#define CHARYBDIS_INTERFACE

#include <cstdlib>
#include <cstdio>

#include "Charybdis_i/CharybdisInterface.h"

int WriteCharybdisParamInit=0;

struct CharybdisParams
{
  double Mplank;
  int TotDim;
  double MinMbh;
  double MaxMbh;
  int RemnantDecay;
  int TimeVariation;
  int GreyBodyFactor;
  int KinCut;
  int Yrcsec;
  double Thwmax;
  int Rmboil;
  double Rmminm;
  int MssDef;
  int Gtsca;
  int MssDec;
  int IbhPrn;
  double Ebmup;

};

struct CharybdisParams param;




// This function should be called from c++ and used to write parameters
// to a temporary store
extern void WriteCharybdisParam( int index, int iparameter, double dparameter )
{
  // Write default values into CharybdisParam when function is run for the first time
  if(WriteCharybdisParamInit < 1)
  {
    WriteCharybdisParamInit = 10;

    param.Mplank=1000;          // 1
    param.TotDim=8;             // 2
    param.MinMbh=5000;          // 3
    param.MaxMbh=14000;         // 4
    param.RemnantDecay=2;       // 5
    param.TimeVariation=1;      // 6
    param.GreyBodyFactor=1;     // 7
    param.KinCut=0;             // 8
    param.Yrcsec=0;             // 9
    param.Thwmax=1000;          //10
    param.Rmboil=0;             //11
    param.Rmminm=1000;          //12
    param.MssDef=2;             //13
    param.Gtsca=1;              //14  Boolean
    param.MssDec=3;             //15
    param.IbhPrn=1;             //16
    param.Ebmup=7000;           //17



  }

  if(index == 1)
  {
      param.Mplank = dparameter;
  }

  else if(index == 2)
  {
      param.TotDim = iparameter;
  }

  else if(index == 3)
  {
      param.MinMbh = dparameter;
  }

  else if(index == 4)
  {
      param.MaxMbh = dparameter;
  }

  else if(index == 5)
  {
      param.RemnantDecay = iparameter;
  }

  else if(index == 6)
  {
    param.TimeVariation = iparameter;
  }

  else if(index == 7)
  {
      param.GreyBodyFactor = iparameter;
  }
 
  else if(index == 8)
  {
      param.KinCut = iparameter;
  }
  else if(index == 9)
  {
      param.Yrcsec = iparameter;
  }
  else if(index == 10)
  {
      param.Thwmax = dparameter;
  }
  else if(index == 11)
  {
      param.Rmboil = iparameter;
  }
  else if(index == 12)
  {
      param.Rmminm = dparameter;
  }
  else if(index == 13)
  {
      param.MssDef = iparameter;
  }
  else if(index == 14)
  {
      param.Gtsca = iparameter;
  }
  else if(index == 15)
  {
      param.MssDec = iparameter;
  }
  else if(index == 16)
  {
      param.IbhPrn = iparameter;
  }
  else if(index == 17)
  {
      param.Ebmup = dparameter;
  }

}

// This function should be called from FORTRAN and used to read parameters
// stored by the write function
extern "C" void readcharybdisparamint_(int* index, int* parameter)
{
  // Write default values into CharybdisParam if WriteCharybdisParam has never been run
  if(WriteCharybdisParamInit < 1)
  {
    WriteCharybdisParamInit = 10;

    param.Mplank=1;            //1
    param.TotDim=2;            //2
    param.MinMbh=5000;         //3
    param.MaxMbh=14000;        //4
    param.RemnantDecay=2;      //5
    param.TimeVariation=1;     //6
    param.GreyBodyFactor=1;    //7
    param.KinCut=0;            //8
    param.Yrcsec=0;            //9
    param.Thwmax=1000;         //10
    param.Rmboil=0;            //11
    param.Rmminm=1000;         //12
    param.MssDef=2;            //13
    param.Gtsca=1;             //14
    param.MssDec=3;            //15
    param.IbhPrn=1;            //16
    param.Ebmup=7000;          //17
  }


  if(*index == 2)
  {
      *parameter = param.TotDim;
  }

  else if(*index == 5)
  {
     *parameter = param.RemnantDecay;
  }

  else if(*index == 6)
  {
    *parameter = param.TimeVariation;
  }

  else if(*index == 7)
  {
     *parameter = param.GreyBodyFactor;
  }

  else if(*index == 8)
  {
    *parameter =  param.KinCut;
  }

  else if(*index == 9)
  {
    *parameter =  param.Yrcsec;
  }

  else if(*index == 11)
  {
    *parameter =  param.Rmboil;
  }

  else if(*index == 13)
  {
    *parameter = param.MssDef;
  }
  else if(*index == 14)
  {
    *parameter = param.Gtsca;
  }
  else if(*index == 15)
  {
    *parameter = param.MssDec;
  }
  else if(*index == 16)
  {
    *parameter = param.IbhPrn;
  }



  else
  {
    //log << MSG:: INFO << " Charybdis Interface -> Incorrect type cast for charybdis variable OR Incorrect index\n"  << endreq;
  }
    
}

extern "C" void readcharybdisparamdbl_(int* index, double* parameter)
{
  // Write default values into CharybdisParam if WriteCharybdisParam has never been run
  if(WriteCharybdisParamInit < 1)
  {
    WriteCharybdisParamInit = 10;

    param.Mplank=1;            //1
    param.TotDim=2;            //2
    param.MinMbh=5000;         //3
    param.MaxMbh=14000;        //4
    param.RemnantDecay=2;      //5
    param.TimeVariation=1;     //6
    param.GreyBodyFactor=1;    //7
    param.KinCut=0;            //8
    param.Yrcsec=0;            //9
    param.Thwmax=1000;         //10
    param.Rmboil=0;            //11
    param.Rmminm=1000;         //12
    param.MssDef=2;            //13
    param.Gtsca=1;             //14
    param.MssDec=3;            //15
    param.IbhPrn=1;            //16
    param.Ebmup=7000;          //17
  }

  if(*index == 1)
  {
      *parameter = param.Mplank;
  }

  else if(*index == 3)
  {
     *parameter = param.MinMbh;
  }

  else if(*index == 4)
  {
     *parameter = param.MaxMbh;
  }

  else if(*index == 10)
  {
    *parameter =  param.Thwmax;
  }

  else if(*index == 12)
  {
    *parameter =  (double)(param.Rmminm);
  }
  else if(*index == 17)
  {
    *parameter =  (double)(param.Ebmup);
  }



  else
  {
    //log << MSG:: INFO << " Charybdis Interface -> Incorrect type cast for charybdis variable OR Incorrect index\n"  << endreq;
  }

    
}


#endif
