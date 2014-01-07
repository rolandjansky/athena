/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TString.h"

void trainNN(TString inputfile,
             TString outputclass="JetFitterNN",
             int nIterations=10,
             int dilutionFactor=2,
             int nodesFirstLayer=10,
             int nodesSecondLayer=9,
             int restartTrainingFrom=0,
             int nParticlesTraining=2,
             bool useTrackEstimate=false,
             int numberBinsErrorEstimate=20,
             bool trainXerrors=true,
             int nPatternsPerUpdate=10,
             double learningRate=0.1,
             double learningRateDecrease=0.999,
             double learningRateMomentum=0.05);

int main();
