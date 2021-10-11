/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TPad.h>

#include "../TJetNet.h"
#include "../doNormalization.C"
#include "Riostream.h"
#include "../TNetworkToHistoTool.h"
#include <TSystem.h>

#include "../TTrainedNetwork.h"

#include "TMatrixD.h"
#include "TVectorD.h"
#include "trainNN.h"

#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <cmath>
#include <iostream>

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


bool skipSingle(int nParticles,int iClus, int dilutionFactor){


  // this reduces the number of single particle clusters of a factor k
  int k = 60;
   

  if (nParticles==1)
    {
      if ((iClus %(dilutionFactor*k) != 0) &&  (iClus % (dilutionFactor*k) != 1))
	{
	  return true;
	}
    }

  return false;
  
}



bool badTrackInfo(bool useTrackEstimate, double theta){

  if(useTrackEstimate && theta==0){return true;}
  return false;

}

int main()
{
  trainNN(
      "../reduceddatasets/reduceddataset_Cone4H1TopoParticleJets_forNN.root",
      "dummy",
      10000,
      200,
      10,
      10,
      0);
  return 0;
}


bool binIsFull(int binNumber,bool isX,double errValue,int nParticles,int numberBins)
{
  double interval=0;
  if (isX)
  {
    interval=errorHalfIntervalX(nParticles);
  }
  else
  {
    interval=errorHalfIntervalY(nParticles);
  }
  
  double min=-interval;
  double max=interval;

  int realBins=numberBins-2;//delete case of overflow / underflow

  double binSize=(max-min)/(double)realBins;

  if (errValue<min)
  {
    if (binNumber==0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  if (errValue>=max)
  {
    if (binNumber==numberBins-1)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  
  bool found=false;

  for (int u=0;u<realBins;u++)
  {
    double minInterval=min+u*binSize;
    double maxInterval=min+(u+1)*binSize;

    if (minInterval<=errValue && errValue<maxInterval)
    {
      if (binNumber==u+1)
      {
        return true;
      }
      else
      {
        found=true;
      }
    }
  }

  if (!found)
  {
    std::cout <<" Shouldn't arrive HERE !! errValue: " << errValue << std::endl;
  }
  
  return false;
  
}

  
  


void trainNN(TString inputfile,
             TString outputclass,
             int nIterations,
             int dilutionFactor,
             int nodesFirstLayer,
             int nodesSecondLayer,
             int restartTrainingFrom,
             int nParticlesTraining,
             bool useTrackEstimate,
             int numberBinsErrorEstimate,
             bool trainXerrors,
             int nPatternsPerUpdate,
             double learningRate,
             double learningRateDecrease,
             double learningRateMomentum) {

  double bweight=1;
  double cweight=1.;
  double lweight=1;



  gROOT->SetStyle("Plain");

  cout << "starting with settings: " << endl;
  cout << " nIterations: " << nIterations << endl;
  cout << " dilutionFactor: " << dilutionFactor << endl;
  cout << " nodesFirstLayer: " << nodesFirstLayer << endl;
  cout << " nodesSecondLayer: " << nodesSecondLayer << endl;
  
  
//  TFile *file= TFile::Open(inputfile);
 
//  TTree *simu = (TTree*)file->Get("Validation/NNinput");
  TChain *myChain = new TChain("Validation/NNinput");
 

if(!useTrackEstimate){
  #include "../files.txt"
}

if(useTrackEstimate){
  #include "../filesOnTrack.txt"
}
  TChain* simu=myChain;

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

  TString name;
  if(nParticlesTraining == 1 )   name+="WeightsOneTracks.root";
  if(nParticlesTraining == 2 )   name+="WeightsTwoTracks.root";
  if(nParticlesTraining == 3 )   name+="WeightsThreeTracks.root";

  if (!useTrackEstimate)
  {
    name.ReplaceAll(".root","_noTrack.root");
  }
  
  // getting a postion trained network from file
  TFile *_file0 = new TFile(name);
  TTrainedNetwork* positionTrainedNetwork=(TTrainedNetwork*)_file0->Get("TTrainedNetwork");
  
  cout << " Reading back network with minimum" << endl;
   

  TString filterTrain("Entry$%");
  filterTrain+=dilutionFactor;
  filterTrain+="==0";
  
  TString filterTest("Entry$%");
  filterTest+=dilutionFactor;
  filterTest+="==1";

  int* nneurons;
  int nlayer=3;
  
  simu->GetEntry(0);
  
  cout << "First entry..." << endl;
  
  Int_t           sizeX=-7; 
  Int_t           sizeY=-7; 
  
  
  // loop over the clusters loking for the first cluster properly saved 
  for( unsigned int clus =0; clus<NN_sizeX->size(); clus++ ){
    
    sizeX = (*NN_sizeX)[clus];
    sizeY = (*NN_sizeY)[clus];
    
    if(sizeX>0)break;
    
  }
  
  cout << "Size obtained" << endl;



  int numberinputs=sizeX*(sizeY+1)+4+nParticlesTraining*2;//add also position information
  if (!useTrackEstimate)
  {
    numberinputs=sizeX*(sizeY+1)+5+nParticlesTraining*2;
  }

  int numberoutputs=nParticlesTraining*numberBinsErrorEstimate;
  //2 for x and y
  //nParticlesTraining
  //numberBinsErrorEstimate

  if (nodesSecondLayer!=0)
  {
    nlayer=4;
  }

  if (nodesSecondLayer!=0)
  {
    nneurons=new int[4];
  }
  else
  {
    nneurons=new int[3];
  }
  
  nneurons[0]=numberinputs;
  
  nneurons[1]=nodesFirstLayer;

  if (nodesSecondLayer!=0)
  {
    nneurons[2]=nodesSecondLayer;
    nneurons[3]=numberoutputs;//number of output nodes
  }
  else
  {
    nneurons[2]=numberoutputs;//number of output nodes
  }

  for (int i=0;i<nlayer;i++)
  {
    cout << " layer i: " << i << " number neurons: " << nneurons[i] << endl;
  }
  

  //  float eventWeight(0);
  float trainingError(0);
  float testError(0);
  
  //setting learning parameters

  cout << " now providing training events " << endl;
  
  Long64_t numberTrainingEvents=0;
  Long64_t numberTestingEvents=0;
  int iClus=0;
  int part_0=0;
  int part_1=0;
  int part_2=0;
  int part_3=0;
  
  int nTotal=simu->GetEntries();
  
//  int nTotal=200;

  // Loop over entries:
  for (Int_t i = 0; i < nTotal; i++) {
    
    if (i % 1000000 == 0 ) {
      std::cout << " Counting training / testing events in sample. Looping over event " << i << std::endl;
    }
    
    simu->GetEntry(i);
    
    for( unsigned int clus =0; clus<NN_sizeX->size(); clus++ ){

      vector<float>  *matrixOfToT=0;
      vector<float>   *vectorOfPitchesY=0;
      
      Float_t         phiBS;
      Float_t         thetaBS;
      Float_t         etaModule;
      Int_t ClusterPixLayer;
      Int_t ClusterPixBarrelEC;
      
      std::vector<float> * positionX=0;
      std::vector<float> * positionY=0;
      std::vector<float> * thetaTr=0;
      std::vector<float> * phiTr=0;


      
      int   sizeX = (*NN_sizeX)[clus];      
      positionX =&(*NN_positionX)[clus];
      int nParticles = positionX->size();
      //cout << "------" << endl;
      if (nParticlesTraining!=nParticles) 
	{
	  continue;
	}
      if (isBadCluster(sizeX, nParticles ) )continue;
      
      thetaTr = &(*NN_theta)[clus];
      phiTr = &(*NN_phi)[clus];
      
      // loop over the particles;  
      for( unsigned int P = 0; P < positionX->size(); P++){
	double theta = (*thetaTr)[P];
	if (theta!=theta) continue;

        iClus++;
	
        if ( badTrackInfo(useTrackEstimate, theta ) )continue;



    
	
	if (iClus%dilutionFactor==0) numberTrainingEvents+=1;
	if (iClus%dilutionFactor==1) numberTestingEvents+=1;
	
	if (iClus%dilutionFactor==1 && nParticles==1 ) part_1++;
        if (iClus%dilutionFactor==1 && nParticles==2 ) part_2++;
        if (iClus%dilutionFactor==1 && nParticles==3 ) part_3++;
	
	
	
      }// end loop over th particles
    }// end loop over cluster
  }// end Loop over entries
  
   
  
  cout << " N. training events: " << numberTrainingEvents << 
      " N. testing events: " << numberTestingEvents << endl;
  
  cout << "now start to setup the network..." << endl;
  
  TJetNet* jn = new TJetNet( numberTestingEvents, numberTrainingEvents, nlayer, nneurons );

  cout <<  " setting learning method... " << endl;

   
  jn->SetPatternsPerUpdate( nPatternsPerUpdate );
  jn->SetUpdatesPerEpoch( (int)std::floor((float)numberTrainingEvents/(float)nPatternsPerUpdate) );
  jn->SetUpdatingProcedure( 0 );
  jn->SetErrorMeasure( 0 );
  jn->SetActivationFunction( 1 );
  jn->SetLearningRate( learningRate );//0.8
  jn->SetMomentum( learningRateMomentum );//0.3 //is now 0.5
  jn->SetInitialWeightsWidth( 1. );
  jn->SetLearningRateDecrease( learningRateDecrease );//0.992


  //jn->SetActivationFunction(4,nlayer-1-1); This is to have linear OUTPUT.
  

  TH1F* histoControlTestX=new TH1F("histoControlTestX","histoControlTestX",numberBinsErrorEstimate,0,numberBinsErrorEstimate);
  TH1F* histoControlTestY=new TH1F("histoControlTestY","histoControlTestY",numberBinsErrorEstimate,0,numberBinsErrorEstimate);
  

  int trainSampleNumber=0;
  int testSampleNumber=1;
  
  
  
  cout << " copying over training events " << endl;
  int counter=0; 
  int counter0=0;
  int counter1=0;
  
  iClus=0;


  for (Int_t i = 0; i < nTotal; i++) {
    
    if (i % 1000 == 0 ) {
      std::cout << " Copying over training events. Looping over event " << i << std::endl;
    }
    
    simu->GetEntry(i);
    // loop over clusters
    for( unsigned int clus =0; clus<NN_sizeX->size(); clus++ ){
      
      vector<float>  *matrixOfToT=0;
      vector<float>   *vectorOfPitchesY=0;
      
      Float_t         phiBS;
      Float_t         thetaBS;
      Float_t         etaModule;
      Int_t ClusterPixLayer;
      Int_t ClusterPixBarrelEC;
      
      std::vector<float> * positionX=0;
      std::vector<float> * positionY=0;

      std::vector<float> positionX_reorder;
      std::vector<float> positionY_reorder;




      std::vector<float> * thetaTr=0;
      std::vector<float> * phiTr=0;

      double localColumnWeightedPosition;// = 0;
      double localRowWeightedPosition;// = 0;

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

      positionX_reorder=*positionX;
      positionY_reorder.clear();


      
      thetaTr = &(*NN_theta)[clus];
      phiTr = &(*NN_phi)[clus];
      

      localColumnWeightedPosition =(*NN_localColumnWeightedPosition)[clus];
      localRowWeightedPosition    =(*NN_localRowWeightedPosition)[clus];
      


      int nParticles = positionX->size();
      if (nParticlesTraining!=nParticles) 
	{
	  continue;
	}
      
      if(isBadCluster(sizeX, nParticles ) )continue;
      

      


      
      std::sort(positionX_reorder.begin(),
                positionX_reorder.end());

      for (int o=0;o<positionX->size();o++)
      {
        double corry=-1000;
        for (int e=0;e<positionX->size();e++)
        {
          if (fabs(positionX_reorder[o]-(*positionX)[e])<1e-10)
          {
            if (fabs(corry+1000)>1e-6)
            {
              cout << " Value find more than once! " << endl;
              for (int p=0;p<positionX->size();p++)
              {
                cout << " X n. : " << p << " is: " << (*positionX)[p] << endl;
              }
            }
            corry=(*positionY)[e];
          }
        }
        positionY_reorder.push_back(corry);
      }
      


      for( unsigned int P = 0; P < positionX->size(); P++){
	

	double	theta = (*thetaTr)[P];
	double	phi   = (*phiTr)[P]; 

	if (theta!=theta) continue;
	
	if (ClusterPixBarrelEC==2)
        {
          theta=-theta;
          phi=-phi;
        }
        

	iClus++;	 

	if ( badTrackInfo(useTrackEstimate, theta ) )continue;

	// formatting cluster infos for the NN input 
	
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


	vector<double> outputNN_idX;
	vector<double> outputNN_idY;

        
	vector<double> resultNN = positionTrainedNetwork->calculateOutputValues(inputData);


	

	// storing the obtained X and Y position in vectors of position // nParticlesTraining

	if(nParticlesTraining==1){  
 	  outputNN_idX.push_back(back_posX(resultNN[0]));
 	  outputNN_idY.push_back(back_posY(resultNN[1]));  
	}


 	if(nParticlesTraining==2){  
 	  outputNN_idX.push_back(back_posX(resultNN[0]));
 	  outputNN_idX.push_back(back_posX(resultNN[2]));
 	  outputNN_idY.push_back(back_posY(resultNN[1]));
 	  outputNN_idY.push_back(back_posY(resultNN[3]));
 	}

 	if(nParticlesTraining==3){  
 	  outputNN_idX.push_back(back_posX(resultNN[0]));
 	  outputNN_idX.push_back(back_posX(resultNN[2]));
 	  outputNN_idX.push_back(back_posX(resultNN[4]));
 	  outputNN_idY.push_back(back_posY(resultNN[1]));
 	  outputNN_idY.push_back(back_posY(resultNN[3]));
 	  outputNN_idY.push_back(back_posY(resultNN[5]));
 	}



	// HERE WE NEED THE CONVERSION OF THE POSITION RESPECT WITH CLUSTER CENTER FROM LOCAL POSITION!!!
	vector<float> outputNN_X ; 
	vector<float> outputNN_Y ;

	for(unsigned int t=0; t < outputNN_idX.size(); t++){

	  double PitchX=0.05; //50 micron pitch in Y 

	  // local position respect with the cluster center    questa e' la posizione locale corrispondente al centro del cluster
	  // defined as the center of matrixOfToT              (definito come centro della matrice per la rete neuronale)
	  double centerPosY = localColumnWeightedPosition; 
	  double centerPosX = localRowWeightedPosition;  
//	  cout << sizeX << " " << "centerPosX: " << centerPosX << endl;
	  //questi sono gli unput che la rete restituisce (coordinate rispetto al centro)
	  // output of the NN (respect with the cluster center
	  double indexX =  outputNN_idX[t];
	  double indexY =  outputNN_idY[t];
	  double indexPositionToTranslateY = indexY+(double)(sizeY-1)/2;
          //cout << indexX << " " << centerPosX << endl;
	  double pos_idX = centerPosX + (double)indexX * PitchX;
//	  cout << " pos_idX = " << pos_idX << endl;

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
		  //		  cout << "positionYFromZero: " << positionYFromZero << endl;
		}
	      if (i==(sizeY-1)/2)
		{
		  positionCenterYFromZero =  actualPositionFromZero + 0.5* (*vectorOfPitchesY)[i];
		  //		  cout << "positionCenterYFromZero: " << positionCenterYFromZero << endl;
		}
	      actualPositionFromZero+=(*vectorOfPitchesY)[i];
	    }
	  
	  double pos_idY = centerPosY + positionYFromZero - positionCenterYFromZero;
//          cout << " pos_idY " << pos_idY << endl;


	  // 
	  outputNN_X.push_back(pos_idX);
	  outputNN_Y.push_back(pos_idY);
	} 
	      
     
  
      
	if (matrixOfToT->size()!=sizeX*sizeY)
	  {
	    std::cout << " Event: " << i << " PROBLEM: size Y is: " << matrixOfToT->size() << std::endl;
	    throw std::runtime_error("Error in errors/trainNN.cxx");
	  }
	
	// loop over elements of matrixOfTot which is actually a vector
	for(unsigned int ME =0; ME < matrixOfToT->size(); ME++){
	  
	  if (iClus%dilutionFactor==0)	jn->SetInputTrainSet( counter0, ME, norm_ToT((*matrixOfToT)[ME]));
	  if (iClus%dilutionFactor==1)	jn->SetInputTestSet( counter1, ME, norm_ToT((*matrixOfToT)[ME]));
	  
	  if (counter1 == 0) std::cout << " element: " << ME <<  " ToT set to: " << norm_ToT((*matrixOfToT)[ME]) << std::endl;
	  
	}     
      
	
	// loop over vector of pitches
	for (int s=0;s<sizeY;s++)
	{
	  if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, sizeX*sizeY+s, norm_pitch((*vectorOfPitchesY)[s]));
	  if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, sizeX*sizeY+s, norm_pitch((*vectorOfPitchesY)[s]));
	  
	  if (counter0 == 0) std::cout <<  " s: " << s << " pitch set to: " << norm_pitch((*vectorOfPitchesY)[s]) << std::endl;
	}
	
	if (iClus%dilutionFactor==0)      jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY, norm_layerNumber(ClusterPixLayer));
	if (iClus%dilutionFactor==0)      jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+1, norm_layerType(ClusterPixBarrelEC));
	
	if (iClus%dilutionFactor==1)      jn->SetInputTestSet( counter1, (sizeX+1)*sizeY, norm_layerNumber(ClusterPixLayer));
	if (iClus%dilutionFactor==1)      jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+1, norm_layerType(ClusterPixBarrelEC));
	
	
      
	if (counter0 == 0) std::cout << " ClusterPixLayer " << norm_layerNumber(ClusterPixLayer) << " ClusterPixBarrelEC " << norm_layerType(ClusterPixBarrelEC) << std::endl;
	
	if (useTrackEstimate)
	  {
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+2, norm_phi(phi) );
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+3, norm_theta(theta) );
	    
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+2, norm_phi(phi) );
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+3, norm_theta(theta) );
	    
	    if (counter0==0) std::cout << " phi " << norm_phi(phi) << " theta: " << norm_theta(theta) << std::endl;
	    
	  }
	else
	  {
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+2, norm_phiBS(phiBS) );
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+3, norm_thetaBS(thetaBS) );
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+4, norm_etaModule(etaModule) );
	    
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+2, norm_phiBS(phiBS) );
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+3, norm_thetaBS(thetaBS) );
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+4, norm_etaModule(etaModule) );
	    
	    
	    if (counter0==0) std::cout << 
	      " phiBS " << norm_phiBS(phiBS) << 
	      " thetaBS: " << norm_thetaBS(thetaBS) << 
	      " etaModule: " << norm_etaModule(etaModule) << std::endl;
	  }
	
	int addNumber=5;
	if (useTrackEstimate) addNumber=4;
	
	

	for (int o=0;o<nParticlesTraining;o++)
	  {
	    if (iClus%dilutionFactor==0) jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+addNumber+2*o,norm_posX(outputNN_idX[o]) );
	    if (iClus%dilutionFactor==0) jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+addNumber+2*o+1,norm_posY(outputNN_idY[o]) );
	    
	    if (iClus%dilutionFactor==1) jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+addNumber+2*o,norm_posX(outputNN_idX[o]) );
	    if (iClus%dilutionFactor==1) jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+addNumber+2*o+1,norm_posY(outputNN_idY[o]) );


	    	    
            if (counter==0) std::cout << " n. " << o << 
                " posX: " << norm_posX((outputNN_idX)[o])  <<
                " posY: " << norm_posY((outputNN_idY)[o]) << std::endl;
	    
	  }


        
