/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void doTraining(TString nameCollection,bool withIP3D) {

  gSystem->Load("./libTJetNet.so");
  gSystem->Load("trainNN_cxx.so");

  
  TString path("../../../reduceddatasets/reduceddataset_");
  path+=nameCollection;
  path+="_forNN.root";
  
  TString classname("JetFitterNN_");
  classname+=nameCollection;
  
  if (withIP3D)
  {
    trainNN(path,classname,
            10000,//trainingEpoches
            2,//dilution
            false,//dummy
            true,//withIP3D
            14,//nHidden1
            9);//nHidden2
  }
  else
  {
    trainNN(path,classname,
            10000,//trainingEpoches
            2,//dilution
            false,//dummy
            false,//withIP3D
            12,//nHidden1
            7);//nHidden2
  }
  
  gROOT->ProcessLine(".q");

}
