/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  if(1==0) {
    // Needs athena environment setup and ISF_FastCaloSimParametrization package compiled.
    // Uses the root interface library created in the compilation of ISF_FastCaloSimParametrization
    gSystem->Load("libISF_FastCaloSimParametrizationLib.so");
  } else {  
    gSystem->AddIncludePath(" -I.. ");

    gROOT->LoadMacro("../ISF_FastCaloSimParametrization/MeanAndRMS.h+");
    gROOT->LoadMacro("../Root/TFCSExtrapolationState.cxx+");
    gROOT->LoadMacro("../Root/TFCSTruthState.cxx+");
    gROOT->LoadMacro("../Root/TFCSSimulationState.cxx+");
    gROOT->LoadMacro("../Root/TFCSParametrizationBase.cxx+");
    gROOT->LoadMacro("../Root/TFCSParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSEnergyParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSPCAEnergyParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSLateralShapeParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSNNLateralShapeParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSSimpleLateralShapeParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCS1DFunction.cxx+");
    gROOT->LoadMacro("../Root/TFCS1DFunctionRegression.cxx+");
    gROOT->LoadMacro("../Root/TFCS1DFunctionHistogram.cxx+");
    gROOT->LoadMacro("../Root/TFCSFunction.cxx+");
  }  
  
  TH1D* hist=new TH1D("hist","hist",10,0,1);
  TFCS1DFunction* fct=TFCSFunction::Create(hist);
  cout<<fct<<endl;
}

