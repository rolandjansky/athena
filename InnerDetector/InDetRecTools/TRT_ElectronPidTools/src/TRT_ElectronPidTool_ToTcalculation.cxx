/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidTool_ToTcalculation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifdef INDETTRT_ELECTRONPIDTOOL_H
#ifndef TRT_ELECTRONPID_TOTCALCULATION_CXX
#define TRT_ELECTRONPID_TOTCALCULATION_CXX

#include <time.h>

/*****************************************************************************\

  This file contains the implementation of the class ToTcalculator.

  It is intended to contain all the code that is used for the calculation of
  the likelyhood of a TRT hit being from an electron based on the time over 
  threshold.

  Author: Simon Heisterkamp (simon.heisterkamp@cern.ch)

\*****************************************************************************/

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Default Constructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidTool::ToTcalculator::ToTcalculator(AthAlgTool & parent):
  BaseTRTPIDCalculator(parent,(SIZE_OF_HEADER + SIZE_GAUS_PARS * 2 + SIZE_TOT_CONSTANTS + SIZE_PAR_VAL),"ToT"),
  ToTCorrectionConstants( (float*)( Blob + OFF_TOT_CONSTANTS )),
  gausParametersElectron( (float*)( Blob + OFF_GAUS_PARS_ELE )),
  gausParametersPion( (float*)( Blob + OFF_GAUS_PARS_PIO )),
  parValBarrelLongNeg( *( 0 + (float*)( Blob + OFF_PAR_VALS))),
  parValBarrelLongPos( *( 1 + (float*)( Blob + OFF_PAR_VALS))),
  parValBarrelShortNeg( *( 2 + (float*)( Blob + OFF_PAR_VALS))),
  parValBarrelShortPos( *( 3 + (float*)( Blob + OFF_PAR_VALS)))
{
  CurrentVersion = my_CurrentVersion;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Default Destructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidTool::ToTcalculator::~ToTcalculator(){
  //Nothing here yet
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Calculate the value of ToT.  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Fully corrected for all factors  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

double InDet::TRT_ElectronPidTool::ToTcalculator::GetToT(unsigned int bitpattern, double HitZ, double HitR, int BEC, int Layer, int Strawlayer){
  checkInitialization();

  double ToTraw=ExtractToT(bitpattern);

  double ToTcorr=correctToT(ToTraw, HitZ, HitR, BEC, Layer, Strawlayer);

  //std::cout<<"TRT_ElectronPidTool    found raw ToT "<<ToTraw<<"  corrected to "<<ToTcorr<<std::endl;
  //  std::cout<<"TRT_ElectronPidTool    used HitZ "<<HitZ<<", HitR "<<HitR<<", BEC "<<BEC<<", Layer "<<Layer<<", Strawlayer "<<Strawlayer<<std::endl;
  return ToTcorr;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Calculate the value of L.  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

double InDet::TRT_ElectronPidTool::ToTcalculator::GetD(double R_Track){
  // This function was found to be the best function to use when normalizing the Time over Threshold

  R_Track=fabs(R_Track);

  if(R_Track>2.) return 0;

  return 2. * sqrt( 4. - R_Track * R_Track );
}


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Correct the ToT for effects determined by length along a straw  %%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/


double InDet::TRT_ElectronPidTool::ToTcalculator::correctToT(double ToT, double HitZ, double HitR, int HitPart, int Layer, int Strawlayer) {
  checkInitialization();
  //Function Pointer to the function that will correct the ToT
  double (InDet::TRT_ElectronPidTool::ToTcalculator::*correctionFunction)(double, double, const float*);

  if (abs(HitPart)==1){
    if(Layer==0 && Strawlayer<9)
      correctionFunction=&InDet::TRT_ElectronPidTool::ToTcalculator::BarrelShortToTCorrection;
    else
      correctionFunction=&InDet::TRT_ElectronPidTool::ToTcalculator::BarrelLongToTCorrection;
  }
  else
    correctionFunction=&InDet::TRT_ElectronPidTool::ToTcalculator::EndcapToTCorrection;


  double Val= parValBarrelLongNeg;  
  
  double correction=(*this.*correctionFunction)( HitZ, HitR, ToTCorrectionConstants + resolveIndex(HitPart,Layer,Strawlayer));

  //std::cout<<"TRT_ElectronPidTool      Part: "<<HitPart<<" Layer "<<Layer<<" SL "<<Strawlayer<<std::endl; 
  //std::cout<<"TRT_ElectronPidTool      calculated ToT correction: "<<correction<<std::endl;

  return ToT  - correction + Val;
}

double InDet::TRT_ElectronPidTool::ToTcalculator::BarrelLongToTCorrection( double HitZ, double /*HitR*/, const float *params){

  return params[PAR0] + (std::abs( HitZ ) - 7.5 + 704.6 ) / params[PAR1] * exp( ( std::abs( HitZ ) - 704.6 ) / params[PAR2]);
}

double InDet::TRT_ElectronPidTool::ToTcalculator::BarrelShortToTCorrection( double HitZ, double /*HitR*/, const float *params){

  return params[PAR0] + ( HitZ * HitZ - 7.5 + 704.6 ) / params[PAR1] * exp( (  HitZ * HitZ  - 704.6 ) / params[PAR2]);
}

double InDet::TRT_ElectronPidTool::ToTcalculator::EndcapToTCorrection( double /*HitZ*/, double HitR, const float *params){

  return params[PAR0] + params[PAR1] * exp( ( HitR * HitR - params[PAR2]) / params[PAR3] );
}


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Extract ToT from BitPattern,  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

double InDet::TRT_ElectronPidTool::ToTcalculator::ExtractToT(unsigned int bitpattern){
  /***************************************************************************
    Will find the time over threshold for this bitpattern using the largest island
    To find out how HT and LT bins are distributed please check TRT_DriftCircle.h
  ***************************************************************************/

  int ToT = 0;

  unsigned int mask = 0x02000000;
  
  bool LastBit=0;
  
  int start_i=99;
  int i;

  for(i=0;i<24;i++){

    bool ThisBit = mask & bitpattern;

    mask >>= 1;
    //skip HT bits
    if(not (mask & LTbits)) mask>>=1;

    if(ThisBit!=LastBit){
      //We're at an edge

      if(ThisBit)
        //LeadingEdge
        start_i = i;

      if(!ThisBit)
        //TrailingEdge
        if( (i - start_i) > ToT )
          ToT = i - start_i;

      LastBit=ThisBit;
    }
  }
  //If we finished on a high, also insert that island
  if(LastBit)
   if( (i - start_i) > ToT )
          ToT = i - start_i;
  
  return ToT*3.125;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Get The Pobability of this being an electron based on the known  %%%%%*|
|*%%%  distributions of SumToT/SumD for pions ond electrons.  %%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

double  InDet::TRT_ElectronPidTool::ToTcalculator::GetElProb(double ToTbyD){
  checkInitialization();
  // calculate the likelyhood for electrons at this values of ToT over D
  //The distributions are given given by the sum of three gaussians
  //Each gaussian has 3 parameters giving 9 parameters
  
  double eleP = (gausParametersElectron[0] * exp( -0.5 * ( ( ( ToTbyD - gausParametersElectron[1] ) / gausParametersElectron[2] ) *
                                                           ( ( ToTbyD - gausParametersElectron[1] ) / gausParametersElectron[2] ) ) ) +

                 gausParametersElectron[3] * exp( -0.5 * ( ( ( ToTbyD - gausParametersElectron[4] ) / gausParametersElectron[5] ) *
                                                           ( ( ToTbyD - gausParametersElectron[4] ) / gausParametersElectron[5] ) ) ) +

                 gausParametersElectron[6] * exp( -0.5 * ( ( ( ToTbyD - gausParametersElectron[7] ) / gausParametersElectron[8] ) *
                                                           ( ( ToTbyD - gausParametersElectron[7] ) / gausParametersElectron[8] ) ) ) );
  
  double pioP = (gausParametersPion[0] * exp( -0.5 * ( ( ( ToTbyD - gausParametersPion[1] ) / gausParametersPion[2] ) *
                                                       ( ( ToTbyD - gausParametersPion[1] ) / gausParametersPion[2] ) ) ) +
                 
                 gausParametersPion[3] * exp( -0.5 * ( ( ( ToTbyD - gausParametersPion[4] ) / gausParametersPion[5] ) *
                                                       ( ( ToTbyD - gausParametersPion[4] ) / gausParametersPion[5] ) ) ) +
                 
                 gausParametersPion[6] * exp( -0.5 * ( ( ( ToTbyD - gausParametersPion[7] ) / gausParametersPion[8] ) *
                                                       ( ( ToTbyD - gausParametersPion[7] ) / gausParametersPion[8] ) ) ) );
  
  if( (eleP+pioP) != 0)
    return eleP/(eleP+pioP);
  else
    return 0.5;
}



/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  turn the BEC, strawlayer and layer information into a linear index  %%*|
|*%%%  that can be used on a flat array.  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

int InDet::TRT_ElectronPidTool::ToTcalculator::resolveIndex(int BEC, int Layer, int Strawlayer=-9999){

  int part=abs(BEC)-1;

  static const int strawsBeforeBarrelLayer[]={0, 19, 43};

  static const int parametersPerBEC[2]={NparsBarrel,NparsEndcap};

  // where do the constatnts for one particular module start (by BEC+2)
  static const int startOfModule[5]={0, //start at the beginning
                                     NparametrizedSectionsEndcap * NparsEndcap,  // after the first endcap
                                     -9999, // never encountered, correcponds to BEC==0
                                     NparametrizedSectionsEndcap * NparsEndcap + NparametrizedSectionsBarrel * NparsBarrel, // after the first endcap and the first barrel
                                     NparametrizedSectionsEndcap * NparsEndcap + 2 * NparametrizedSectionsBarrel * NparsBarrel }; // after the first endcap and both barrels

  int offset;

  // First take care of the fact that we split the Barrel up after individual straw layers,
  //  while the endcap is treated simply on a by layer basis
  if(abs(BEC)==1) {
     if (Layer > 2) {
        offset = Strawlayer; 
        parent.msg(MSG::WARNING) << "WARNING Something went wrong! For barrel, got Layer == " << Layer << " which is greater than 2"<<endmsg;
     }
     else {offset = Strawlayer + strawsBeforeBarrelLayer[Layer];}
  }
  else {
    offset=Layer;
  }

  // the offset is now in parametersets.
  // multiply by the size of the parameterset to get the offset in floats:
  offset*=parametersPerBEC[part];
  
  // the offset is now from the start of the module, now add the module dependent offset:
  offset+=startOfModule[BEC+2];

  if( not (offset < N_TOT_CONSTANTS ) ){
    parent.msg(MSG::ERROR)<<"In ToTcalculator: something must have gone wrong with the index resolver, found an index that is larger than  N_TOT_CONSTANTS"<<endmsg;
    parent.msg(MSG::ERROR)<<"BEC="<<BEC<<", Layer="<<Layer<<", Strawlayer="<<Strawlayer<<" resolved to "<<offset<<" which is outside of "<<int(N_TOT_CONSTANTS)<<endmsg;
    return 0;
  }

  return offset;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Hard-coded ToT Calibration Constants  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

void InDet::TRT_ElectronPidTool::ToTcalculator::setDefaultCalibrationConstants(){
  /*****************************************************************************\

    This code is never called in production. It is used to set all
    constants in the positions in the ToTBlob where they are needed, and 
    finally print out the blob as an array of numbers. This is far easier and 
    less error prone than having a separate setter-script which might itself 
    have a version mismatch with this code.

    PLEASE REMEMBER to increment the version number precisely when you change 
    the addresses of the various arrays inside the ToTBlob, and NEVER otherwise!
    
  \*****************************************************************************/
  HasBeenInitialized=1;
  // BEC constants
  static const int EndcapNeg = -2;
  static const int BarrelNeg = -1;
  static const int BarrelPos =  1;
  static const int EndcapPos =  2;

  Blob[_Version]=CurrentVersion;
  {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    Blob[_Day]=timeinfo->tm_mday;
    Blob[_Month]=timeinfo->tm_mon+1;
    Blob[_Year]=(timeinfo->tm_year)%100;
  }
  UpperLimit=0.98;
  LowerLimit=0.02;

  /* Data */

  parValBarrelLongNeg = 16.5306;
  parValBarrelLongPos = 16.5308;
  parValBarrelShortNeg = 19.2477;
  parValBarrelShortPos = 19.0788;

  gausParametersElectron[0]=7.21288e+02 / 21545.6513898;
  gausParametersElectron[1]=6.59160e+00;
  gausParametersElectron[2]=-1.14696e+00;
  gausParametersElectron[3]=4.94402e+02 / 21545.6513898 ;
  gausParametersElectron[4]=7.21959e+00;
  gausParametersElectron[5]=3.50076e-01 ;
  gausParametersElectron[6]=1.47287e+04 / 21545.6513898;
  gausParametersElectron[7]=6.16054e+00;
  gausParametersElectron[8]=5.15667e-01 ;
    
  gausParametersPion[0]=1.13734e+07 / 17656949.2231;
  gausParametersPion[1]=4.90641e+00;
  gausParametersPion[2]=-4.03279e-01;
  gausParametersPion[3]=4.63741e+06 / 17656949.2231;
  gausParametersPion[4]=5.31315e+00;
  gausParametersPion[5]=4.91662e-01 ;
  gausParametersPion[6]=2.05522e+05 / 17656949.2231;
  gausParametersPion[7]=6.18323e+00;
  gausParametersPion[8]=8.63226e-01 ;

  //---------------------------------------------------------------------------------------------------
  //  ToTCorrectionConstants[resolveIndex(BEC,Layer,Strawlayer)+PAR0];

  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 0) ] = -361.001;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 0) ] = 1056.19;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 0) ] = 1.38385e+08;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 0) ] = 1.34844e+08;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 0) ] = 18.5289;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 1) ] = -5360.7;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 1) ] = 6726.87;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 1) ] = 4.43262e+08;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 1) ] = 1.98054e+09;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 1) ] = 18.5009;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 2) ] = 4.99001;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 2) ] = 19.7789;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 2) ] = 2.6042e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 2) ] = 5.29042e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 2) ] = 18.2252;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 3) ] = 1.94621;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 3) ] = 49.4971;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 3) ] = 8.36338e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 3) ] = 7.05244e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 3) ] = 18.1467;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 4) ] = 13.6863;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 4) ] = 12.0519;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 4) ] = 2.7602e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 4) ] = 2.13675e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 4) ] = 18.1505;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 5) ] = 15.5639;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 5) ] = 4.97136;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 5) ] = 1.52817e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 5) ] = 1.31715e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 5) ] = 18.1146;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 6) ] = -5777.44;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 6) ] = 17796.3;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 6) ] = 3.22137e+09;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 6) ] = 2.87108e+09;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 6) ] = 17.7181;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 7) ] = 14.663;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 7) ] = 4.7284;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 7) ] = 1.11746e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 7) ] = 2.49389e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 7) ] = 17.7053;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 8) ] = 16.7397;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 8) ] = 15.5089;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 8) ] = 3.22312e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 8) ] = 1.15271e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 8) ] = 17.3967;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 9) ] = 16.8959;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 9) ] = 8.21084;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 9) ] = 2.40447e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 9) ] = 1.0258e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 9) ] = 17.5003;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 10) ] = 15.2276;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 10) ] = 17.2821;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 10) ] = 4.14328e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 10) ] = 2.00192e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 10) ] = 17.5807;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 11) ] = -1889.01;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 11) ] = 3110.58;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 11) ] = 6.14415e+08;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 11) ] = 1.2549e+09;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 11) ] = 17.5224;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 12) ] = -18539.9;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 12) ] = 17729.2;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 12) ] = -5.14811e+08;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 12) ] = 1.12807e+10;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 12) ] = 17.3586;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 13) ] = -1844.47;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 13) ] = 5124.25;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 13) ] = 1.05124e+09;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 13) ] = 1.03815e+09;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 13) ] = 16.9787;
  //  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 14) ] = 16.6804;
  //  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 14) ] = 225.711;
  //  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 14) ] = 1.50857e+06;
  //  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 14) ] = 2.04088e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapPos, 14) ] = 148.745;
  //---------------------------------------------------------------------------------------------------  
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 0) ] = -4193.59;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 0) ] = 14357.2;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 0) ] = 1.76951e+09;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 0) ] = 1.44264e+09;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 0) ] = 18.6464;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 1) ] = -11262.8;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 1) ] = 14051.9;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 1) ] = 8.84899e+08;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 1) ] = 4.02754e+09;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 1) ] = 18.3413;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 2) ] = -171.332;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 2) ] = 215.888;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 2) ] = 9.63327e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 2) ] = 7.08746e+07;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 2) ] = 18.0658;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 3) ] = 12.7826;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 3) ] = 22.5418;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 3) ] = 4.00044e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 3) ] = 2.49764e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 3) ] = 17.9822;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 4) ] = 14.8661;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 4) ] = 5.50288;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 4) ] = 1.33966e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 4) ] = 1.79218e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 4) ] = 17.7073;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 5) ] = 12.0988;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 5) ] = 59.557;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 5) ] = 7.65208e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 5) ] = 3.15691e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 5) ] = 17.6987;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 6) ] = 13.1897;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 6) ] = 1.96233;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 6) ] = -1.90674e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 6) ] = 2.77615e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 6) ] = 18.345;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 7) ] = 15.1971;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 7) ] = 16.8885;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 7) ] = 3.76656e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 7) ] = 1.75758e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 7) ] = 18.2302;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 8) ] = 15.2791;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 8) ] = 26.902;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 8) ] = 4.35497e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 8) ] = 1.55011e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 8) ] = 18.0964;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 9) ] = 16.5057;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 9) ] = 4.83938;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 9) ] = 1.80912e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 9) ] = 881288;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 9) ] = 18.0762;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 10) ] = -296.491;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 10) ] = 26951.5;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 10) ] = 8.80194e+08;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 10) ] = 1.97607e+08;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 10) ] = 17.9113;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 11) ] = -663.676;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 11) ] = 973.365;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 11) ] = 1.59632e+08;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 11) ] = 4.46105e+08;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 11) ] = 17.9858;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 12) ] = -3064.59;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 12) ] = 5778.6;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 12) ] = 1.19256e+09;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 12) ] = 1.89652e+09;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 12) ] = 17.8263;
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 13) ] = -4338.77;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 13) ] = 11246.1;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 13) ] = 2.32016e+09;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 13) ] = 2.44563e+09;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 13) ] = 17.7572;
  //  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 14) ] = 16.6804;
  //  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 14) ] = 225.711;
  //  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 14) ] = 1.50857e+06;
  //  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 14) ] = 2.04088e+06;
  //  ToTCorrectionConstants[ PARVAL+ resolveIndex( EndcapNeg, 14) ] = 148.745;
  //---------------------------------------------------------------------------------------------------

  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 0) ] = 17.1648;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 0) ] = 83385;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 0) ] = 6.64843e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 0) ] = 17.1732;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 1) ] = 17.0428;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 1) ] = 80545.9;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 1) ] = 1.46919e+07;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 1) ] = 17.0514;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 2) ] = 17.4083;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 2) ] = 110448;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 2) ] = 1.77359e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 2) ] = 17.4146;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 3) ] = 17.3203;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 3) ] = 98089.4;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 3) ] = 2.56151e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 3) ] = 17.3274;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 4) ] = 17.2442;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 4) ] = 89749.5;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 4) ] = 4.28931e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 4) ] = 17.252;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 5) ] = 17.3975;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 5) ] = 99836.2;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 5) ] = 2.46783e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 5) ] = 17.4044;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 6) ] = 17.251;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 6) ] = 100980;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 6) ] = 2.47867e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 6) ] = 17.2579;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 7) ] = 17.4156;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 7) ] = 139944;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 7) ] = 1.09326e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 7) ] = 17.4206;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 8) ] = 16.7497;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 8) ] = 122245;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 8) ] = 1.48192e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 8) ] = 16.7554;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 9) ] = 16.774;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 9) ] = 217.809;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 9) ] = 268.564;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 9) ] = 17.0062;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 10) ] = 16.4844;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 10) ] = 222.683;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 10) ] = 266.315;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 10) ] = 16.7065;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 11) ] = 16.3632;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 11) ] = 225.21;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 11) ] = 261.36;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 11) ] = 16.572;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 12) ] = 16.2991;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 12) ] = 235.856;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 12) ] = 270.015;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 12) ] = 16.5166;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 13) ] = 16.3739;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 13) ] = 231.908;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 13) ] = 263.725;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 13) ] = 16.5817;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 14) ] = 16.3912;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 14) ] = 236.605;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 14) ] = 261.679;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 14) ] = 16.5907;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 15) ] = 16.4241;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 15) ] = 238.15;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 15) ] = 264.851;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 15) ] = 16.6287;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 16) ] = 16.4165;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 16) ] = 242.458;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 16) ] = 262.662;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 16) ] = 16.6131;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 17) ] = 16.45;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 17) ] = 248.146;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 17) ] = 269.243;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 17) ] = 16.6552;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 18) ] = 16.4476;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 18) ] = 251.213;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 18) ] = 264.643;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 0, 18) ] = 16.6412;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 0) ] = 16.4161;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 0) ] = 259.866;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 0) ] = 270.391;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 0) ] = 16.6142;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 1) ] = 16.3506;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 1) ] = 267.967;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 1) ] = 266.274;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 1) ] = 16.5351;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 2) ] = 16.4134;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 2) ] = 270.764;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 2) ] = 262.719;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 2) ] = 16.5896;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 3) ] = 16.4242;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 3) ] = 269.361;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 3) ] = 262.053;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 3) ] = 16.6001;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 4) ] = 16.4364;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 4) ] = 272.263;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 4) ] = 253.575;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 4) ] = 16.5955;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 5) ] = 16.4903;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 5) ] = 270.457;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 5) ] = 248.092;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 5) ] = 16.6408;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 6) ] = 16.4571;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 6) ] = 271.724;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 6) ] = 243.419;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 6) ] = 16.599;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 7) ] = 16.4021;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 7) ] = 276.711;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 7) ] = 244.326;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 7) ] = 16.5429;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 8) ] = 16.4139;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 8) ] = 282.263;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 8) ] = 245.117;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 8) ] = 16.5533;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 9) ] = 16.4287;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 9) ] = 281.192;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 9) ] = 238.671;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 9) ] = 16.5581;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 10) ] = 16.3864;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 10) ] = 285.481;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 10) ] = 238.946;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 10) ] = 16.5144;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 11) ] = 16.4427;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 11) ] = 288.229;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 11) ] = 237.705;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 11) ] = 16.5675;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 12) ] = 16.3928;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 12) ] = 291.153;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 12) ] = 244.119;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 12) ] = 16.5263;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 13) ] = 16.4095;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 13) ] = 299.029;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 13) ] = 247.007;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 13) ] = 16.544;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 14) ] = 16.372;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 14) ] = 301.199;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 14) ] = 239.162;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 14) ] = 16.4936;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 15) ] = 16.4245;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 15) ] = 304.547;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 15) ] = 240.098;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 15) ] = 16.5461;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 16) ] = 16.4528;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 16) ] = 306.7;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 16) ] = 243.319;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 16) ] = 16.5784;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 17) ] = 16.5062;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 17) ] = 313.006;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 17) ] = 242.605;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 17) ] = 16.6282;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 18) ] = 16.4456;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 18) ] = 321.902;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 18) ] = 246.374;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 18) ] = 16.5696;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 19) ] = 16.5164;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 19) ] = 324.872;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 19) ] = 247.049;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 19) ] = 16.6403;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 20) ] = 16.4567;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 20) ] = 330.972;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 20) ] = 243.561;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 20) ] = 16.5734;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 21) ] = 16.4014;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 21) ] = 337.737;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 21) ] = 253.332;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 21) ] = 16.5293;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 22) ] = 16.4942;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 22) ] = 344.993;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 22) ] = 251.47;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 22) ] = 16.6168;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 23) ] = 16.457;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 23) ] = 348.206;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 23) ] = 254.389;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 1, 23) ] = 16.5824;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 0) ] = 16.4002;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 0) ] = 362.754;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 0) ] = 235.592;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 0) ] = 16.4968;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 1) ] = 16.3857;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 1) ] = 360.531;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 1) ] = 235.164;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 1) ] = 16.4824;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 2) ] = 16.3241;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 2) ] = 362.805;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 2) ] = 228.974;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 2) ] = 16.4127;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 3) ] = 16.3652;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 3) ] = 363.111;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 3) ] = 232.48;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 3) ] = 16.4579;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 4) ] = 16.4269;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 4) ] = 362.924;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 4) ] = 221.956;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 4) ] = 16.5072;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 5) ] = 16.4122;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 5) ] = 364.066;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 5) ] = 223.214;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 5) ] = 16.4937;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 6) ] = 16.3961;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 6) ] = 366.814;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 6) ] = 219.647;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 6) ] = 16.473;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 7) ] = 16.3793;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 7) ] = 369.039;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 7) ] = 218.968;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 7) ] = 16.4549;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 8) ] = 16.3857;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 8) ] = 375.277;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 8) ] = 214.909;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 8) ] = 16.4557;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 9) ] = 16.394;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 9) ] = 374.764;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 9) ] = 217.351;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 9) ] = 16.4667;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 10) ] = 16.3118;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 10) ] = 374.533;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 10) ] = 211.972;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 10) ] = 16.3788;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 11) ] = 16.3187;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 11) ] = 378;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 11) ] = 215.39;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 11) ] = 16.3887;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 12) ] = 16.3606;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 12) ] = 380.565;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 12) ] = 206.065;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 12) ] = 16.4206;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 13) ] = 16.3279;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 13) ] = 385.944;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 13) ] = 206.306;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 13) ] = 16.3873;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 14) ] = 16.2987;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 14) ] = 385.274;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 14) ] = 205.406;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 14) ] = 16.3573;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 15) ] = 16.3689;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 15) ] = 384.61;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 15) ] = 202.586;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 15) ] = 16.4249;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 16) ] = 16.3813;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 16) ] = 389.806;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 16) ] = 208.153;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 16) ] = 16.4419;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 17) ] = 16.3875;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 17) ] = 393.024;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 17) ] = 207.273;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 17) ] = 16.4467;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 18) ] = 16.3088;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 18) ] = 400.953;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 18) ] = 201.301;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 18) ] = 16.3613;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 19) ] = 16.3408;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 19) ] = 402.562;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 19) ] = 203.062;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 19) ] = 16.3947;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 20) ] = 16.3815;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 20) ] = 406.069;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 20) ] = 200.755;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 20) ] = 16.4328;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 21) ] = 16.3895;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 21) ] = 411.391;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 21) ] = 204.42;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 21) ] = 16.4434;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 22) ] = 16.3169;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 22) ] = 419.486;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 22) ] = 202.975;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 22) ] = 16.3686;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 23) ] = 16.3083;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 23) ] = 423.337;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 23) ] = 198.199;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 23) ] = 16.3554;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 24) ] = 16.3305;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 24) ] = 426.703;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 24) ] = 204.534;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 24) ] = 16.3826;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 25) ] = 16.3767;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 25) ] = 436.931;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 25) ] = 206.552;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 25) ] = 16.4293;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 26) ] = 16.3353;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 26) ] = 452.882;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 26) ] = 204.493;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 26) ] = 16.3844;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 27) ] = 16.3283;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 27) ] = 457.209;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 27) ] = 202.611;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 27) ] = 16.3754;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 28) ] = 16.3827;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 28) ] = 462.656;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 28) ] = 207.001;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 28) ] = 16.4328;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 29) ] = 16.3803;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 29) ] = 473.432;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 29) ] = 213.062;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelPos, 2, 29) ] = 16.4342;
  //---------------------------------------------------------------------------------------------------
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 0) ] = 17.2201;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 0) ] = 77657.8;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 0) ] = 2.05668e+13;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 0) ] = 17.2291;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 1) ] = 17.245;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 1) ] = 79307;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 1) ] = 2.43455e+13;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 1) ] = 17.2538;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 2) ] = 17.6764;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 2) ] = 112810;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 2) ] = 1.83723e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 2) ] = 17.6826;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 3) ] = 17.2166;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 3) ] = 82984.8;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 3) ] = 1.77907e+07;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 3) ] = 17.225;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 4) ] = 17.4748;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 4) ] = 92230.4;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 4) ] = 4.3814e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 4) ] = 17.4823;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 5) ] = 17.6057;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 5) ] = 103098;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 5) ] = 2.54733e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 5) ] = 17.6125;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 6) ] = 17.3653;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 6) ] = 103209;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 6) ] = 2.68794e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 6) ] = 17.3721;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 7) ] = 17.422;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 7) ] = 130180;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 7) ] = 1.43041e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 7) ] = 17.4273;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 8) ] = 16.7569;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 8) ] = 111957;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 8) ] = 2.23147e+06;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 8) ] = 16.7631;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 9) ] = 16.851;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 9) ] = 228.842;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 9) ] = 278.269;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 9) ] = 17.0931;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 10) ] = 16.5599;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 10) ] = 235.178;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 10) ] = 276.739;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 10) ] = 16.7923;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 11) ] = 16.4607;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 11) ] = 238.178;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 11) ] = 272.689;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 11) ] = 16.6817;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 12) ] = 16.3212;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 12) ] = 248.056;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 12) ] = 276.312;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 12) ] = 16.5406;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 13) ] = 16.3927;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 13) ] = 246.51;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 13) ] = 268.753;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 13) ] = 16.5982;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 14) ] = 16.4557;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 14) ] = 249.682;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 14) ] = 264.357;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 14) ] = 16.65;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 15) ] = 16.4854;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 15) ] = 251.745;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 15) ] = 264.009;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 15) ] = 16.6773;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 16) ] = 16.5013;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 16) ] = 255.059;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 16) ] = 268.377;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 16) ] = 16.6992;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 17) ] = 16.4562;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 17) ] = 262.892;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 17) ] = 267.259;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 17) ] = 16.6461;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 18) ] = 16.4657;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 18) ] = 264.486;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 18) ] = 272.782;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 0, 18) ] = 16.6649;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 0) ] = 16.3103;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 0) ] = 276.559;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 0) ] = 265.761;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 0) ] = 16.4881;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 1) ] = 16.344;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 1) ] = 280.464;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 1) ] = 260.52;   
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 1) ] = 16.5103;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 2) ] = 16.3052;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 2) ] = 284.99;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 2) ] = 258.521;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 2) ] = 16.4655;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 3) ] = 16.3284;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 3) ] = 282.115;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 3) ] = 253.977;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 3) ] = 16.4826;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 4) ] = 16.4184;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 4) ] = 284.082;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 4) ] = 246.97;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 4) ] = 16.56;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 5) ] = 16.3677;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 5) ] = 282.965;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 5) ] = 244.932;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 5) ] = 16.5065;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 6) ] = 16.3946;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 6) ] = 282.727;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 6) ] = 241.664;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 6) ] = 16.5281;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 7) ] = 16.3548;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 7) ] = 289.494;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 7) ] = 241.788;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 7) ] = 16.4855;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 8) ] = 16.3626;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 8) ] = 292.221;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 8) ] = 243.173;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 8) ] = 16.4942;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 9) ] = 16.4512;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 9) ] = 289.463;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 9) ] = 232.28;     
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 9) ] = 16.5671;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 10) ] = 16.409;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 10) ] = 294.661;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 10) ] = 230.547;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 10) ] = 16.5204;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 11) ] = 16.455;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 11) ] = 296.682;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 11) ] = 232.385;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 11) ] = 16.5683;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 12) ] = 16.4035;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 12) ] = 297.929;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 12) ] = 234.274;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 12) ] = 16.5191;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 13) ] = 16.4333;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 13) ] = 304.895;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 13) ] = 240.799;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 13) ] = 16.5559;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 14) ] = 16.3847;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 14) ] = 305.584;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 14) ] = 236.21;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 14) ] = 16.5002;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 15) ] = 16.4184;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 15) ] = 311.917;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 15) ] = 233.756;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 15) ] = 16.5281;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 16) ] = 16.4532;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 16) ] = 313.05;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 16) ] = 233.214;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 16) ] = 16.5617;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 17) ] = 16.4456;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 17) ] = 320.818;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 17) ] = 234.775;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 17) ] = 16.5537;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 18) ] = 16.5229;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 18) ] = 324.488;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 18) ] = 232.388;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 18) ] = 16.6265;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 19) ] = 16.5161;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 19) ] = 331.13;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 19) ] = 235.526;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 19) ] = 16.6218;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 20) ] = 16.4663;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 20) ] = 334.782;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 20) ] = 235.252;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 20) ] = 16.5705;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 21) ] = 16.4257;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 21) ] = 339.834;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 21) ] = 241.462;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 21) ] = 16.5366;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 22) ] = 16.4668;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 22) ] = 347.79;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 22) ] = 242.529;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 22) ] = 16.5765;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 23) ] = 16.4681;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 23) ] = 354.133;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 23) ] = 243.889;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 23) ] = 16.5776;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 1, 29) ] = 22.5978;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 0) ] = 16.4176;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 0) ] = 362.39;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 0) ] = 242.94;   
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 0) ] = 16.5234;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 1) ] = 16.4283;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 1) ] = 363.687;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 1) ] = 241.355;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 1) ] = 16.5318;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 2) ] = 16.36;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 2) ] = 364.209;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 2) ] = 240.096;   
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 2) ] = 16.4617;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 3) ] = 16.3701;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 3) ] = 368.273;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 3) ] = 236.289;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 3) ] = 16.4661;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 4) ] = 16.4472;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 4) ] = 371.292;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 4) ] = 229.104;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 4) ] = 16.5339;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 5) ] = 16.3927;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 5) ] = 372.885;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 5) ] = 229.713;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 5) ] = 16.4797;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 6) ] = 16.3477;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 6) ] = 372.726;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 6) ] = 221.466;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 6) ] = 16.4254;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 7) ] = 16.3849;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 7) ] = 376.555;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 7) ] = 224.71;  
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 7) ] = 16.4654;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 8) ] = 16.3504;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 8) ] = 380.803;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 8) ] = 219.346;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 8) ] = 16.4242;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 9) ] = 16.3717;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 9) ] = 382.715;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 9) ] = 219.756;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 9) ] = 16.4454;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 10) ] = 16.3067;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 10) ] = 385.197;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 10) ] = 215.773;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 10) ] = 16.3758;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 11) ] = 16.3171;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 11) ] = 388.504;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 11) ] = 214.139;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 11) ] = 16.3839;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 12) ] = 16.2842;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 12) ] = 395.379;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 12) ] = 209.704;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 12) ] = 16.3454;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 13) ] = 16.3173;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 13) ] = 395.654;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 13) ] = 206.161;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 13) ] = 16.3751;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 14) ] = 16.3403;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 14) ] = 398.331;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 14) ] = 203.895;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 14) ] = 16.3955;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 15) ] = 16.3713;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 15) ] = 397.673;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 15) ] = 208.123;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 15) ] = 16.4307;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 16) ] = 16.4063;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 16) ] = 400.237;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 16) ] = 211.825;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 16) ] = 16.4689;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 17) ] = 16.4343;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 17) ] = 405.452;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 17) ] = 210.94;  
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 17) ] = 16.4952;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 18) ] = 16.3505;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 18) ] = 413.653;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 18) ] = 204.853;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 18) ] = 16.4046;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 19) ] = 16.3036;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 19) ] = 416.53;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 19) ] = 209.314;  
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 19) ] = 16.3614;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 20) ] = 16.386;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 20) ] = 416.454;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 20) ] = 205.721;  
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 20) ] = 16.4405;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 21) ] = 16.3735;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 21) ] = 425.249;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 21) ] = 206.48;  
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 21) ] = 16.4275;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 22) ] = 16.3099;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 22) ] = 429.241;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 22) ] = 205.723;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 22) ] = 16.3628;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 23) ] = 16.2008;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 23) ] = 434.183;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 23) ] = 207.06;  
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 23) ] = 16.2542;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 24) ] = 16.339;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 24) ] = 443.952;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 24) ] = 206.046; 
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 24) ] = 16.3904;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 25) ] = 16.3504;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 25) ] = 449.336;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 25) ] = 209.784;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 25) ] = 16.4044;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 26) ] = 16.3581;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 26) ] = 463.901;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 26) ] = 208.928;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 26) ] = 16.4097;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 27) ] = 16.3491;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 27) ] = 468.166;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 27) ] = 207.115;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 27) ] = 16.3987;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 28) ] = 16.3822;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 28) ] = 475.266;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 28) ] = 209.026;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 28) ] = 16.4326;
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 29) ] = 16.4306;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 29) ] = 484.997;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 29) ] = 217.403;
  //  ToTCorrectionConstants[ PARVAL + resolveIndex( BarrelNeg, 2, 29) ] = 16.4868;
  /*Data*/

  /*MC

    
  gausParametersElectron[0]=4.42618e+03/9464.67582986;
  gausParametersElectron[1]=6.01833e+00;
  gausParametersElectron[2]=4.42918e-01;
  gausParametersElectron[3]=1.21274e+02/9464.67582986;
  gausParametersElectron[4]=7.05030e+00;
  gausParametersElectron[5]=1.34858e+00;
  gausParametersElectron[6]=2.49294e+03/9464.67582986;
  gausParametersElectron[7]=6.19950e+00;
  gausParametersElectron[8]=6.62622e-01;
  
  gausParametersPion[0]=1.34389e+06/5321869.88143;
  gausParametersPion[1]=5.33348e+00;
  gausParametersPion[2]=5.01469e-01;
  gausParametersPion[3]=3.52869e+06/5321869.88143;
  gausParametersPion[4]=4.94735e+00;
  gausParametersPion[5]=4.01352e-01;
  gausParametersPion[6]=3.60485e+04/5321869.88143;
  gausParametersPion[7]=6.16533e+00;
  gausParametersPion[8]=9.14128e-01;
  
  parValBarrelLongNeg = 16.5306;
  parValBarrelLongPos = 16.5308;
  parValBarrelShortNeg = 19.2477;
  parValBarrelShortPos = 19.0788;
  
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 0 ) ] = 15.276;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 0 ) ] = 30.1367;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 0 ) ] = 3.91321e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 0 ) ] = 1.72123e+06; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 1 ) ] = 4.0676;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 1 ) ] = 55.4918;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 1 ) ] = 8.4327e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 1 ) ] = 6.07447e+06; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 2 ) ] = 16.9878;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 2 ) ] = 18.2382;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 2 ) ] = 2.59662e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 2 ) ] = 921757; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 3 ) ] = 17.5059;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 3 ) ] = 9.6671;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 3 ) ] = 1.96708e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 3 ) ] = 703677; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 4 ) ] = 17.7666;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 4 ) ] = 14.4802;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 4 ) ] = 2.13944e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 4 ) ] = 574198; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 5 ) ] = 17.9393;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 5 ) ] = 9.69865;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 5 ) ] = 1.8712e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 5 ) ] = 493120; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 6 ) ] = 18.0258;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 6 ) ] = 10.8898;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 6 ) ] = 1.95067e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 6 ) ] = 473052; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 7 ) ] = 18.0669;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 7 ) ] = 27.7303;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 7 ) ] = 2.29756e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 7 ) ] = 426550; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 8 ) ] = 18.0663;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 8 ) ] = 5.73261;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 8 ) ] = 1.64648e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 8 ) ] = 427677; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 9 ) ] = 18.1227;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 9 ) ] = 4.18172;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 9 ) ] = 1.49183e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 9 ) ] = 390630; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 10 ) ] = 18.0223;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 10 ) ] = 2.33984;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 10 ) ] = 1.28904e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 10 ) ] = 456755; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 11 ) ] = 18.0501;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 11 ) ] = 2.34491;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 11 ) ] = 1.27889e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 11 ) ] = 425862; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 12 ) ] = -1042.92;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 12 ) ] = 11437.9;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 12 ) ] = 1.53023e+09;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 12 ) ] = 6.43425e+08; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapPos, 13 ) ] = -2748.66;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapPos, 13 ) ] = 28010.1;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapPos, 13 ) ] = 3.51061e+09;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapPos, 13 ) ] = 1.51636e+09;  
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 0 ) ] = 15.8752;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 0 ) ] = 25.7239;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 0 ) ] = 3.47485e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 0 ) ] = 1.50458e+06; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 1 ) ] = 1.15483;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 1 ) ] = 27.3478;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 1 ) ] = 3.52052e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 1 ) ] = 7.17056e+06; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 2 ) ] = 16.9712;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 2 ) ] = 4.97552;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 2 ) ] = 1.39363e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 2 ) ] = 924500; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 3 ) ] = 17.489;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 3 ) ] = 4.16836;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 3 ) ] = 1.36938e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 3 ) ] = 707499; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 4 ) ] = 17.7818;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 4 ) ] = 14.6824;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 4 ) ] = 2.13567e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 4 ) ] = 567219; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 5 ) ] = 17.9323;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 5 ) ] = 9.57073;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 5 ) ] = 1.86242e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 5 ) ] = 493893; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 6 ) ] = 18.0239;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 6 ) ] = 10.9606;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 6 ) ] = 1.95266e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 6 ) ] = 472734; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 7 ) ] = 18.0883;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 7 ) ] = 2.28047;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 7 ) ] = 1.22652e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 7 ) ] = 410213; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 8 ) ] = 18.0682;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 8 ) ] = 4.25198;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 8 ) ] = 1.51798e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 8 ) ] = 426605; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 9 ) ] = 18.1081;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 9 ) ] = 5.36213;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 9 ) ] = 1.60989e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 9 ) ] = 405538; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 10 ) ] = 18.034;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 10 ) ] = 5.26541;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 10 ) ] = 1.64241e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 10 ) ] = 444893; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 11 ) ] = 18.0632;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 11 ) ] = 9.2788;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 11 ) ] = 1.85654e+06;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 11 ) ] = 419795; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 12 ) ] = -671.42;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 12 ) ] = 1596.46;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 12 ) ] = 3.48688e+08;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 12 ) ] = 4.14895e+08; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( EndcapNeg, 13 ) ] = -371.03;
  ToTCorrectionConstants[ PAR1 + resolveIndex( EndcapNeg, 13 ) ] = 359.766;
  ToTCorrectionConstants[ PAR2 + resolveIndex( EndcapNeg, 13 ) ] = -1.64905e+07;
  ToTCorrectionConstants[ PAR3 + resolveIndex( EndcapNeg, 13 ) ] = 2.15353e+08; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 0 ) ] = 17.9835;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 0 ) ] = 78095.7;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 0 ) ] = 1.45842e+15; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 1 ) ] = 17.9538;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 1 ) ] = 79480.8;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 1 ) ] = 5.49645e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 2 ) ] = 17.9467;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 2 ) ] = 81628.9;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 2 ) ] = 3.42582e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 3 ) ] = 17.9132;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 3 ) ] = 81780.6;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 3 ) ] = 1.91304e+15; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 4 ) ] = 17.9524;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 4 ) ] = 83861.7;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 4 ) ] = 1.81813e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 5 ) ] = 17.9543;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 5 ) ] = 85617.5;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 5 ) ] = 7.43655e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 6 ) ] = 17.9382;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 6 ) ] = 86335.9;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 6 ) ] = 5.43576e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 7 ) ] = 18.0155;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 7 ) ] = 91195.7;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 7 ) ] = 1.26446e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 8 ) ] = 17.9843;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 8 ) ] = 90991.1;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 8 ) ] = 6.86286e+13; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 9 ) ] = 16.3945;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 9 ) ] = 158.896;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 9 ) ] = 481.758; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 10 ) ] = 16.4145;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 10 ) ] = 160.505;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 10 ) ] = 476.913; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 11 ) ] = 16.3941;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 11 ) ] = 162.197;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 11 ) ] = 483.361; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 12 ) ] = 16.3582;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 12 ) ] = 165.204;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 12 ) ] = 497.721; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 13 ) ] = 16.3854;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 13 ) ] = 165.765;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 13 ) ] = 488.027; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 14 ) ] = 16.3976;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 14 ) ] = 167.489;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 14 ) ] = 485.805; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 15 ) ] = 16.4092;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 15 ) ] = 169.507;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 15 ) ] = 484.609; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 16 ) ] = 16.4068;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 16 ) ] = 171.203;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 16 ) ] = 483.515; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 17 ) ] = 16.4143;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 17 ) ] = 174.062;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 17 ) ] = 489.134; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 0, 18 ) ] = 16.4339;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 0, 18 ) ] = 174.433;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 0, 18 ) ] = 481.582; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 0 ) ] = 16.4837;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 0 ) ] = 177.029;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 0 ) ] = 477.465; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 1 ) ] = 16.456;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 1 ) ] = 179.382;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 1 ) ] = 483.005; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 2 ) ] = 16.4525;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 2 ) ] = 181.846;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 2 ) ] = 486.727; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 3 ) ] = 16.4666;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 3 ) ] = 183.048;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 3 ) ] = 481.086; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 4 ) ] = 16.4743;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 4 ) ] = 185.027;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 4 ) ] = 482.106; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 5 ) ] = 16.4866;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 5 ) ] = 186.099;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 5 ) ] = 476.156; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 6 ) ] = 16.4818;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 6 ) ] = 187.654;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 6 ) ] = 478.862; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 7 ) ] = 16.5057;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 7 ) ] = 189.464;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 7 ) ] = 473.55; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 8 ) ] = 16.4719;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 8 ) ] = 191.807;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 8 ) ] = 485.814; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 9 ) ] = 16.518;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 9 ) ] = 193.268;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 9 ) ] = 476.697; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 10 ) ] = 16.5062;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 10 ) ] = 195;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 10 ) ] = 480.744;  
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 11 ) ] = 16.4562;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 11 ) ] = 195.747;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 11 ) ] = 492.7; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 12 ) ] = 16.4976;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 12 ) ] = 198.582;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 12 ) ] = 485.048; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 13 ) ] = 16.4417;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 13 ) ] = 199.928;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 13 ) ] = 503.311; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 14 ) ] = 16.4592;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 14 ) ] = 201.292;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 14 ) ] = 496.886; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 15 ) ] = 16.4766;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 15 ) ] = 203.472;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 15 ) ] = 497.942; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 16 ) ] = 16.4839;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 16 ) ] = 205.174;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 16 ) ] = 496.018; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 17 ) ] = 16.4439;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 17 ) ] = 206.377;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 17 ) ] = 508.162; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 18 ) ] = 16.4869;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 18 ) ] = 208.814;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 18 ) ] = 499.245; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 19 ) ] = 16.4889;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 19 ) ] = 210.476;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 19 ) ] = 500.37; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 20 ) ] = 16.4742;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 20 ) ] = 211.401;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 20 ) ] = 506.651; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 21 ) ] = 16.4733;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 21 ) ] = 213.81;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 21 ) ] = 511.838; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 22 ) ] = 16.4537;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 22 ) ] = 214.291;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 22 ) ] = 517.453; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 1, 23 ) ] = 16.4739;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 1, 23 ) ] = 216.05;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 1, 23 ) ] = 512.408; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 0 ) ] = 16.5288;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 0 ) ] = 218.185;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 0 ) ] = 502.062; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 1 ) ] = 16.4773;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 1 ) ] = 220.175;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 1 ) ] = 515.82; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 2 ) ] = 16.4656;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 2 ) ] = 222.105;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 2 ) ] = 522.132; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 3 ) ] = 16.5025;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 3 ) ] = 223.873;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 3 ) ] = 511.847; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 4 ) ] = 16.4973;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 4 ) ] = 225.201;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 4 ) ] = 515.165; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 5 ) ] = 16.4861;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 5 ) ] = 226.081;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 5 ) ] = 518.619; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 6 ) ] = 16.5029;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 6 ) ] = 227.597;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 6 ) ] = 511.376; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 7 ) ] = 16.4791;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 7 ) ] = 228.952;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 7 ) ] = 519.937; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 8 ) ] = 16.5143;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 8 ) ] = 230.844;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 8 ) ] = 510.614; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 9 ) ] = 16.5122;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 9 ) ] = 231.974;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 9 ) ] = 512.326; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 10 ) ] = 16.5091;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 10 ) ] = 233.581;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 10 ) ] = 514.486; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 11 ) ] = 16.5044;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 11 ) ] = 234.8;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 11 ) ] = 515.174; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 12 ) ] = 16.5156;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 12 ) ] = 236.382;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 12 ) ] = 513.969; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 13 ) ] = 16.5084;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 13 ) ] = 237.532;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 13 ) ] = 514.545; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 14 ) ] = 16.5143;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 14 ) ] = 238.704;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 14 ) ] = 514.74; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 15 ) ] = 16.5088;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 15 ) ] = 240.252;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 15 ) ] = 517.449; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 16 ) ] = 16.5017;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 16 ) ] = 241.447;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 16 ) ] = 519.83; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 17 ) ] = 16.5351;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 17 ) ] = 243.015;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 17 ) ] = 509.168; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 18 ) ] = 16.5394;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 18 ) ] = 244.078;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 18 ) ] = 506.411; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 19 ) ] = 16.5216;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 19 ) ] = 245.329;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 19 ) ] = 513.087; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 20 ) ] = 16.5335;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 20 ) ] = 246.372;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 20 ) ] = 509.636; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 21 ) ] = 16.5223;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 21 ) ] = 247.43;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 21 ) ] = 512.843; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 22 ) ] = 16.5175;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 22 ) ] = 248.924;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 22 ) ] = 517.671; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 23 ) ] = 16.5125;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 23 ) ] = 250.317;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 23 ) ] = 521.496; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 24 ) ] = 16.542;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 24 ) ] = 251.642;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 24 ) ] = 508.397; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 25 ) ] = 16.5564;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 25 ) ] = 253.897;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 25 ) ] = 507.305; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 26 ) ] = 16.5372;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 26 ) ] = 254.053;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 26 ) ] = 511.626; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 27 ) ] = 16.5342;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 27 ) ] = 255.6;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 27 ) ] = 516.366; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 28 ) ] = 16.5281;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 28 ) ] = 256.025;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 28 ) ] = 517.222; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelPos, 2, 29 ) ] = 16.5187;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelPos, 2, 29 ) ] = 257.825;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelPos, 2, 29 ) ] = 524.5; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 0 ) ] = 17.9805;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 0 ) ] = 77824.4;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 0 ) ] = 1.76852e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 1 ) ] = 17.952;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 1 ) ] = 79483.4;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 1 ) ] = 7.49134e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 2 ) ] = 17.9436;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 2 ) ] = 81559.8;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 2 ) ] = 1.06448e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 3 ) ] = 17.9607;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 3 ) ] = 82655.3;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 3 ) ] = 4.75137e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 4 ) ] = 17.9608;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 4 ) ] = 84131.4;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 4 ) ] = 1.35179e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 5 ) ] = 17.9725;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 5 ) ] = 85664.1;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 5 ) ] = 4.56004e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 6 ) ] = 17.9611;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 6 ) ] = 86975.6;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 6 ) ] = 5.77916e+13; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 7 ) ] = 18.0445;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 7 ) ] = 92022.7;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 7 ) ] = 2.30904e+13; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 8 ) ] = 17.9768;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 8 ) ] = 90758.1;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 8 ) ] = 4.31426e+14; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 9 ) ] = 16.3874;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 9 ) ] = 158.839;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 9 ) ] = 482.947; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 10 ) ] = 16.3929;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 10 ) ] = 160.771;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 10 ) ] = 484.375; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 11 ) ] = 16.3795;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 11 ) ] = 162.53;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 11 ) ] = 488.343; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 12 ) ] = 16.3571;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 12 ) ] = 165.537;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 12 ) ] = 499.558; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 13 ) ] = 16.4259;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 13 ) ] = 166.663;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 13 ) ] = 482.585; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 14 ) ] = 16.4137;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 14 ) ] = 168.005;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 14 ) ] = 483.898; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 15 ) ] = 16.4423;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 15 ) ] = 169.65;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 15 ) ] = 478.413; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 16 ) ] = 16.4477;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 16 ) ] = 172.164;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 16 ) ] = 479.076; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 17 ) ] = 16.4479;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 17 ) ] = 173.407;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 17 ) ] = 478.046; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 0, 18 ) ] = 16.4258;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 0, 18 ) ] = 174.897;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 0, 18 ) ] = 484.947; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 0 ) ] = 16.4829;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 0 ) ] = 176.955;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 0 ) ] = 477.122; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 1 ) ] = 16.4737;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 1 ) ] = 179.879;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 1 ) ] = 480.089; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 2 ) ] = 16.4485;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 2 ) ] = 181.06;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 2 ) ] = 483.905; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 3 ) ] = 16.4791;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 3 ) ] = 183.025;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 3 ) ] = 478.242; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 4 ) ] = 16.4888;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 4 ) ] = 184.705;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 4 ) ] = 476.778; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 5 ) ] = 16.4947;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 5 ) ] = 186.615;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 5 ) ] = 476.342; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 6 ) ] = 16.494;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 6 ) ] = 187.633;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 6 ) ] = 476.318; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 7 ) ] = 16.5177;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 7 ) ] = 190.284;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 7 ) ] = 475.69; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 8 ) ] = 16.4424;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 8 ) ] = 191.863;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 8 ) ] = 495.056; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 9 ) ] = 16.4942;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 9 ) ] = 192.979;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 9 ) ] = 482.256; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 10 ) ] = 16.4879;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 10 ) ] = 194.626;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 10 ) ] = 483.562; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 11 ) ] = 16.5103;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 11 ) ] = 197.04;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 11 ) ] = 480.802; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 12 ) ] = 16.5026;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 12 ) ] = 198.655;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 12 ) ] = 485.049; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 13 ) ] = 16.4736;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 13 ) ] = 200.141;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 13 ) ] = 493.542; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 14 ) ] = 16.4998;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 14 ) ] = 202.721;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 14 ) ] = 492.166; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 15 ) ] = 16.4708;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 15 ) ] = 203.259;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 15 ) ] = 497.456; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 16 ) ] = 16.518;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 16 ) ] = 205.81;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 16 ) ] = 488.043; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 17 ) ] = 16.487;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 17 ) ] = 207.013;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 17 ) ] = 497.452; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 18 ) ] = 16.4617;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 18 ) ] = 208.446;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 18 ) ] = 507.295; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 19 ) ] = 16.4995;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 19 ) ] = 210.485;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 19 ) ] = 498.125; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 20 ) ] = 16.4819;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 20 ) ] = 211.687;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 20 ) ] = 505.009; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 21 ) ] = 16.4476;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 21 ) ] = 212.838;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 21 ) ] = 516.704; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 22 ) ] = 16.4603;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 22 ) ] = 214.687;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 22 ) ] = 517.514; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 1, 23 ) ] = 16.5171;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 1, 23 ) ] = 217.217;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 1, 23 ) ] = 502.652; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 0 ) ] = 16.5119;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 0 ) ] = 219.353;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 0 ) ] = 515.422; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 1 ) ] = 16.4503;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 1 ) ] = 220.146;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 1 ) ] = 525.573; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 2 ) ] = 16.5015;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 2 ) ] = 223.095;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 2 ) ] = 512.815; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 3 ) ] = 16.4887;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 3 ) ] = 223.978;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 3 ) ] = 517.274; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 4 ) ] = 16.4913;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 4 ) ] = 224.729;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 4 ) ] = 517.382; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 5 ) ] = 16.5109;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 5 ) ] = 226.198;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 5 ) ] = 510.704; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 6 ) ] = 16.5033;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 6 ) ] = 228.08;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 6 ) ] = 515.674; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 7 ) ] = 16.4821;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 7 ) ] = 228.893;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 7 ) ] = 522.873; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 8 ) ] = 16.4968;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 8 ) ] = 230.763;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 8 ) ] = 518.264; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 9 ) ] = 16.4909;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 9 ) ] = 231.542;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 9 ) ] = 519.386; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 10 ) ] = 16.4858;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 10 ) ] = 232.935;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 10 ) ] = 523.663; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 11 ) ] = 16.4919;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 11 ) ] = 234.622;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 11 ) ] = 521.715; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 12 ) ] = 16.5166;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 12 ) ] = 236.815;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 12 ) ] = 514.364; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 13 ) ] = 16.5263;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 13 ) ] = 238.12;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 13 ) ] = 511.306; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 14 ) ] = 16.5231;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 14 ) ] = 239.023;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 14 ) ] = 513.645; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 15 ) ] = 16.5483;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 15 ) ] = 241.451;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 15 ) ] = 506.986; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 16 ) ] = 16.5005;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 16 ) ] = 240.89;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 16 ) ] = 519.766; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 17 ) ] = 16.5219;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 17 ) ] = 242.571;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 17 ) ] = 512.147; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 18 ) ] = 16.5214;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 18 ) ] = 243.632;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 18 ) ] = 512.49; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 19 ) ] = 16.5038;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 19 ) ] = 245.103;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 19 ) ] = 519.445; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 20 ) ] = 16.5263;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 20 ) ] = 247.168;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 20 ) ] = 514.523; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 21 ) ] = 16.5183;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 21 ) ] = 247.702;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 21 ) ] = 515.323; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 22 ) ] = 16.5224;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 22 ) ] = 249.093;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 22 ) ] = 515.186; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 23 ) ] = 16.5089;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 23 ) ] = 250.295;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 23 ) ] = 520.456; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 24 ) ] = 16.5723;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 24 ) ] = 253.183;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 24 ) ] = 501.818; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 25 ) ] = 16.574;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 25 ) ] = 254.409;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 25 ) ] = 502.867; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 26 ) ] = 16.5308;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 26 ) ] = 254.282;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 26 ) ] = 517.584; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 27 ) ] = 16.5292;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 27 ) ] = 255.256;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 27 ) ] = 519.15; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 28 ) ] = 16.5474;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 28 ) ] = 257.706;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 28 ) ] = 514.771; 
  ToTCorrectionConstants[ PAR0 + resolveIndex( BarrelNeg, 2, 29 ) ] = 16.5473;
  ToTCorrectionConstants[ PAR1 + resolveIndex( BarrelNeg, 2, 29 ) ] = 257.483;
  ToTCorrectionConstants[ PAR2 + resolveIndex( BarrelNeg, 2, 29 ) ] = 514.611; 
  */

}
#endif
#endif
