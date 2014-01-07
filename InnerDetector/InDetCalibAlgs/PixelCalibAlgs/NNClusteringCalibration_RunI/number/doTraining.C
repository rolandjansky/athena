/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void doTraining(bool useTrackEstimate=false,
                int nPatternsPerUpdate=200,
                double learningRate=0.3,
                double learningRateDecrease=0.99,
                double learningRateMomentum=0.1)
{

//  TString nameFile="root://castoratlas//castor/cern.ch/user/g/giacinto/PixelClustering/calibNtupleFebruary/outputNN_new.root";
  TString nameFile="root://castoratlas//castor/cern.ch/user/l/lorenzi/TrkValidation17.0.0.5_J5_T1.root";

//TString nameFile="root://castoratlas//castor/cern.ch/user/l/lorenzi/TrkValidation_4.root";



//  TString nameFile ="../../TrkValidation.root";
  gSystem->Load("libTJetNet.so");
  gSystem->Load("trainNN_cxx.so");



  //gROOT->ProcessLine(".L trainNN.cxx");
  
  TString path(nameFile);
  
  TString classname("clusterMultiplicityNN");
  
  trainNN(path,classname,
          10000,//trainingEpoches
          2,//dilution
          25,//nHidden1
          20,//nHidden2
          0,
          useTrackEstimate,
          nPatternsPerUpdate,
          learningRate,
          learningRateDecrease,
          learningRateMomentum);
  
  gROOT->ProcessLine(".q");

}
