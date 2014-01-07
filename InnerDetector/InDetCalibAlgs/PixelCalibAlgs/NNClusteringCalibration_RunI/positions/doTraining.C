/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void doTraining(int nParticles,
                bool useTrackEstimate=false,
                int nPatternsPerUpdate=200,
                double learningRate=0.3,
                double learningRateDecrease=0.99,
                double learningRateMomentum=0.1)
{

//  TString nameFile="root://castoratlas//castor/cern.ch/user/g/giacinto/PixelClustering/calibNtupleFebruary/outputNN_new.root";
//  TString nameFile ="../../TrkValidation.root";
  //TString nameFile="root://castoratlas//castor/cern.ch/user/l/lorenzi/TrkValidation.root";

  TString nameFile="root://castoratlas//castor/cern.ch/user/l/lorenzi/TrkValidation17.0.0.5_J5_T1.root";
//  TString nameFile="/tmp/giacinto/run/TrkValidation.root";
///tmp/giacinto/user.lorenzi.MC10J6.trainingNN.lorentz.110616091547/user.lorenzi.MC06J6.trainingNN.110615121136/user.lorenzi.000130.TRKVAL._00095.root";

///tmp/giacinto/user.lorenzi.MC10J6.trainingNN.lorentz.110616091547/user.lorenzi.000136.TRKVAL._00002.root";
  
  gSystem->Load("libTJetNet.so");
  gSystem->Load("trainNN_cxx.so");

  
  TString path(nameFile);
  
  TString classname("clusterTwoPositionsNN");
  
  int nHidden1=40;
  int nHidden2=20;
  if (nParticles>=3)
  {
    nHidden1=30;
    nHidden2=10;
  }

  int dilution=2;
  if (nParticles==1)
  {
    dilution=40;
  }
  trainNN(path,classname,
          10000,//trainingEpoches
          dilution,//dilution
          nHidden1,//nHidden1
          nHidden2,//nHidden2
          0,//restartTraiingFrom
          nParticles,//nParticles
          useTrackEstimate,
          nPatternsPerUpdate,
          learningRate,
          learningRateDecrease,
          learningRateMomentum);//useTrackEstimate
  
  gROOT->ProcessLine(".q");

}
