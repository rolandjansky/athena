/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TSystem.h"
#include "TROOT.h"
#include <iostream>

#include "TLorentzVector.h"
#include "TMatrixD.h"

void init_epara_validation();

void init_epara_validation()
{
 
 cout<<"hello"<<endl;
 
 TLorentzVector *t;
 TMatrixD *m;
 
 gInterpreter->AddIncludePath("..");
 gInterpreter->AddIncludePath("../../ISF_FastCaloSimEvent");

 gROOT->LoadMacro("../Root/TreeReader.cxx+");
 
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCS1DFunction.cxx+");
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCS1DFunctionRegression.cxx+");
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCS1DFunctionRegressionTF.cxx+");
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCS1DFunctionHistogram.cxx+");
 
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/IntArray.cxx+");
 
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCSExtrapolationState.cxx+");
 
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCSTruthState.cxx+");
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCSSimulationState.cxx+");
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCSParametrizationBase.cxx+");
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCSParametrization.cxx+");
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCSEnergyParametrization.cxx+");
 gROOT->LoadMacro("../../ISF_FastCaloSimEvent/src/TFCSPCAEnergyParametrization.cxx+");
 gROOT->LoadMacro("../Root/EnergyParametrizationValidation.cxx+");
 
 cout<<"init done"<<endl;
 
}
