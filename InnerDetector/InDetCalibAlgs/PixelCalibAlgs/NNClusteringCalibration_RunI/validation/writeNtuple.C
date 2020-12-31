/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#define writeNtuple_cxx
#include "writeNtuple.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
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

#include "TMatrixD.h"
#include "TVectorD.h"
//#include "trainNN.h"
#include <algorithm>
#include <vector>
#include <utility>
#include <TLatex.h>
#include "TH2D.h"
#include <sstream>
#include <TProfile.h>
#include <TMath.h>
#include <TChain.h>
#include <typeinfo>
#include "TROOT.h"

using namespace std;

bool isBadCluster( int sizeX, int nParticles ){


  if(sizeX==-100){return true;}
  if(nParticles==0){return true;}
  if(nParticles!=1 && nParticles!=2 && nParticles!=3){return true;}



  return false;
 
}



void writeNtuple::Loop(  int nParticlesTraining, bool useTrackEstimate, TString outFileName  )
{
//   In a ROOT session, you can do:
//      Root > .L writeNtuple.C
//      Root > writeNtuple t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

//   NNTrkValidationNtuple_t.root
//   bool useTrackEstimate =false;
//   int nParticlesTraining =1;

   if(outFileName == 0 ){
 
    cout <<"ERROR: plase provide a name for the output file" << endl;
    return;

   }
   
   gROOT->ProcessLine("#include <vector>");

   TFile *newfile = new TFile(outFileName,"recreate");

   newfile->mkdir("Validation");


   TTree* treeN = new TTree("CombinedInDetTracks","CombinedInDetTracks");


   // treeN->SetMakeClass(0);


 
   // Layer information (0, 1, 2 for barrel /
   // 100, 101, 102 for endcap A side, -100,
   // -101, -102 for endcap C side) it is 0 by default
   treeN->Branch("PixDetType",      &m_PixDetType);
   // 1 if the cluster contains ganged pixels
   treeN->Branch("PixHasGanged",    &m_PixHasGanged);
   // 1 if the cluster is marked as fake
   treeN->Branch("PixIsFake",       &m_PixIsFake);
   // lower level1 accept among the pixel of the cluster
   treeN->Branch("PixLVL1A",        &m_PixLVL1A);
   // total ToT of the cluster
   treeN->Branch("PixToT",          &m_PixToT);
   // total charge of the cluster
   treeN->Branch("PixCharge",       &m_PixCharge);
   // tangent of the lorentz angle (local x direction)
   treeN->Branch("PixTLorPhi",      &m_PixTLorPhi);
   // tangent of the lorentz angle (local y direction - should be 0!)
   treeN->Branch("PixTLorEta",      &m_PixTLorEta);
   // Bias voltage (from conditions service)
   treeN->Branch("PixBiasVolt",      &m_PixBiasVolt);
   // Temperature (from conditions service)
   treeN->Branch("PixTemp",      &m_PixTemp);
   // Depletion voltage (from conditions service)
   treeN->Branch("PixDepVolt",      &m_PixDepVolt);
   // Residuals obtained using the following cluster coordinates:
   // [x(rowmin)+x(rowmax)]/2.
   // [y(colmin)+y(colmax)]/2
   // they are useful for the tuning of charge interpolation algorithms
   treeN->Branch("PixDigResPhi",    &m_PixDigResPhi);
   treeN->Branch("PixDigResEta",    &m_PixDigResEta);
   treeN->Branch("PixResPhi",       &m_PixResPhi);
   treeN->Branch("PixResEta",       &m_PixResEta);
   // Pixel Cluster position error (local x)
   treeN->Branch("PixErrPhi",       &m_PixErrPhi);
   // Pixel Cluster position error (local y)
   treeN->Branch("PixErrEta",       &m_PixErrEta);
   // Cluster local coordinates
   treeN->Branch("PixLocX",         &m_PixLocX);
   treeN->Branch("PixLocY",         &m_PixLocY);
   // Eta and phi indexes of the module
   treeN->Branch("PixEtaModule",    &m_PixEtaModule_t);
   treeN->Branch("PixPhiModule",    &m_PixPhiModule_t);
   // Global position of the cluster
   treeN->Branch("PixGloX",         &m_PixGloX);
   treeN->Branch("PixGloY",         &m_PixGloY);
   treeN->Branch("PixGloZ",         &m_PixGloZ);
   treeN->Branch("PixEta",          &m_PixEta_t);
//    // lower row of the pixels contained in the cluster
//    treeN->Branch("PixFirstRow",     &m_PixFirstRow);
//    // lower column of the pixels contained in the cluster
//    treeN->Branch("PixFirstCol",     &m_PixFirstCol);
   // Pixel Cluster Size, in cell units
   treeN->Branch("PixDeltaRow",     &m_PixDeltaRow_t);
   treeN->Branch("PixDeltaCol",     &m_PixDeltaCol_t);
   // Pixel cluster size, in mm
//    treeN->Branch("PixDeltaPhi",     &m_PixDeltaPhi_t);
//    treeN->Branch("PixDeltaEta",     &m_PixDeltaEta_t);
   // Pixel charge sharing variable. It is defined as
   // omega_phi = Q1/(Q+Q2)
   // where Q1 is the sum of the charges of the pixel cells of the
   // first row of the cluster, Q2 is the sum of the charges of
   // pixel cells of the last row of the cluster
   // For omega_eta the definition is similar, changing row with
   // columns
   // It is a data member of PixelCluster.cxx and it is used in
   // charge interpolation algorithms
   treeN->Branch("PixOmegaPhi",     &m_PixOmegaPhi_t);
   treeN->Branch("PixOmegaEta",     &m_PixOmegaEta_t);
   /// detailed information for Pixel in the Cluster
   /*          tree->Branch("PixChargeList",&m_PixChargeList) ;
	       tree->Branch("PixToTList",&m_PixToTList) ;
	       tree->Branch("PixLVL1AList",&m_PixLVL1AList) ;
	       tree->Branch("PixEtaIndex", &m_PixEtaIndex);
	       tree->Branch("PixPhiIndex", &m_PixPhiIndex);
   */      // Track incidence angle on the module (rphi plane) in local reference frame
   treeN->Branch("PixTrkAngle",     &m_PixTrkAngle);
   // Track incidence angle on the module (eta-normal plane) in local reference frame
   treeN->Branch("PixTrkThetaI",     &m_PixTrkThetaI);
   // Track pseudorapidity incidence in local module reference frame
   // i.e. m_log(atan(theta/2)) where theta is the local polar angle
   treeN->Branch("PixTrkEta",       &m_PixTrkEta);
        // Track momentum
   treeN->Branch("PixTrkPt",        &m_PixTrkPt);
   // charge of the associated track (this allows to easily
   // make plots for positive and negative particles using the same
   // ntuple, for example)
   treeN->Branch("PixTrkQ",         &m_PixTrkQ);
   // total size of the cluster
   treeN->Branch("PixTrkClusGroupSize", &m_PixTrkClusGroupSize);
   
   treeN->Branch("nPixelHits", &m_nPixelHits);
   treeN->Branch("nSCTHits",&m_nSCTHits);
   treeN->Branch("nTRTHits",&m_nTRTHits);

   treeN->Branch("PixPullPhi",m_PixPullPhi);
   treeN->Branch("PixPullEta",m_PixPullEta);

   treeN->Branch("RecTheta", &m_PixTrkThetaI);


  cout << "Branches set..." << endl;


 // Naming convention for the weights filename to be set and understood 


  TString name;
  TString nameErrX;
  TString nameErrY;
  
  TString tr;
//  if(useTrackEstimate) tr+="_withTrack";
  TString tr1;
//  if(!useTrackEstimate) tr1+="_noTrack";


  if(!useTrackEstimate) {


/*
    name    +="/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetJune2011/withoutTracks/";
    nameErrX+="/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetJune2011/withoutTracks/";
    nameErrY+="/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetJune2011/withoutTracks/";
*/   
    name += m_pathWeightsWithoutTracks;
    nameErrX = name;
    nameErrY = name;
 
    if(nParticlesTraining == 1 ){   name+="WeightsOneTracks"+tr1+".root";  nameErrX+="WeightsOneTrackErrorX"+tr+".root";   nameErrY+="WeightsOneTrackErrorY"+tr+".root"; }
    if(nParticlesTraining == 2 ){   name+="WeightsTwoTracks"+tr1+".root";  nameErrX+="WeightsTwoTracksErrorX"+tr+".root";   nameErrY+="WeightsTwoTracksErrorY"+tr+".root"; }
    if(nParticlesTraining == 3 ){   name+="WeightsThreeTracks"+tr1+".root";nameErrX+="WeightsThreeTracksErrorX"+tr+".root"; nameErrY+="WeightsThreeTracksErrorY"+tr+".root"; }
    
    
  }else{
    /*
    name    +="/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtupleApril2011/withTracks/";
    nameErrX+="/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtupleApril2011/withTracks/";
    nameErrY+="/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtupleApril2011/withTracks/";
  */
    name += m_pathWeightsWithTracks;   
    nameErrX = name;
    nameErrY = name;
 
    if(nParticlesTraining == 1 ){   name+="WeightsOneTrack"+tr1+".root";  nameErrX+="WeightOneTrackErrorX"+tr+".root";   nameErrY+="WeightOneTrackErrorY"+tr+".root"; }
    if(nParticlesTraining == 2 ){   name+="WeightsTwoTracks"+tr1+".root";  nameErrX+="WeightTwoTracksErrorX"+tr+".root";   nameErrY+="WeightTwoTracksErrorY"+tr+".root"; }
    if(nParticlesTraining == 3 ){   name+="WeightsThreeTracks"+tr1+".root";nameErrX+="WeightThreeTracksErrorX"+tr+".root"; nameErrY+="WeightThreeTracksErrorY"+tr+".root"; }
    
    
  }

  cout << "Reading weights from files:  " << endl;
  cout << name << endl;
  cout << nameErrX << endl;
  cout << nameErrY <<endl;
  // return;

  // getting a postion trained network from file
  TFile *_file0 = new TFile(name);
  TTrainedNetwork* positionTrainedNetwork=(TTrainedNetwork*)_file0->Get("TTrainedNetwork");
  
  TFile *_file1 = new TFile(nameErrX);
  TTrainedNetwork* network_errX =(TTrainedNetwork*)_file1->Get("TTrainedNetwork");
  
  TFile *_file2 = new TFile(nameErrY);
  TTrainedNetwork* network_errY=(TTrainedNetwork*)_file2->Get("TTrainedNetwork");


  cout << " Reading back network with minimum" << endl;
   

  Int_t           sizeX=-7; 
  Int_t           sizeY=-7; 

  int iClus=0;

  Long64_t nentries = fChain->GetEntriesFast();
//  Long64_t nentriesO= fOther->fChain->GetEntriesFast();




  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

//    cout <<  fOther << endl;


//    cout << typeid(fOther).name() << endl;

// The following 3 lines are needed if the 2 tree are not friend.
   ientry = fOther->LoadTree(jentry);


   if (ientry < 0) break;
   nb += fOther->fChain->GetEntry(jentry); nbytes += nb; 




    // if (Cut(ientry) < 0) continue;
    
    if(jentry%100 == 0 ) {cout << "Processing event: " << jentry << " cluster " << iClus << " ->PixClusNum " << fOther->PixelClusNum << " "<<NN_sizeX->size()    << endl; }
    // if(iClus>10000)break;
     
//          if (jentry > 300 ) break;
   


    ResetVectors();


    // cout << jentry << "\t" <<  NN_etaModule->size() << "\t" <<  fOther->PixEtaModule->size() << endl;


    // continue;

    //    if(NN_sizeX->at(0)==0)continue;
    //    if(NN_sizeX->size()!=fOther->PixClusEta->size())continue;
    // if(NN_etaModule->size()!=fOther->PixEtaModule->size())continue;

    //   cout  << "match" << endl;

    // m_nPixelHits = NN_sizeX->size();

    int nPixelHits = 0;

    //   cout << fOther->PixelClusNum << endl;
    for( unsigned int clus =0; clus< NN_sizeX->size(); clus++ ){    //fOther->PixelClusNum; clus++){//
      nPixelHits++;

      //  cout << "clus" << endl;
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

      //      double locX = (*PixClusLocX)[clus];
      //       double locY = (*PixClusLocY)[clus];
      

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
 


      unsigned int nParticles = positionX->size();
      
      if(  nParticles !=  nParticlesTraining ) continue;


      if (isBadCluster(sizeX, nParticles ) )continue;
      
      thetaTr = &(*NN_theta)[clus];
      phiTr = &(*NN_phi)[clus];
      
      //      cout << "---" << endl;

      vector<double> outputNN_errX;  
      vector<double> outputNN_errY;

      // loop over the particles;  
      for( unsigned int P = 0; P < positionX->size(); P++){
        iClus++;
	//cout << "P " << endl; 
	//cout << clus << " " <<  iClus << endl;	
	
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
	//		cout << "formatted" << endl;


	vector<double> outputNN_idX;
	vector<double> outputNN_idY;
	// using the trained network for position to obtain the position
	//	cout << "Obtaining Positions... " << endl;
	vector<double> resultNN = positionTrainedNetwork->calculateOutputValues(inputData);
	//	cout << "Positions Obtained" << endl;

        if(nParticlesTraining==1){
          outputNN_idX.push_back( back_posX(resultNN[0]));
          outputNN_idY.push_back( back_posY(resultNN[1]));
        }


        if(nParticlesTraining==2){
          outputNN_idX.push_back( back_posX(resultNN[0]));
	  outputNN_idX.push_back( back_posX(resultNN[2]));
          outputNN_idY.push_back( back_posY(resultNN[1]));
          outputNN_idY.push_back( back_posY(resultNN[3]));
        }

        if(nParticlesTraining==3){
          outputNN_idX.push_back( back_posX(resultNN[0]));
          outputNN_idX.push_back( back_posX(resultNN[2]));
          outputNN_idX.push_back( back_posX(resultNN[4]));
          outputNN_idY.push_back( back_posY(resultNN[1]));
          outputNN_idY.push_back( back_posY(resultNN[3]));
          outputNN_idY.push_back( back_posY(resultNN[5]));
        }


	int sizeReal;

	std::vector<Double_t> inputDataNewError;
	//	cout << "data error " << endl;

	if(nParticlesTraining==1)
	  {
	    inputDataNewError=inputData;
	    inputDataNewError.push_back(norm_posX((outputNN_idX)[0]));
	    inputDataNewError.push_back(norm_posY((outputNN_idY)[0]));
	    	    
	  }
	
	
        if(nParticlesTraining==2){
	  
	  inputDataNewError=inputData;
	  inputDataNewError.push_back(norm_posX((outputNN_idX)[1]));
	  inputDataNewError.push_back(norm_posY((outputNN_idY)[1]));
	  inputDataNewError.push_back(norm_posX((outputNN_idX)[2]));
	  inputDataNewError.push_back(norm_posY((outputNN_idY)[2]));
	}
	
        if(nParticlesTraining==3){
	  	 
	  inputDataNewError=inputData;
	  inputDataNewError.push_back(norm_posX((outputNN_idX)[1]));
	  inputDataNewError.push_back(norm_posY((outputNN_idY)[1]));
	  inputDataNewError.push_back(norm_posX((outputNN_idX)[2]));
	  inputDataNewError.push_back(norm_posY((outputNN_idY)[2]));
	  inputDataNewError.push_back(norm_posX((outputNN_idX)[3]));
	  inputDataNewError.push_back(norm_posY((outputNN_idY)[3]));
	}
	
	
	
	vector<double>	outputX=network_errX->calculateOutputValues(inputDataNewError);
	vector<double>	outputY=network_errY->calculateOutputValues(inputDataNewError);

	//	cout << "error obtained" << endl;

	int nParticles = nParticlesTraining;

	int sizeOutputX=outputX.size()/nParticles;
	int sizeOutputY=outputY.size()/nParticles;
	
	double minimumX=-errorHalfIntervalX(nParticles);
	double maximumX=errorHalfIntervalX(nParticles);
	
	double minimumY=-errorHalfIntervalY(nParticles);
	double maximumY=errorHalfIntervalY(nParticles);
	
	
	//X=0...sizeOutput-1
	//Y=minimum+(maximum-minimum)/sizeOutput*(X+1./2.)
	
	for (int i=0;i<nParticles;i++)
	  {
	    double sumValuesX=0;
	    for (int u=0;u<sizeOutputX;u++)
	      {



		sumValuesX+=outputX[i*sizeOutputX+u];
	      }
	    double sumValuesY=0;
	    for (int u=0;u<sizeOutputY;u++)
	      {
		sumValuesY+=outputY[i*sizeOutputY+u];
	      }
	    
	    double RMSx=0;
	    for (int u=0;u<sizeOutputX;u++)
	      {
		
		RMSx+=outputX[i*sizeOutputX+u]/sumValuesX*std::pow(minimumX+(maximumX-minimumX)/(double)(sizeOutputX-2)*(u-1./2.),2);
	      }
	    RMSx=sqrt(RMSx);//computed error!
	    
 	    double intervalErrorX=3*RMSx;
	    
 	    //now recompute between -3*RMSx and +3*RMSx
 	    int minBinX=(int)(1+(-intervalErrorX-minimumX)/(maximumX-minimumX)*(double)(sizeOutputX-2));
 	    int maxBinX=(int)(1+(intervalErrorX-minimumX)/(maximumX-minimumX)*(double)(sizeOutputX-2));
 
 	    if (maxBinX>sizeOutputX-1) maxBinX=sizeOutputX-1;
 	    if (minBinX<0) minBinX=0;
	    
 	    RMSx=0;
 	    for (int u=minBinX;u<maxBinX+1;u++)
 	      {
 		RMSx+=outputX[i*sizeOutputX+u]/sumValuesX*std::pow(minimumX+(maximumX-minimumX)/(double)(sizeOutputX-2)*(u-1./2.),2);
 	      }
 	    RMSx=sqrt(RMSx);//computed error!
	    
	    double RMSy=0;
	    for (int u=0;u<sizeOutputY;u++)
	      {
		RMSy+=outputY[i*sizeOutputY+u]/sumValuesY*std::pow(minimumY+(maximumY-minimumY)/(double)(sizeOutputY-2)*(u-1./2.),2);
	      }

	    RMSy=sqrt(RMSy);//computed error!
	    
    
 	    double intervalErrorY=3*RMSy;
 
 	    //now recompute between -3*RMSy and +3*RMSy
 	    int minBinY=(int)(1+(-intervalErrorY-minimumY)/(maximumY-minimumY)*(double)(sizeOutputY-2));
 	    int maxBinY=(int)(1+(intervalErrorY-minimumY)/(maximumY-minimumY)*(double)(sizeOutputY-2));
	    
 	    if (maxBinY>sizeOutputY-1) maxBinY=sizeOutputY-1;
 	    if (minBinY<0) minBinY=0;
 
	    
 	    RMSy=0;
 	    for (int u=minBinY;u<maxBinY+1;u++)
 	      {
 		RMSy+=outputY[i*sizeOutputY+u]/sumValuesY*std::pow(minimumY+(maximumY-minimumY)/(double)(sizeOutputY-2)*(u-1./2.),2);
 	      }
 	    RMSy=sqrt(RMSy);//computed error!
	    
	    outputNN_errX.push_back(RMSx);
	    //	    cout <<i << " " << RMSx << endl;
	    
	    outputNN_errY.push_back(RMSy);
	           
	  
	
	  }
	//	cout << "errors computed" << endl;




        // HERE WE NEED THE CONVERSION OF THE POSITION RESPECT WITH CLUSTER CENTER FROM LOCAL POSITION!!!
        vector<float> outputNN_X ;
        vector<float> outputNN_Y ;

        for(unsigned int t=0; t < outputNN_idX.size(); t++){

          double PitchX=0.05; //50 micron pitch in Y
          // local position respect with the cluster center    questa e' la posizione locale corrispondente al centro del cluster
          // defined as the cemter of matrixOfToT              (definito come centro della matrice per la rete neuronale)
          
          // inconcistency 
	  // should be X -> row   Y -> column !!! 
	  // but in the code is X->col , Y->row
	  
          double centerPosY = localColumnWeightedPosition;
          double centerPosX = localRowWeightedPosition;
          

          //questi sono gli unput che la rete restituisce (coordinate rispetto al centro)
          // output of the NN (respect with the cluster center
          double indexX =  outputNN_idX[t];
          double indexY =  outputNN_idY[t];
          double indexPositionToTranslateY = indexY+(double)(sizeY-1)/2;
          //       cout << indexX << " " << centerPosX << endl;
          double pos_idX = centerPosX + (double)indexX * PitchX;
          //cout << " position X = " << pos_idX << endl;

          //now for Y it is slightly more complicated (but not much)
          double positionYFromZero = -100;
          double positionCenterYFromZero = -100;
          double actualPositionFromZero=0.;

          //start both indices and positions at zero

          for (int i=0;i<sizeY;i++)
            {
              if (indexPositionToTranslateY>=(double)i && indexPositionToTranslateY<=(double)(i+1))
                {
                  positionYFromZero = actualPositionFromZero + (double)(indexPositionToTranslateY-(double)i+0.5)*(*vectorOfPitchesY)[i];
                  //              cout << "positionYFromZero: " << positionYFromZero << endl;
                }
              if (i==(sizeY-1)/2)
                {
                  positionCenterYFromZero =  actualPositionFromZero + 0.5* (*vectorOfPitchesY)[i];
                  //              cout << "positionCenterYFromZero: " << positionCenterYFromZero << endl;
                }
              actualPositionFromZero+=(*vectorOfPitchesY)[i];
            }

          double pos_idY = centerPosY + positionYFromZero - positionCenterYFromZero;


          //
          outputNN_X.push_back(pos_idX);
          outputNN_Y.push_back(pos_idY);
        }

	//	cout << "position converted" <<	  endl;
	  

	// sort according to X position 	
	int n = positionX->size();	
	
	bool swapped = true;

	int j = 0;
	
	double tmpX;
	double tmpY;
	
	while (swapped) {
	  
	  swapped = false;

	  j++;
	  
	  for (int i = 0; i < n - j; i++) {
	    
	    if ( positionX->at(i) > positionX->at(i + 1)) {
	      
	      tmpX = positionX->at(i);
	      (*positionX)[i] =  positionX->at(i + 1);
	      (*positionX)[i + 1] = tmpX;
	      
	      tmpY = positionY->at(i);
	      (*positionY)[i] =  positionY->at(i + 1);
	      (*positionY)[i + 1] = tmpY;
	      
	      swapped = true;
	      
	    }
	    
	  }
	  
	}

	//	cout << "position ordered" << endl; 


        double minDist = 10000000;

        double TruthLocX;
        double TruthLocY;
      
        int truthIndex;         
        // check which associated truth locX and locY are closer to the estimated passage point
        for( unsigned int mp=0; mp < outputNN_X.size(); mp++){



          double Xtruth =  (*positionX)[mp];
          double Ytruth =  (*positionY)[mp];
          

          double d = sqrt ( pow(Xtruth - outputNN_X[mp],2. ) +
                            pow(Ytruth - outputNN_Y[mp],2.) );


          if( minDist >  d ){

            minDist = d;

            TruthLocX = Xtruth;
            TruthLocY = Ytruth;
  
            truthIndex = mp;

          }




        }
	//	cout << "position chosen " << endl;

        double locX_NN =  outputNN_X[truthIndex];
        double locY_NN =  outputNN_Y[truthIndex];

	double errX_NN = outputNN_errX[truthIndex];
	double errY_NN = outputNN_errY[truthIndex];


	//	cout << "position and error set " << endl;


	// if you want to fill with old clustering insted of NN uncomment this

	// 	locX_NN = fOther->PixClusLocX->at(clus) -  fOther->PixClusLocX_LorentzCorrection->at(clus);
	// 	locY_NN = fOther->PixClusLocY->at(clus);

        m_PixLocX->push_back( locX_NN );
        m_PixLocY->push_back( locY_NN );
                             
        m_PixErrPhi->push_back( errX_NN );
        m_PixErrEta->push_back( errY_NN );


	double PixDigResPhi = TruthLocX - fOther->PixClusLocXcentroid->at(clus) +  fOther->PixClusLocX_LorentzCorrection->at(clus) ;
	double PixDigResEta = TruthLocY - fOther->PixClusLocYcentroid->at(clus)  ;

	//	cout << "digital resolution obtained" << endl;

 	m_PixDigResPhi->push_back( PixDigResPhi);
 	m_PixDigResEta->push_back( PixDigResEta);


	double PixResPhi = TruthLocX - locX_NN;
	double PixResEta = TruthLocY - locY_NN;

 	m_PixResPhi->push_back( PixResPhi);
 	m_PixResEta->push_back( PixResEta);


	m_PixPullPhi->push_back( PixResPhi / errX_NN);
	m_PixPullEta->push_back( PixResEta / errY_NN);



	float PixTrkAngle   =  phi  ;
	float PixTrkThetaI   = theta  ;


	double eta = 0;
        if( ClusterPixBarrelEC == 0  ){
	  
          if(theta > M_PI / 2 ) theta -= M_PI;
          else if(theta < - M_PI / 2 ) theta += M_PI;
          theta = ( M_PI / 2 ) - theta;
          eta = -log(tan(theta/2));
        }
	

        if( ClusterPixBarrelEC == 2  ){
          if(theta > 0) theta*=-1.;
          eta  = -TMath::Log(TMath::Tan((-1.*theta)*.5 ));
	}
	
        if( ClusterPixBarrelEC == -2 ){
	  if(theta < 0) theta*=-1.;
          eta  = TMath::Log(TMath::Tan((theta)*.5 ));
	  
        }
      // 
   	float PixTrkEta     = eta ;
	float PixTrkPt      = 10000.;
	float PixTrkQ       = 10000.;
	float PixTrkClusGroupSize = fOther->PixClusGroupsize->at(clus) ;
 	
	m_PixTrkAngle->push_back(PixTrkAngle);
	m_PixTrkThetaI->push_back(PixTrkThetaI);
	m_PixTrkEta->push_back(PixTrkEta);
	m_PixTrkPt->push_back(PixTrkPt);
	m_PixTrkQ->push_back(PixTrkQ);
	m_PixTrkClusGroupSize->push_back(PixTrkClusGroupSize);

	//	cout <<"variables  filled" << endl; 

        FillClusterInfo(clus);      

	// need to clear all vectors?
 
      }//part  
    }//clus

    m_nPixelHits = m_PixTrkClusGroupSize->size();
    m_nSCTHits=100;
    m_nTRTHits=100;

    treeN->Fill();

  }//entries
  newfile->cd("Validation");
  treeN->Print();
  treeN->Write(); 
}

 