//        cout << " nParticlesTraining " << nParticlesTraining << endl;


	for (int r=0;r<nParticlesTraining;r++)
	  {

//        cout << " numberBinsErrorEstimate " << numberBinsErrorEstimate << endl;
	    //	    if (counter==0) std::cout << " particle: " << r << std::endl;
	    
	    for (int u=0;u<numberBinsErrorEstimate;u++)
	      {
		if (trainXerrors)
		  {
//                    cout << " outputNN_X["<<r<<"]="<<outputNN_X[r] << 
//                        " positionX_reorder["<<r<<"] "<< positionX_reorder[r] << endl;
		    bool full=binIsFull(u,true,(outputNN_X[r]-positionX_reorder[r]),nParticlesTraining,numberBinsErrorEstimate);
		    int nValueFull=0;
		    if (full) nValueFull=1;
		    if (iClus%dilutionFactor==0)    jn->SetOutputTrainSet(counter0, r*numberBinsErrorEstimate+u, nValueFull);
		    if (iClus%dilutionFactor==1)    jn->SetOutputTestSet(counter1, r*numberBinsErrorEstimate+u, nValueFull);
		    
                    if (counter==0) std::cout << " X bin: " << u << " gl: "<< r*2*numberBinsErrorEstimate+u << " val: " << nValueFull;
		    histoControlTestX->Fill(u+1,nValueFull);
		  }
	      }
	    for (int u=0;u<numberBinsErrorEstimate;u++)
	      {
		if (!trainXerrors)
		  {
//                    cout << " outputNN_Y["<<r<<"]="<<outputNN_Y[r] << 
//                        " positionY_reorder["<<r<<"] "<< positionY_reorder[r] << endl;

		    bool full=binIsFull(u,false,(outputNN_Y[r]-positionY_reorder[r]),nParticlesTraining,numberBinsErrorEstimate);
		    int nValueFull=0;
		    if (full) nValueFull=1;
		    if (iClus%dilutionFactor==0)   jn->SetOutputTrainSet(counter0, r*numberBinsErrorEstimate+u, nValueFull);
		    if (iClus%dilutionFactor==1)   jn->SetOutputTestSet(counter1, r*numberBinsErrorEstimate+u, nValueFull);
		    
		    if (counter0==0) std::cout << " Y bin: " << u << " gl: " << r*2*numberBinsErrorEstimate+numberBinsErrorEstimate+u << " val: " << nValueFull;
		    if (iClus%dilutionFactor==0) 	 histoControlTestY->Fill(u+1,nValueFull);
		  }
		
	      }
	  }
	
	if (counter0==0) std::cout << std::endl << " total number of bins: " << numberoutputs << std::endl;
	
	
	
	if (iClus%dilutionFactor==0)  jn->SetEventWeightTrainSet(  counter0, 1 );
	if (iClus%dilutionFactor==1)  jn->SetEventWeightTestSet(  counter1, 1 );
	
	counter+=1;
	
	if (iClus%dilutionFactor==0) counter0+=1;
	if (iClus%dilutionFactor==1) counter1+=1;
	
	
      }
    }
  }
 
  if (counter0!=numberTrainingEvents)
    {
      cout << " counter up to: " << counter0 << " while events in training sample are " << numberTrainingEvents << endl;
           return;
    }

  if (counter1!=numberTestingEvents)
    {
      cout << " counter up to: " << counter1 << " while events in testing sample are " << numberTestingEvents << endl;
         return;
    }



 
  
  
  
 
  jn->Shuffle(true,false);
  
  std::cout << " Potts units are: " << jn->GetPottsUnits() << std::endl; 


  cout << " setting pattern for training events " << endl;
  
  if (restartTrainingFrom==0)
    {
      jn->Init();
    }
  else
    {
      TString name("Weights");
      name+=restartTrainingFrom;
      name+=".w";
      
      jn->ReadFromFile(name);
    }
  
  
  
  float minimumError=1e10;
  int epochesWithRisingError=0;
  int epochWithMinimum=0;
  
  int updatesPerEpoch=jn->GetUpdatesPerEpoch();
  
  TString directory("weights");
  directory+="_nPat";
  directory+=nPatternsPerUpdate;
  directory+="_rate";
  directory+=(int)(learningRate*100);
  directory+="_rdec";
  directory+=(int)(100*learningRateDecrease);
  directory+="_mom";
  directory+=(int)(100*learningRateMomentum);
  directory+="_";
  directory+=nParticlesTraining;
  directory+="_";
  if (trainXerrors)
  {
    directory+="X";
  }
  else
  {
    directory+="Y";
  }
  
  if (useTrackEstimate)
  {
    directory+="_withTracks";
  }
  
  TString command("mkdir ");
  command+=directory;

  gSystem->Exec(command);

  TString nameCronology=directory;
  
  nameCronology+="/trainingCronology.txt";

  //prepare output stream

  ofstream cronology(nameCronology,ios_base::out);//|ios_base::app);
  
  cronology << "-------------SETTINGS----------------" << endl;
  cronology << "Epochs: " << jn->GetEpochs() << std::endl;
  cronology << "Updates Per Epoch: " << jn->GetUpdatesPerEpoch() << std::endl;
  cronology << "Updating Procedure: " << jn->GetUpdatingProcedure() << std::endl;
  cronology << "Error Measure: " << jn->GetErrorMeasure() << std::endl;
  cronology << "Patterns Per Update: " << jn->GetPatternsPerUpdate() << std::endl;
  cronology << "Learning Rate: " << jn->GetLearningRate() << std::endl;
  cronology << "Momentum: " << jn->GetMomentum() << std::endl;
  cronology << "Initial Weights Width: " << jn->GetInitialWeightsWidth() << std::endl;
  cronology << "Learning Rate Decrease: " << jn->GetLearningRateDecrease() << std::endl;
  cronology << "Activation Function: " << jn->GetActivationFunction() << std::endl;
  cronology << "-------------LAYOUT------------------" << endl;
  cronology << "Input variables: " << jn->GetInputDim() << endl;
  cronology << "Output variables: " << jn->GetOutputDim() << endl;
  cronology << "Hidden layers: " << jn->GetHiddenLayerDim() << endl;
  cronology << "Layout : ";
  for (Int_t s=0;s<jn->GetHiddenLayerDim()+2;++s)
  {
    cronology << jn->GetHiddenLayerSize(s);
    if (s<jn->GetHiddenLayerDim()+1) cronology << "-";
  }
  cronology << endl;
  cronology << "--------------HISTORY-----------------" << endl;
  cronology << "History of iterations: " << endl;
  cronology.close();

  //prepare training histo
  TH1F* histoTraining=new TH1F("training","training",(int)std::floor((float)nIterations/10.+0.5),1,std::floor((float)nIterations/10.+1.5));
  TH1F* histoTesting=new TH1F("testing","testing",(int)std::floor((float)nIterations/10.+0.5),1,std::floor((float)nIterations/10.+1.5));

  double maximumTrain=0;
  double minimumTrain=1e10;

  for(int epoch=restartTrainingFrom+1;epoch<=nIterations;++epoch)
  {
    if (epoch!=restartTrainingFrom+1)
    {
      trainingError = jn->Train();
    }

    if (epoch%10==0 || epoch==restartTrainingFrom+1)
    {

      cronology.open(nameCronology,ios_base::app);

      testError = jn->Test();

      if (trainingError>maximumTrain) maximumTrain=trainingError;
      if (testError>maximumTrain) maximumTrain=testError;
      if (trainingError<minimumTrain) minimumTrain=trainingError;
      if (testError<minimumTrain) minimumTrain=testError;

      
      histoTraining->Fill(epoch/10.,trainingError);
      histoTesting->Fill(epoch/10.,testError);

      if (testError<minimumError)
      {
        minimumError=testError;
        epochesWithRisingError=0;
        epochWithMinimum=epoch;
      }
      else
      {
        epochesWithRisingError+=10;

      }
      
      
      if (epochesWithRisingError>300)
      {
	if (trainingError<minimumError)
	{
	  cout << " End of training. Minimum already on epoch: " << epochWithMinimum << endl;
          cronology << " End of training. Minimum already on epoch: " << epochWithMinimum << endl;
	  break;
	} 
      }
      
      cronology << "Epoch: [" << epoch <<
          "] Error: " << trainingError << 
          " Test: " << testError << endl;

      cout << "Epoch: [" << epoch <<
	"] Error: " << trainingError << 
	" Test: " << testError << endl;

      cronology.close();
        
      TString name=directory;
      name+="/Weights";
      name+=epoch;
      name+=".root";

      TFile* file=new TFile(name,"recreate");
      TTrainedNetwork* trainedNetwork=jn->createTrainedNetwork();

     

      trainedNetwork->Write();
      histoControlTestX->Write();
      histoControlTestY->Write();
      file->Write();
      file->Close();
      delete file;



      //      jn->DumpToFile(name);
    }
  }
      
  jn->writeNetworkInfo(1);
  jn->writeNetworkInfo(2);
 



  ////////////WWWWWAAAAASSSSSS HERE
  Int_t nInput=jn->GetInputDim();
  
  cout << " create Trained Network object..." << endl;
  
  TTrainedNetwork* trainedNetwork=jn->createTrainedNetwork();



  cout << " Now getting histograms from trainingResult" << endl;
  cronology << " Now getting histograms from trainingResult" << endl;

  TNetworkToHistoTool myHistoTool;

  cout << " From network to histo..." << endl;
  std::vector<TH1*> myHistos=myHistoTool.fromTrainedNetworkToHisto(trainedNetwork);

  cout << " From histo to network back..." << endl;
  TTrainedNetwork* trainedNetwork2=myHistoTool.fromHistoToTrainedNetwork(myHistos);
  
  cout << " reading back " << endl;
  jn->readBackTrainedNetwork(trainedNetwork2);
   

  


  if (epochWithMinimum!=0)
  {
    cronology << "Minimum stored from Epoch: " << epochWithMinimum << endl;
  }  else
  {
    cronology << "Minimum not reached" << endl;
  }

  cronology.close();

  if (epochWithMinimum!=0)
  {
    
      TString name=directory;
      name+="/Weights";
      name+=epochWithMinimum;
      name+=".root";
      std::cout << " reading back from minimum " << endl;


      TFile *_file0 = new TFile(name);
      TTrainedNetwork* trainedNetwork=(TTrainedNetwork*)_file0->Get("TTrainedNetwork");

      cout << " Reading back network with minimum" << endl;
      jn->readBackTrainedNetwork(trainedNetwork);

      TString nameFile=directory;
      nameFile+="/weightMinimum.root";

      TFile* file=new TFile(nameFile,"recreate");
      trainedNetwork->Write();
      file->Write();
      file->Close();
      delete file;

      cout << " -------------------- " << endl;
      cout << " Writing OUTPUT histos " << endl;
      TString histoFName=directory;
      histoFName+="/histoWeights.root";

      TFile* fileHistos=new TFile(histoFName,"recreate");
      TNetworkToHistoTool histoTool;
      std::vector<TH1*> myHistos=histoTool.fromTrainedNetworkToHisto(trainedNetwork);
      std::vector<TH1*>::const_iterator histoBegin=myHistos.begin();
      std::vector<TH1*>::const_iterator histoEnd=myHistos.end();
      for (std::vector<TH1*>::const_iterator histoIter=histoBegin;
           histoIter!=histoEnd;++histoIter)
      {
        (*histoIter)->Write();
      }
      fileHistos->Write();
      fileHistos->Close();
      delete fileHistos;



  } 
  else
  {
    cout << " using network at last iteration (minimum not reached..." << endl;
  }
  
  //here you should create the class... Still open how to deal with this...
  //  char* myname=const_cast<char*>(static_cast<const char*>(outputclass));
  //  ierr=mlpsavecf_(myname);
 
  TString histoTName=directory;
  histoTName+="/trainingInfo.root";

  TFile* histoFile=new TFile(histoTName,"recreate");
  histoTraining->Write();
  histoTesting->Write();
  histoFile->Write();
  histoFile->Close();
  delete histoFile;

  TCanvas* trainingCanvas=new TCanvas("trainingCanvas","trainingCanvas");
  histoTraining->SetLineColor(2);
  histoTesting->SetLineColor(4);

  histoTraining->GetYaxis()->SetRangeUser(minimumTrain,maximumTrain);
  histoTraining->Draw("l");
  histoTesting->Draw("lsame");
  TString canvasName=directory;
  canvasName+="/trainingCurve.eps";
  trainingCanvas->SaveAs(canvasName);

 
  TCanvas* mlpa_canvas = new TCanvas("jetnet_canvas","Network analysis");
  mlpa_canvas->Divide(2,4);


  
}

