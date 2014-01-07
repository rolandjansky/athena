/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void doValidation(bool useTrackEstimate )
{

  gSystem->Load("../libTJetNet.so");
  gROOT->ProcessLine(".L validateNNnum.cxx++g");

  gSystem->Load("validateNNnum_cxx.so");

 validateNN(useTrackEstimate); 
  
  
  
  //gROOT->ProcessLine(".q");

}
