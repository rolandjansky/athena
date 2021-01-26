/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>
#include <iostream>
#include <TPad.h>
//#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
#include <math.h>
#include "../TJetNet.h"
#include "../doNormalization.C"
#include "Riostream.h"
#include "../TNetworkToHistoTool.h"
#include <TSystem.h>

#include "../TTrainedNetwork.h"

#include <sstream>
#include "TMatrixD.h"
#include "TVectorD.h"
//#include "trainNN.h"
#include <algorithm>
#include <vector>
#include <utility>
#include <TLatex.h>
#include <TChain.h>
#include <TGraphErrors.h>

using namespace std;

Double_t sigmoid(Double_t x)
{
  return 1./(1.+exp(-2*x));
}

using namespace std;


bool isBadCluster( int sizeX, int nParticles ){


  if(sizeX==-100){return true;}
  if(nParticles==0){return true;}     
  if(nParticles!=1 && nParticles!=2 && nParticles!=3){return true;}
  
  

  return false;


}


double refineRMS(TH1D* hist ){

  double HistoRms = hist->GetRMS();
  double HistMean = hist->GetMean();
  //hist->SetAxisRange(HistMean - 3.*HistoRms, HistMean + 3.*HistoRms  );
  double   RMS =  hist->GetRMS();

  return RMS;

}

  


