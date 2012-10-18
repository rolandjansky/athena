/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Work/JESUncertaintyR17/JetUncertainties/StandAlone/
//g++ -o ProduceJESUncPlots.exe ProduceJESUncPlots.C `$ROOTSYS/bin/root-config --cflags --glibs` -I../JetUncertainties -L../StandAlone -lJetUncertainties

//#include "JES_Smoother.C"
#include "JetUncertainties/JESUncertaintyProvider.h"
#include "TFile.h"
#include "TH1D.h"
#include "TSystem.h"
#include "TGraphErrors.h"
#include "TError.h"
#include <vector>

using namespace std;

int main () {

  gErrorIgnoreLevel=2000; 
  gSystem->Load("../StandAlone/libJetUncertainties.so");
  
  TString JetSizes[2] = {"4", "6"};
  TString JetCalibs[2] = {"EMJES", "LCJES"};
  TString JetCalibNames[2] = {"EM", "LC"};
  TString MCNames[4] = {"MC11b", "MC11c", "FrozenShowers","AFII" };
  //TString MCNames[1] = {"AFII" };
  
  for (unsigned int iMC = 0 ; iMC < 4; iMC++) {
    for (unsigned int iJetSize = 0; iJetSize < 2; iJetSize++)  { 
      for (unsigned int iJetCalib = 0; iJetCalib < 2; iJetCalib++)  { 
	
        JESUncertaintyProvider *jj = new JESUncertaintyProvider("JES_2011/Final/InsituJES2011_AllNuisanceParameters.config","AntiKt"+JetSizes[iJetSize]+"Topo"+JetCalibNames[iJetCalib], MCNames[iMC], "../share/");
	JESUncertaintyProvider &j = *jj;
	j.useGeV();
	TString calibType= JetCalibs[iJetCalib];
	TString jetSize = JetSizes[iJetSize];
	
	TFile * fout = TFile::Open("JESPlots_AntiKt"+jetSize+"_"+calibType+"_"+MCNames[iMC]+".root","RECREATE");

	double etaBins [] = {0.1, 0.5, 1.0, 1.7, 2.45, 3.0, 3.4, 4.0};

	//log binning
	int N = 100;
	double min = 15, max = 2500;
	
	std::vector<double> vec; 
	double dx=(log(max)-log(min))/N;
	for (int i=0;i<=N;++i) {
	  vec.push_back(exp(log(min)+i*dx));
	}
	
	//ahem...std::vector is contiguous though
	const double * ptBins = &vec[0];
	
	int Ncomp=j.getNUncertaintyComponents();
	
	TString etaBinsTitles [] = {"0.1", "0.5", "1.0", "1.7", "2.5", "3.0", "3.4", "4.0"};
	for (int icomp=0;icomp<Ncomp;++icomp) {
	  TString compName = j.getComponentNames().at(icomp);
	  
	  TString compDesc = j.getComponentDescriptions().at(icomp);
	  
	  for (unsigned int iEtaBin = 0; iEtaBin < 8; iEtaBin++) {
	    double eta=etaBins[iEtaBin];
	    
	    TGraphErrors * gUnc = new TGraphErrors();   
	    gUnc->SetName("uncertainty_"+compName+"_"+etaBinsTitles[iEtaBin]);
	    gUnc->SetTitle(compDesc+", "+"#eta= "+etaBinsTitles[iEtaBin]);
	    for (unsigned int iPtBin=0; iPtBin<N; iPtBin++) {
	      double unc = j.getRelUncertComponent(compName,ptBins[iPtBin], eta);
	      gUnc->SetPoint(iPtBin, ptBins[iPtBin], unc);
	    }
	    fout->cd();
	    gUnc->Write();
	  }
	}
	
	for (unsigned int iEtaBin = 0; iEtaBin < 8; iEtaBin++) {
	  double eta=etaBins[iEtaBin];
	  
	  TGraphErrors * gTotUnc = new TGraphErrors();   
	  gTotUnc->SetName("TotalUncertainty_"+etaBinsTitles[iEtaBin]);
	  gTotUnc->SetTitle("Total JES uncertainty, #eta= "+etaBinsTitles[iEtaBin]);
	  for (unsigned int iPtBin=0; iPtBin<N; iPtBin++) {
	    double unc = j.getRelUncert(ptBins[iPtBin], eta);
	    gTotUnc->SetPoint(iPtBin, ptBins[iPtBin], unc);
	  }
	  fout->cd();
	  gTotUnc->Write();
	}
	//printf("at eta=4.0 : %.5f\n",j.getRelUncertComponent("RelativeNonClosure_AFII", 4.0, 8, 8));

	printf("Wrote to %s\n",fout->GetName());
	fout->Close();

      }
      
    }
  }
}
