/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidTool_HTcalculation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifdef INDETTRT_ELECTRONPIDTOOL_H
#ifndef TRT_ELECTRONPID_HTCALCULATION_CXX
#define TRT_ELECTRONPID_HTCALCULATION_CXX

/*****************************************************************************\

  This file contains the implementation of the class HTcalculator.

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

InDet::TRT_ElectronPidTool::HTcalculator::HTcalculator(AthAlgTool & parent):
  BaseTRTPIDCalculator(parent,(SIZE_OF_HEADER +
                         SIZE_PAR_SET * 3 +
                         SIZE_STRAW_CONSTANTS ),"HT"),
  StrawConstants( (float*)( Blob + OFF_STRAW_CONSTANTS)),

  ParsBarrel( (float*)( Blob + OFF_PAR_SET_BARREL)),
  ParsEndcapA( (float*)( Blob + OFF_PAR_SET_ENDCAP_A)),
  ParsEndcapB( (float*)( Blob + OFF_PAR_SET_ENDCAP_B))
{
  CurrentVersion = my_CurrentVersion;
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Default Destructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidTool::HTcalculator::~HTcalculator(){
  //Nothing here yet
}

/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Get The Pobability of this hit being a Hight THreshold hit  %%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

double InDet::TRT_ElectronPidTool::HTcalculator::getProbHT(const double pTrk, Trk::ParticleHypothesis hypothesis, int HitPart, int Layer, int Strawlayer){
  checkInitialization();
   
  double eProbHT = 0.5;

  // Make sure that the information passed makes sense:
  if (((pTrk < 100.0) || (pTrk > 5000000.0))) 
    //outside the range where we can make a decision
    return eProbHT;

  double mass = m_particlemasses.mass[hypothesis];

  int part_i;
  if(abs(HitPart)==1) // the detector part as in the database
    part_i=0;//Barrel
  else if(Layer < 6) //EndcapA
    part_i=1;
  else
    part_i=2;   //EndcapB

  const float *parameters[3]={ParsBarrel,ParsEndcapA,ParsEndcapB};

  const float *par = parameters[part_i];

  // take the log10 of beta*gamma, this is our x-value
  double x = log10( pTrk / mass );

  // Calcualte the probablility based on a logistic sigmoid with constants from the database
  // for retreival of database constants m_gamma_TR_size etc. see the update() function
  double prob_TR    = par[TR_SIZE] / (1.0 + exp( - ( x - par[TR_MEAN] ) / par[TR_WIDTH] ) );
  double prob_dEdx  = par[DEDX_C] + x * par[DEDX_M];

  //and apply a correction based on the straw layer
  const float * corr = StrawConstants + resolveIndex(HitPart,Layer,Strawlayer);


  eProbHT = prob_dEdx * corr[DEDX_CORR] + prob_TR * corr[TR_CORR];// * probfactor_depth * probfactor_angle * probfactor_distwire * probfactor_phi;

  //ATH_MSG_INFO("Determined Eprob from HT to be "<<eProbHT<<" in part "<<part_i);

  return eProbHT;
}



/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  turn the BEC, strawlayer and layer information into a linear index  %%*|
|*%%%  that can be used on a flat array.  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

int InDet::TRT_ElectronPidTool::HTcalculator::resolveIndex( int BEC,  int Layer, int Strawlayer){

  int part=abs(BEC)-1;

  
  // Do a consistency check:
  static const int strawsPerBEC[2][14]={{19,24,30, 0, 0, 0,0,0,0,0,0,0,0,0},
                                        {16,16,16,16,16,16,8,8,8,8,8,8,8,8}};

  //if(Strawlayer >= strawsPerBEC[part][Layer])
  //  std::cout<<"TRT_ElectronPidTool   ERROR TRT part "<<BEC<<" Layer "<<Layer<<" only has "<<strawsPerBEC[part][Layer]<<" straws. Found index "<<Strawlayer<<std::endl;

  // the following array will contain the number that has to be added for each Layer. It is derived as the integral of the array above.
  // the initialization looks unnecessarily complicated but these computations are carried out at compile time, so no worries ;)
  static const int strawsBeforeLayer[2][14]={{0, strawsPerBEC[0][0]+strawsBeforeLayer[0][0], strawsPerBEC[0][1]+strawsBeforeLayer[0][1],
                                              strawsPerBEC[0][2]+strawsBeforeLayer[0][2], 0,  0,  0,   0,   0,   0,   0,   0,   0,   0},
                                             {strawsBeforeLayer[0][3], //last from Barrel
                                              strawsPerBEC[1][0]+strawsBeforeLayer[1][0],
                                              strawsPerBEC[1][1]+strawsBeforeLayer[1][1], strawsPerBEC[1][2]+strawsBeforeLayer[1][2],
                                              strawsPerBEC[1][3]+strawsBeforeLayer[1][3], strawsPerBEC[1][4]+strawsBeforeLayer[1][4],
                                              strawsPerBEC[1][5]+strawsBeforeLayer[1][5], strawsPerBEC[1][6]+strawsBeforeLayer[1][6],
                                              strawsPerBEC[1][7]+strawsBeforeLayer[1][7], strawsPerBEC[1][8]+strawsBeforeLayer[1][8],
                                              strawsPerBEC[1][9]+strawsBeforeLayer[1][9], strawsPerBEC[1][10]+strawsBeforeLayer[1][10],
                                              strawsPerBEC[1][11]+strawsBeforeLayer[1][11], strawsPerBEC[1][12]+strawsBeforeLayer[1][12]}};
  
  // std::cout<<"TRT_ElectronPidTool::HTcalculator      INFO now returning index "<<Strawlayer + strawsBeforeLayer[part][Layer]<<" Part: "<<part<<" Layer: "<<Layer<<" SL: " <<Strawlayer<<std::endl;
  int glob_straw_index = Strawlayer + strawsBeforeLayer[part][Layer];
  int offset = glob_straw_index * N_CONSTS_PER_STRAW;
  if(not (offset < N_STRAW_CONSTS))
    return 0;
  return offset;
}


/*****************************************************************************\
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%  Hard-coded HT Calibration Constants  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
|*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

void InDet::TRT_ElectronPidTool::HTcalculator::setDefaultCalibrationConstants(){
  /*****************************************************************************\

    This code is never called in production. It is used to set all
    constants in the positions in the HTBlob where they are needed, and 
    finally print out the blob as an array of numbers. This is far easier and 
    less error prone than having a separate setter-script which might itself 
    have a version mismatch with this code.

    PLEASE REMEMBER to increment the version number precisely when you change 
    the addresses of the various arrays inside the HTBlob, and NEVER otherwise!
    
  \*****************************************************************************/
  HasBeenInitialized=1;

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

  UpperLimit=1.0;
  LowerLimit=0.0;

  /*
  //For MC:
  ParsBarrel[DEDX_C]=4.12398e-02;
  ParsBarrel[DEDX_M]=2.92087e-03;
  ParsBarrel[TR_SIZE]=1.47385e-01;
  ParsBarrel[TR_MEAN]=3.31603e+00;
  ParsBarrel[TR_WIDTH]=1.84435e-01;

  ParsEndcapA[DEDX_C]=3.84573e-02;
  ParsEndcapA[DEDX_M]=6.13286e-03;
  ParsEndcapA[TR_SIZE]=1.03985e-01;
  ParsEndcapA[TR_MEAN]=3.25720e+00;
  ParsEndcapA[TR_WIDTH]=1.15939e-01;

  ParsEndcapB[DEDX_C]=2.81769e-02;
  ParsEndcapB[DEDX_M]=1.10635e-02;
  ParsEndcapB[TR_SIZE]=1.74251e-01;
  ParsEndcapB[TR_MEAN]=3.33191e+00;
  ParsEndcapB[TR_WIDTH]=1.55063e-01;
  */
  
  //For data we found the following:

  ParsBarrel[DEDX_C]=3.65026e-2;
  ParsBarrel[DEDX_M]=5.45505e-3;
  ParsBarrel[TR_SIZE]=1.55901e-1;
  ParsBarrel[TR_MEAN]=3.28898;
  ParsBarrel[TR_WIDTH]=2.06788e-1;

  ParsEndcapA[DEDX_C]=3.38951e-2;
  ParsEndcapA[DEDX_M]=6.40528e-3;
  ParsEndcapA[TR_SIZE]=1.41382e-1;
  ParsEndcapA[TR_MEAN]=3.28648;
  ParsEndcapA[TR_WIDTH]=1.57271e-1;

  ParsEndcapB[DEDX_C]=3.64409e-02;
  ParsEndcapB[DEDX_M]=2.83820e-03;
  ParsEndcapB[TR_SIZE]=2.28494e-01;
  ParsEndcapB[TR_MEAN]=3.33994e+00;
  ParsEndcapB[TR_WIDTH]=2.43090e-01;
  

  StrawConstants[ 0 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 0 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 1 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 1 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 2 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 2 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 3 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 3 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 4 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 4 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 5 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 5 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 6 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 6 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 7 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 7 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 8 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 8 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 9 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 9 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 10 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 10 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 11 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 11 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 12 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 12 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 13 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 13 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 14 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 14 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 15 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 15 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 16 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 16 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 17 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 17 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 18 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 18 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 19 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 19 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 20 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 20 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 21 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 21 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 22 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 22 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 23 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 23 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 24 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 24 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 25 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 25 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 26 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 26 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 27 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 27 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 28 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 28 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 29 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 29 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 30 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 30 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 31 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 31 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 32 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 32 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 33 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 33 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 34 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 34 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 35 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 35 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 36 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 36 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 37 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 37 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 38 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 38 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 39 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 39 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 40 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 40 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 41 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 41 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 42 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 42 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 43 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 43 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 44 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 44 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 45 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 45 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 46 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 46 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 47 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 47 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 48 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 48 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 49 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 49 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 50 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 50 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 51 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 51 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 52 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 52 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 53 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 53 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 54 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 54 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 55 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 55 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 56 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 56 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 57 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 57 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 58 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 58 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 59 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 59 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 60 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 60 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 61 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 61 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 62 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 62 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 63 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 63 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 64 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 64 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 65 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 65 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 66 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 66 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 67 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 67 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 68 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 68 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 69 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 69 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 70 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 70 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 71 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 71 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 72 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 72 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 73 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 73 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 74 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 74 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 75 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 75 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 76 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 76 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 77 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 77 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 78 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 78 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 79 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 79 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 80 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 80 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 81 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 81 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 82 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 82 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 83 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 83 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 84 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 84 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 85 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 85 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 86 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 86 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 87 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 87 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 88 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 88 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 89 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 89 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 90 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 90 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 91 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 91 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 92 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 92 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 93 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 93 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 94 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 94 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 95 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 95 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 96 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 96 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 97 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 97 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 98 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 98 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 99 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 99 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 100 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 100 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 101 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 101 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 102 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 102 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 103 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 103 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 104 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 104 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 105 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 105 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 106 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 106 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 107 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 107 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 108 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 108 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 109 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 109 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 110 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 110 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 111 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 111 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 112 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 112 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 113 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 113 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 114 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 114 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 115 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 115 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 116 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 116 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 117 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 117 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 118 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 118 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 119 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 119 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 120 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 120 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 121 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 121 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 122 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 122 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 123 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 123 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 124 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 124 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 125 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 125 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 126 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 126 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 127 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 127 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 128 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 128 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 129 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 129 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 130 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 130 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 131 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 131 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 132 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 132 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 133 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 133 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 134 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 134 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 135 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 135 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 136 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 136 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 137 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 137 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 138 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 138 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 139 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 139 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 140 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 140 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 141 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 141 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 142 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 142 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 143 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 143 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 144 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 144 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 145 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 145 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 146 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 146 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 147 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 147 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 148 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 148 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 149 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 149 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 150 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 150 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 151 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 151 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 152 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 152 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 153 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 153 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 154 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 154 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 155 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 155 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 156 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 156 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 157 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 157 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 158 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 158 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 159 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 159 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 160 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 160 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 161 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 161 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 162 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 162 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 163 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 163 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 164 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 164 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 165 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 165 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 166 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 166 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 167 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 167 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 168 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 168 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 169 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 169 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 170 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 170 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 171 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 171 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 172 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 172 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 173 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 173 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 174 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 174 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 175 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 175 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 176 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 176 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 177 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 177 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 178 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 178 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 179 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 179 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 180 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 180 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 181 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 181 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 182 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 182 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 183 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 183 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 184 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 184 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 185 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 185 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 186 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 186 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 187 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 187 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 188 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 188 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 189 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 189 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 190 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 190 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 191 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 191 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 192 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 192 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 193 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 193 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 194 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 194 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 195 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 195 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 196 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 196 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 197 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 197 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 198 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 198 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 199 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 199 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 200 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 200 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 201 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 201 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 202 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 202 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 203 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 203 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 204 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 204 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 205 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 205 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 206 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 206 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 207 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 207 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 208 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 208 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 209 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 209 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 210 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 210 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 211 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 211 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 212 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 212 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 213 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 213 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 214 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 214 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 215 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 215 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 216 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 216 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 217 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 217 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 218 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 218 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 219 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 219 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 220 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 220 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 221 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 221 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 222 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 222 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 223 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 223 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 224 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 224 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 225 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 225 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 226 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 226 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 227 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 227 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 228 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 228 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 229 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 229 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 230 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 230 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 231 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 231 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;
  StrawConstants[ 232 * N_CONSTS_PER_STRAW + DEDX_CORR] = 1.;    StrawConstants[ 232 * N_CONSTS_PER_STRAW + TR_CORR] = 1.;

}

#endif
#endif
