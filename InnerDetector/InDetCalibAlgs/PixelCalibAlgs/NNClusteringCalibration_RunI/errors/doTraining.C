/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void doTraining(int nParticles,
                bool planeX,
                int nBins,
                bool useTrackEstimate=false,
                int nPatternsPerUpdate=10,
                double learningRate=0.1,
                double learningRateDecrease=0.999,
                double learningRateMomentum=0.05)
{
  


//  TString nameFile="root://castoratlas//castor/cern.ch/user/g/giacinto/PixelClustering/calibNtupleFebruary/outputNN_withOutput_withoutTracks.root";

//  if (useTrackEstimate)
//  {
//  nameFile="root://castoratlas//castor/cern.ch/user/g/giacinto/PixelClustering/calibNtupleFebruary/outputNN_withOutput_withTracks.root";
//  }

//  TString nameFile="root://castoratlas//castor/cern.ch/user/l/lorenzi/TrkValidation.root";

TString nameFile="root://castoratlas//castor/cern.ch/user/l/lorenzi/TrkValidation_4.root";

// TString nameFile="../../TrkValidation.root";


  gSystem->Load("libTJetNet.so");
  gSystem->Load("trainNN_cxx.so");

  TString path(nameFile);
  
  TString classname("clusterTwoPositionsErrorNN");
  
  int nHidden1=15;
  int nHidden2=10;
  if (nParticles>=3)
  {
    nHidden1=10;
    nHidden2=7;
  }

  int dilution=2;
  if (nParticles==1)
  {
    dilution=20;
  }

  trainNN(path,classname,
          10000,//trainingEpoches
          dilution,//dilution
          nHidden1,
          nHidden2,
          0,//restartTrainingFrom
          nParticles,//nParticles
          useTrackEstimate,
          nBins,
          planeX,
          nPatternsPerUpdate,
          learningRate,
          learningRateDecrease,
          learningRateMomentum);
  
  gROOT->ProcessLine(".q");

}
