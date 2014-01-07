/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TString.h"
#include <vector>
using namespace std;

//#ifdef __MAKECINT__
//#pragma link C++ class vector<vector<float> > ;  
//#pragma link C++ class vector<float> ;  
//#endif


void trainNN(TString inputfile,
             TString outputclass="JetFitterNN",
             int nIterations=10,
             int dilutionFactor=10,
             int nodesFirstLayer=10,
             int nodesSecondLayer=9,
             int restartTrainingFrom=0,
             bool useTrackEstimate=false,
             int nPatternsPerUpdate=200,
             double learningRate=0.3,
             double learningRateDecrease=0.99,
             double learningRateMomentum=0.1);

int main();
