/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TSystem.h"
#include "TROOT.h"
#include <iostream>

#include "../ISF_FastCaloSimParametrization/MeanAndRMS.h"
#include "Identifier/Identifier.h"
#include "CaloDetDescr/CaloDetDescrElement.h"


void init_geo();

void init_geo()
{
 
 cout<<"init geometry test tool"<<endl;

 gInterpreter->AddIncludePath("..");
 gInterpreter->AddIncludePath("../../ISF_FastCaloSimEvent");

 gROOT->LoadMacro("CaloSampling.cxx+");
 gROOT->LoadMacro("../src/CaloGeometry.cxx+");
 gROOT->LoadMacro("CaloGeometryFromFile.cxx+");
 
 cout<<"init geometry done"<<endl;
 
}
