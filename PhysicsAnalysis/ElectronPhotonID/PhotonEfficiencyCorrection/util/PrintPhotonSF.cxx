//PrintPhotonSF.cxx  - print SF for a given input
//michael.pitt@cern.ch

// System include(s):
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <boost/filesystem.hpp>

// ROOT include(s):
#include "TFile.h"
#include "TString.h"
#include "TList.h"
#include "TKey.h"

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
	// Check which directories (run numbers) exists
	//Let user to deside which one to use in case of multiple
	TList * listDirectories = TFile::Open(file)->GetListOfKeys();
	const int ndirs = listDirectories->GetSize();
	if(0==ndirs){
	  printf("Error: file %s does not contains any keys\n",argv[1]);
	  return 0;
	}	
	TString dirName = listDirectories->First()->GetName();
	if(ndirs>1) {
	  printf("Reads: %s, found %d folders:\n",argv[1],ndirs);
	  TIter next(listDirectories);
	  TKey *key;
	  while ((key = (TKey*)next())) printf("%s\n",key->GetName());
	  printf("which directory to use? (type the number from 1 to %d): ",ndirs);
	  int ndir_input=0; cin >> ndir_input;
	  if(ndir_input>ndirs || ndir_input<1)
	    printf("\nWarning... entered wrong key number, will print SF for directory: %s\n",dirName.Data());
	  else{
	    dirName = listDirectories->At(ndir_input-1)->GetName();
	    printf("\nprint SF for directory: %s\n",dirName.Data());
	  }
	}
	else printf("Reads: %s\nDirectory: %s\n",argv[1],dirName.Data());
	
	// read first run number from the directory name:
	int run_number = atoi(dirName.Tokenize("_")->First()->GetName());
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
TH2F * h = file.Contains("AFII") ? (TH2F*)TFile::Open(file)->Get(Form("%s/AtlFast2_sf",dirName.Data())) : (TH2F*)TFile::Open(file)->Get(Form("%s/FullSim_sf",dirName.Data()));


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
printf("%2.2f+/-%2.4f\t|",tool_SF.calculate(datatype,run_number,eta,pt).getScaleFactor(),tool_SF.calculate(datatype,run_number,eta,pt).getTotalUncertainty());
} cout << endl;
}
cout << dash_line.Data() <<endl;

} // END PROGRAM

