/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TMatrixF.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"
#include "TVectorF.h"
#include "TH1D.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TTree.h"
#include "TSystem.h"
#include "TH2D.h"
#include "TPrincipal.h"
#include "TMath.h"
#include "TBrowser.h"
#include "ISF_FastCaloSimParametrization/FitDetailedShape.h"
#include "ISF_FastCaloSimParametrization/TFCS2Function.h"
#include "ISF_FastCaloSimParametrization/TFCS2DFunction.h"

#include <iostream>
#include <sstream>

using namespace std;

FitDetailedShape::FitDetailedShape()
{
 
}

void FitDetailedShape::run(string inputfile, int /*verbose_level*/, string /*thisname*/, string /*labelout*/, int /*mycase*/, float /*calosize*/, bool /*doNNHit*/)
{
 
 //Open inputfile:
 cout<<endl;
 cout<<"****************"<<endl;
 cout<<" Fit detailed shape "<<endl;
 cout<<"****************"<<endl;
 cout<<endl;
 cout<<"--- Open input file "<<inputfile<<endl;
 TFile* file=TFile::Open(inputfile.c_str());
 if(!file) cout<<"ERROR: Inputfile could not be opened"<<endl;
  
 //TFCS2DFunction* fct=TFCS2Function::Create(inputfile,verbose_level,thisname,labelout, mycase, calosize,doNNHit);
 
}
