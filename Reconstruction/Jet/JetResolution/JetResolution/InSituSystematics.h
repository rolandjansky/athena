/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Placeholder for the in-situ resolution systematics
// updated: 20-01-2012
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "TString.h"
#include "TMath.h"

using namespace std; 

class InsituSystematic
{ 
  public:
    inline void GetSRIsystematic(string jetCollection, double pt, double y, double& InSituSyst);

    // set number of bins
    static const int NPTB = 16;
    
  private:
};

inline void InsituSystematic::GetSRIsystematic(string jetCollection, double pt, double rapidity, double& InSituSyst)
{ 
  //set Pt bins for different jet collections
  double PTminEdge[NPTB] = {0.0};
  double PTmaxEdge[NPTB] = {0.0};  

  if(jetCollection == "AntiKt6TopoJES"){
    double dataPTminEdge[NPTB] ={40, 55, 70, 85, 100, 135, 180, 200, 240, 300, 380, 480, 600, 760,  1200.,1500};
    double dataPTmaxEdge[NPTB] ={55, 70, 85, 100, 135, 180, 200, 240, 300, 380, 480, 600,760, 1200,1500.,2000};
    for(int i=0;i<NPTB; i++) {PTminEdge[i] = dataPTminEdge[i]; PTmaxEdge[i] = dataPTmaxEdge[i];}
  }    
  else if(jetCollection == "AntiKt4TopoJES"){
    double dataPTminEdge[NPTB] ={30, 45, 60, 75, 90, 125, 160, 185, 240, 300, 380, 480, 600, 760, 1000, 1200.};
    double dataPTmaxEdge[NPTB] ={45, 60, 75, 90, 125, 160, 185, 240, 300, 380, 480, 600,760, 1000, 1200, 1500.};
    for(int i=0;i<NPTB; i++) {PTminEdge[i] = dataPTminEdge[i]; PTmaxEdge[i] = dataPTmaxEdge[i];}
  }
  // get correct bin
  int myPtbin = -1;
  for(int i_pt=0;i_pt<NPTB;i_pt++){
    if(PTminEdge[i_pt] <= pt && pt < PTmaxEdge[i_pt]){
      myPtbin = i_pt;
      break;
    }
  }
  //for Pt values outside range
  if(myPtbin == -1){
    if(pt <= PTminEdge[0]) myPtbin = 0;
    if(pt >= PTminEdge[NPTB-1]) myPtbin = NPTB-1;
  }
  
  //Systematic error values (%)
  rapidity = TMath::Abs(rapidity);
  if(jetCollection == "AntiKt6TopoiJES"){
    if(0 <= rapidity && rapidity < 0.8){
      float systInsituData[NPTB] = {14.6466,7.27574,5.76593,8.52178,3.22971,1.41657,8.15794,2.04324,2.39987,1.09485,2.27742,0.735287,1.14027,1.10746,0.0901085,0.0933185};
      InSituSyst = systInsituData[myPtbin]/100; 
    }
    else if(0.8 <= rapidity  && rapidity < 1.2){
      float systInsituData[NPTB] = {7.39735,3.34408,6.17725,2.45445,5.26137,1.7929,1.29223,1.61393,1.87901,3.24897,1.18797,0.733271,3.23954,2.81923,3,3};
      InSituSyst = systInsituData[myPtbin]/100; 
    }
    else if(1.2 <= rapidity  && rapidity < 2.1){
      float systInsituData[NPTB] = {18.3958,7.62861,4.48832,4.7839,1.96362,0.92448,2.67955,2.80633,2.01313,1.07053,0.887708,1.1773,1.3091,2.27417,2,2};
      InSituSyst = systInsituData[myPtbin]/100; 
    }
    else if(2.1 <= rapidity  && rapidity < 2.8){
      float systInsituData[NPTB] = {21.0962,11.8631,11.0961,13.6039,4.53162,5.72776,4.19736,5.33695,5,5,5,5,5,5,5,5};
      InSituSyst = systInsituData[myPtbin]/100; 
    }
    else if(2.8 <= rapidity  && rapidity < 3.6){
      InSituSyst = 0.01;
    }
    else if(3.6 <= rapidity  && rapidity < 4.5){
      InSituSyst = 0.03;
    }
    else {cout << "Rapidity region not implemented " << endl;}
  }
  ///////////////////////////////////////////////
  else if(jetCollection == "AntiKt4TopoJES"){
    if(0 <= rapidity  && rapidity < 0.8){
      float systInsituData[NPTB] = {24.869,5.10101,3.18466,8.54764,6.63753,4.08011,5.01364,4.96796,2.55286,3.08062,2.45873,1.54316,1.43749,2.65429,3,3};
      InSituSyst = systInsituData[myPtbin]/100; 
    }
    else if(0.8 <= rapidity  && rapidity < 1.2){
      float systInsituData[NPTB] = {559.627,2.24466,8.77602,5.80955,5.396,3.58351,1.87867,3.68311,3.85232,4.81409,1.80389,1.83954,3.61462,11.0273,2.56955,0.139192};
      InSituSyst = systInsituData[myPtbin]/100; 
    }
    else if(1.2 <= rapidity  && rapidity < 2.1){
      float systInsituData[NPTB] = {12.6316,10.5838,8.94002,12.8279,8.73815,8.64667,4.10653,4.04215,3.53947,1.49076,2.32493,2.71934,2.87719,2.92721,3,3};
      InSituSyst = systInsituData[myPtbin]/100; 
    }
    else if(2.1 <= rapidity  && rapidity < 2.8){
      float systInsituData[NPTB] = {12.9068,4.33309,9.99561,6.50565,5.68658,10.0806,7.92245,6.18596,7.71486,7,7,7,7,7,7,7};
      InSituSyst = systInsituData[myPtbin]/100; 
    }
    else if(2.8 <= rapidity  && rapidity < 3.6){
      InSituSyst = 0.01;
    }
    else if(3.6 <= rapidity  && rapidity < 4.5){
      InSituSyst = 0.02;
    }
    else {cout << "Rapidity region not implemented " << endl;}
  }//eof kt4
}
