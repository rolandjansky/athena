/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//PrintPhotonSF.cxx  - print SF for a given input

// System include(s):
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <boost/filesystem.hpp>

// ROOT include(s):
#include "TFile.h"
#include "TString.h"

// Local include(s):
#include "PhotonEfficiencyCorrection/TPhotonEfficiencyCorrectionTool.h"
#define GEV 1000.0
#define TEV 1000000.0

using namespace std;
using namespace TMath;

int main (int argc, const char * argv[]) {

	if(argc!=2){
		printf("input parameters:\n./PrintPhotonSF [file] \n");
		printf("example:\n PrintPhotonSF $ROOTCOREBIN/data/PhotonEfficiencyCorrection/INPUTFILE.root\n");
		return 0;
	}	
	
	// Check the input file:
	if(!boost::filesystem::exists(argv[1])){
	  printf("Error: file %s not exists\n",argv[1]);
	  return 0;
	}

	TString file(argv[1]);	
	if(!TFile::Open(file)->GetListOfKeys()->Contains("0_99999999999")){
	  printf("Error: no TDirectory 0_99999999999, check your input file\n");
	  return 0;
	}
	
	if(getenv("ROOTCOREDIR")==NULL){
        cout << "Please setup RootCore before running the PrintPhotonSF [file]"<<endl;
        return 0.;
        }

	// Create and initialize an instance for both types of photons
	Root::TPhotonEfficiencyCorrectionTool tool_SF;
	tool_SF.addFileName(file.Data());

if(!tool_SF.initialize()){
  printf("Tool not initialized properly, check the error massages\n");
  return 0;
}

PATCore::ParticleDataType::DataType datatype=PATCore::ParticleDataType::Full;
if(file.Contains("AFII")) datatype=PATCore::ParticleDataType::Fast;

// Access the file to get the histogram binning:
TH2F * h = file.Contains("AFII") ? (TH2F*)TFile::Open(file)->Get("0_99999999999/AltFast2_sf") : (TH2F*)TFile::Open(file)->Get("0_99999999999/FullSim_sf");


const Double_t * pTbounds = h->GetXaxis()->GetXbins()->GetArray();
const Double_t * Etabounds = h->GetYaxis()->GetXbins()->GetArray();

const int npTbins  = h->GetXaxis()->GetNbins();
const int nEtabins = h->GetYaxis()->GetNbins();

double pt, eta;

// loop over bins, in different pt/eta region, and print the SF
cout << "-----------------------------------------------------------------------------------"<<endl;
cout << "Table of photon ScaleFactors obtained by data-driven measurements for input file:"<<endl; cout << file <<endl;
cout << "-----------------------------------------------------------------------------------"<<endl;
TString dash_line="---------------"; for(int i=1;i<=nEtabins;i++) dash_line+="------------------";
cout << "|  pt[GeV]\t\t|"; for(int i=1;i<=nEtabins;i++) printf("%2.2f<eta<%2.2f\t|",Etabounds[i-1],Etabounds[i]); cout<<endl;
cout << dash_line.Data() <<endl;
for (int i=1;i<=npTbins;i++){
pt=0.5*(pTbounds[i-1]+pTbounds[i]);
if(pt/GEV<100) printf("|%2.1f-%2.1f\t\t|",pTbounds[i-1]/GEV,pTbounds[i]/GEV);
else if(pt<TEV) printf("|%2.0f - %2.0f\t\t|",pTbounds[i-1]/GEV,pTbounds[i]/GEV);
else printf("|%2.0f-%2.0f\t\t|",pTbounds[i-1]/GEV,pTbounds[i]/GEV);
for(int j=1;j<=nEtabins;j++){
eta=0.5*(Etabounds[j-1]+Etabounds[j]);
printf("%2.2f+/-%2.4f\t|",tool_SF.calculate(datatype,1,eta,pt).getScaleFactor(),tool_SF.calculate(datatype,1,eta,pt).getTotalUncertainty());
} cout << endl;
}
cout << dash_line.Data() <<endl;

} // END PROGRAM