void validateNN( bool useTrackEstimate  ) {

 

  // gROOT->SetStyle("Plain");

  TString inputfile="root://castoratlas//castor/cern.ch/user/l/lorenzi/TrkValidation17.0.0.5_J5.root";
  //  TString inputfile="root://castoratlas//castor/cern.ch/user/l/lorenzi/TrkValidation17.0.0.5_J5_onTrack.root";
  //  TString inputfile="/tmp/lorenzi/user.lorenzi.J5.TRKVAL.NNtrain.onTrack1.110620162506/TrkValidation_J5_onTrack.root";
  //  TString inputfile="/tmp/lorenzi/user.lorenzi.T1.TRKVAL.NNtrain.onTrack.110620153532/TrkValidation_T1_onTrack.root";
  
  //  TString inputfile="/tmp/lorenzi/17.0.0.5/TrkValidation.root";
  
  TFile *file= TFile::Open(inputfile);
  
  TTree *simu = (TTree*)file->Get("Validation/NNinput");


 
  std::cout << " Training sample obtained... " << std::endl;

  vector<int>     *NN_sizeX;
  vector<int>     *NN_sizeY;
  vector<vector<float> > *NN_matrixOfToT;
  vector<vector<float> > *NN_vectorOfPitchesY;
  vector<int>     *NN_ClusterPixLayer;
  vector<int>     *NN_ClusterPixBarrelEC;
  vector<float>   *NN_phiBS;
  vector<float>   *NN_thetaBS;
  vector<float>   *NN_etaModule;
  vector<bool>    *NN_useTrackInfo;
  vector<int>     *NN_columnWeightedPosition;
  vector<int>     *NN_rowWeightedPosition;
  vector<double>     *NN_localColumnWeightedPosition;
  vector<double>     *NN_localRowWeightedPosition;

  vector<vector<float> > *NN_positionX;
  vector<vector<float> > *NN_positionY;
  vector<vector<float> > *NN_position_idX;
  vector<vector<float> > *NN_position_idY;
  vector<vector<float> > *NN_theta;
  vector<vector<float> > *NN_phi;
  
  // List of branches
  TBranch        *b_NN_sizeX;   //!
  TBranch        *b_NN_sizeY;   //!
  TBranch        *b_NN_matrixOfToT;   //!
  TBranch        *b_NN_vectorOfPitchesY;   //!
  TBranch        *b_NN_ClusterPixLayer;   //!
  TBranch        *b_NN_ClusterPixBarrelEC;   //!
  TBranch        *b_NN_phiBS;   //!
  TBranch        *b_NN_thetaBS;   //!
  TBranch        *b_NN_etaModule;   //!
  TBranch        *b_NN_useTrackInfo;   //!
  TBranch        *b_NN_columnWeightedPosition;   //!
  TBranch        *b_NN_rowWeightedPosition;   //!
  TBranch        *b_NN_localColumnWeightedPosition;   //!
  TBranch        *b_NN_localRowWeightedPosition;   //!
  TBranch        *b_NN_positionX;   //!
  TBranch        *b_NN_positionY;   //!
  TBranch        *b_NN_position_idX;   //!
  TBranch        *b_NN_position_idY;   //!
  TBranch        *b_NN_theta;   //!
  TBranch        *b_NN_phi;   //!
  


  NN_sizeX = 0;
  NN_sizeY = 0;
  NN_matrixOfToT = 0;
  NN_vectorOfPitchesY = 0;
  NN_ClusterPixLayer = 0;
  NN_ClusterPixBarrelEC = 0;
  NN_phiBS = 0;
  NN_thetaBS = 0;
  NN_etaModule = 0;
  NN_useTrackInfo = 0;
  NN_columnWeightedPosition = 0;
  NN_rowWeightedPosition = 0;
  NN_localColumnWeightedPosition = 0;
  NN_localRowWeightedPosition = 0;
  NN_positionX = 0;
  NN_positionY = 0;
  NN_position_idX = 0;
  NN_position_idY = 0;
  NN_theta = 0;
  NN_phi = 0;
  // Set branch addresses and branch pointers
  //  if (!tree) return 0;
  //  TTree* simu = tree;
  //  fCurrent = -1;
  simu->SetMakeClass(1);

  simu->SetBranchAddress("NN_sizeX", &NN_sizeX, &b_NN_sizeX);
  simu->SetBranchAddress("NN_sizeY", &NN_sizeY, &b_NN_sizeY);
  simu->SetBranchAddress("NN_matrixOfToT", &NN_matrixOfToT, &b_NN_matrixOfToT);
  simu->SetBranchAddress("NN_vectorOfPitchesY", &NN_vectorOfPitchesY, &b_NN_vectorOfPitchesY);
  simu->SetBranchAddress("NN_ClusterPixLayer", &NN_ClusterPixLayer, &b_NN_ClusterPixLayer);
  simu->SetBranchAddress("NN_ClusterPixBarrelEC", &NN_ClusterPixBarrelEC, &b_NN_ClusterPixBarrelEC);
  simu->SetBranchAddress("NN_phiBS", &NN_phiBS, &b_NN_phiBS);
  simu->SetBranchAddress("NN_thetaBS", &NN_thetaBS, &b_NN_thetaBS);
  simu->SetBranchAddress("NN_etaModule", &NN_etaModule, &b_NN_etaModule);
  simu->SetBranchAddress("NN_useTrackInfo", &NN_useTrackInfo, &b_NN_useTrackInfo);
  simu->SetBranchAddress("NN_columnWeightedPosition", &NN_columnWeightedPosition, &b_NN_columnWeightedPosition);
  simu->SetBranchAddress("NN_rowWeightedPosition", &NN_rowWeightedPosition, &b_NN_rowWeightedPosition);

  simu->SetBranchAddress("NN_localColumnWeightedPosition", &NN_localColumnWeightedPosition, &b_NN_localColumnWeightedPosition);
  simu->SetBranchAddress("NN_localRowWeightedPosition", &NN_localRowWeightedPosition, &b_NN_localRowWeightedPosition);

  simu->SetBranchAddress("NN_positionX", &NN_positionX, &b_NN_positionX);
  simu->SetBranchAddress("NN_positionY", &NN_positionY, &b_NN_positionY);
  simu->SetBranchAddress("NN_position_idX", &NN_position_idX, &b_NN_position_idX);
  simu->SetBranchAddress("NN_position_idY", &NN_position_idY, &b_NN_position_idY);

  simu->SetBranchAddress("NN_theta", &NN_theta, &b_NN_theta);
  simu->SetBranchAddress("NN_phi", &NN_phi, &b_NN_phi);
  

  
 
  cout << "Branches set..." << endl;

//  int   nParticlesTraining = 1 ;


  TString    pathWithoutTracks  = "/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetJune2011/withoutTracks/Weights.root";
  TString    pathWithTracks     = "/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtupleApril2011/withTracks/Weights.root";
  
  
  TString name;

  if(!useTrackEstimate){    
    
    name+= pathWithoutTracks;
	
  }else{ 
    
    name+=pathWithTracks;
    
  }


  // getting a postion trained network from file
  TFile *_file0 = new TFile(name);
  TTrainedNetwork* numberTrainedNetwork=(TTrainedNetwork*)_file0->Get("TTrainedNetwork");
  
  cout << " Reading back network with minimum" << endl;
   

  Int_t           sizeX=-7; 
  Int_t           sizeY=-7; 
 

  int iClus=0;

  TH1D* histo1 = new TH1D("1","1",1000,0,1);
  TH1D* histo2 = new TH1D("2","2",1000,0,1);
  TH1D* histo3 = new TH1D("3","3",1000,0,1);

  TH1D* ahisto2= new TH1D("a2","a2",1000,0,1);
  TH1D* ahisto3= new TH1D("a3","a3",1000,0,1);

  // Loop over entries:
  for (Int_t i = 0; i < simu->GetEntries(); i++) {
    
    if (i % 1000 == 0 ) {
      std::cout << " Counting training / testing events in sample. Looping over event " << i << " cluster: " << iClus << std::endl;
    }
    
    if (i > 50000 ) break;
    //    if(iClus > 10000) break;
 
   simu->GetEntry(i);
    
    for( unsigned int clus =0; clus<NN_sizeX->size(); clus++ ){
      //  cout << clus << endl;
      vector<float>  *matrixOfToT=0;
      vector<float>   *vectorOfPitchesY=0;
      
      Float_t         phiBS;
      Float_t         thetaBS;
      Float_t         etaModule;
      Int_t ClusterPixLayer;
      Int_t ClusterPixBarrelEC;
      
      std::vector<float> * positionX=0;
      std::vector<float> * positionY=0;

      std::vector<float> * position_idX=0;
      std::vector<float> * position_idY=0;


      std::vector<float> * thetaTr=0;
      std::vector<float> * phiTr=0;

      double localColumnWeightedPosition;// = 0;
      double localRowWeightedPosition;// = 0;

      double columnWeightedPosition;// = 0;
      double rowWeightedPosition;// = 0;



      sizeX = (*NN_sizeX)[clus];
      sizeY = (*NN_sizeY)[clus];
      
      matrixOfToT=&(*NN_matrixOfToT)[clus];
      vectorOfPitchesY=&(*NN_vectorOfPitchesY)[clus];
      
      phiBS = (*NN_phiBS)[clus];
      thetaBS =(*NN_thetaBS)[clus];
      etaModule =(*NN_etaModule)[clus];
      
      ClusterPixLayer=(*NN_ClusterPixLayer)[clus];
      ClusterPixBarrelEC = (*NN_ClusterPixBarrelEC)[clus];
      
      positionX =&(*NN_positionX)[clus];
      positionY =&(*NN_positionY)[clus];

      position_idX =&(*NN_position_idX)[clus];
      position_idY =&(*NN_position_idY)[clus];
      
      thetaTr = &(*NN_theta)[clus];
      phiTr = &(*NN_phi)[clus];
      

      localColumnWeightedPosition =(*NN_localColumnWeightedPosition)[clus];
      localRowWeightedPosition    =(*NN_localRowWeightedPosition)[clus];
 
      columnWeightedPosition =(*NN_columnWeightedPosition)[clus];
      rowWeightedPosition    =(*NN_rowWeightedPosition)[clus];
 
      //    if( ClusterPixLayer!=0 || ClusterPixBarrelEC!=0) continue;



      unsigned int nParticles = positionX->size();
      


      if (isBadCluster(sizeX, nParticles ) )continue;
      
      thetaTr = &(*NN_theta)[clus];
      phiTr = &(*NN_phi)[clus];
      


      // loop over the particles;  
      for( unsigned int P = 0; P < positionX->size(); P++){
        iClus++;
	
	if(iClus > 5000) break;

	double	theta = (*thetaTr)[P];
	double	phi   = (*phiTr)[P];
	
	std::vector<Double_t> inputData;
	
	for(unsigned int ME =0; ME < matrixOfToT->size(); ME++){
	  
	  inputData.push_back(norm_ToT((*matrixOfToT)[ME]));
	}
	
	for (int s=0;s<sizeY;s++)
	  {
	    inputData.push_back(norm_pitch((*vectorOfPitchesY)[s])); 
	  }
	
	inputData.push_back(norm_layerNumber(ClusterPixLayer));
	inputData.push_back(norm_layerType(ClusterPixBarrelEC));
	
	if (useTrackEstimate)
	  {
	    inputData.push_back(norm_phi(phi));
	    inputData.push_back(norm_theta(theta));
	  }
	else
	  {
	    inputData.push_back(norm_phiBS(phiBS));
	    inputData.push_back(norm_thetaBS(thetaBS));
	    inputData.push_back(norm_etaModule(etaModule));
	  }
	//	cout << "formatted" << endl;


	vector<double> outputNN = numberTrainedNetwork->calculateOutputValues(inputData);

	int nParticles=	positionX->size();

	if( nParticles ==1 ) { 

	  histo1->Fill(outputNN[0]/(outputNN[0]+outputNN[1]+outputNN[2])); 


	}

	if( nParticles ==2 ) { 
	  
	  histo2->Fill(outputNN[0]/(outputNN[0]+outputNN[1]+outputNN[2]));

	  if( outputNN[0]/(outputNN[0]+outputNN[1]+outputNN[2])<0.98 ){

	    ahisto2->Fill(  outputNN[1]/(outputNN[1]+outputNN[2]) );
	  }

	}
		
	if( nParticles ==3 ) { 
	  histo3->Fill(outputNN[0]/(outputNN[0]+outputNN[1]+outputNN[2])); 
	 
	  if( outputNN[0]/(outputNN[0]+outputNN[1]+outputNN[2])<0.98 ) {
	    
	    ahisto3->Fill(outputNN[1]/(outputNN[1]+outputNN[2])  ); 

	  }

	}


	
	
	
      }// end loop over th particles
    }// end loop over cluster
  }// end Loop over entries
  





  double* integral1=  histo1->GetIntegral();
  double* integral2=  histo2->GetIntegral();
  double* integral3=  histo3->GetIntegral();

  double xcut[1000];

  double integral4[1000];
  double integral5[1000];
  for (int u=0;u<1000;u++)
  {
    xcut[u]=histo2->GetBinCenter(u);
    integral4[u]=1-integral2[u];
    integral5[u]=1-integral3[u];
  }

  TGraphErrors* Have1Vs2=new TGraphErrors(1000,integral1,integral4,0,0);
  TGraphErrors* Have1Vs3=new TGraphErrors(1000,integral1,integral5,0,0);
  TGraphErrors* HaveCutVs2=new TGraphErrors(1000,xcut,integral4,0,0);

  TCanvas * c1=new TCanvas("c1","c1",600,600);

  Have1Vs2->SetLineColor(2);
  Have1Vs2->SetMarkerColor(2);
  Have1Vs2->SetMarkerSize(0.4);
  Have1Vs2->SetMarkerStyle(3);
  Have1Vs2->Draw("AP");

  Have1Vs2->GetXaxis()->SetTitle("Fraction of wrongly split single-cluster");
  Have1Vs2->GetYaxis()->SetTitle("Fraction of not split multi-clusters");




 double* aintegral2=  ahisto2->GetIntegral();
  double* aintegral3=  ahisto3->GetIntegral();

  double axcut[1000];

  double aintegral4[1000];
  double aintegral5[1000];
  for (int u=0;u<1000;u++)
  {
    axcut[u]=ahisto2->GetBinCenter(u);
    aintegral4[u]=1-aintegral2[u];
    aintegral5[u]=1-aintegral3[u];
  }

//  double* aintegral21=  ahisto21->GetIntegral();


  TCanvas* c4=new TCanvas("c4","c4",600,600);

  TGraphErrors* aHave1Vs3=new TGraphErrors(1000,aintegral2,aintegral5,0,0);
  TGraphErrors* aHaveCutVs2=new TGraphErrors(1000,axcut,aintegral4,0,0);


  aHave1Vs3->GetXaxis()->SetTitle("Fraction of wrongly split cluster (2 #rightarrow 3)");
  aHave1Vs3->GetYaxis()->SetTitle("Fraction of not split multi-clusters (3 #rightarrow 2)");


  aHave1Vs3->SetLineColor(4);
  aHave1Vs3->SetMarkerColor(4);
  aHave1Vs3->SetMarkerSize(0.5);
  aHave1Vs3->SetMarkerStyle(3);
  aHave1Vs3->Draw("AP");


  //  TCanvas * c2=new TCanvas("c2","c2",600,600);
  // HaveCutVs2->Draw("AP");







}

