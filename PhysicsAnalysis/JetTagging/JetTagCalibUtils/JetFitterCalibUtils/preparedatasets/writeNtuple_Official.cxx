/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "readBaseBTagAnaTree.h"
#include "readJFBTagAna.h"
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include "writeNtuple_Official.h"
#include "getPtEtaCategoryLikelihood.h"
#include "TRandom.h"
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

struct number_pair
{
  int first;
  double second;
  number_pair(int p1, double p2)
    : first (p1), second (p2) {}
  bool operator< (const number_pair& other) const
  { return second > other.second; }
};

struct allinfos
{
  Int_t nVTX;
  Int_t nTracksAtVtx;
  Int_t nSingleTracks;
  Double_t energyFraction;
  Double_t mass;
  Double_t significance3d;

  Int_t cat_flavour;
  Int_t bottom;
  Int_t charm;
  Int_t light;

  Double_t discriminatorIP2D;
  Double_t discriminatorIP3D;
  Double_t discriminatorSV1;
  Double_t discriminatorCOMB;
 
  Double_t weight;
 
  Double_t deltaR;
  Double_t JetPt;
  Double_t JetEta;
  Int_t cat_pT;
  Int_t cat_eta;
};




void writeNtuple_Official(TString inputFileName,
                          TString jetCollectionName,
                          TString suffix,
                          bool forNN,
                          bool randomize) 
{
  

  TString jetCollection=jetCollectionName+suffix;

  cout << " opening input file: " << inputFileName << endl << " processing to obtain: " << 
      jetCollection << " root file "  << endl;
  
  TFile inputFile(inputFileName);


  TString baseBTag("BTag_");
  TString suffixIP2D("_IP2D/PerfTreeAll");
  TString suffixIP3D("_IP3D/PerfTreeAll");
  TString suffixSV1("_SV1/PerfTreeAll");
  TString suffixCOMB("_COMB/PerfTreeAll");
  TString suffixJF("_JetFitterTag/PerfTreeAll");

  cout << "instantiating IP2D " << endl;
  TTree* treeIP2D=(TTree*)inputFile.Get(baseBTag+jetCollection+suffixIP2D);
  if (treeIP2D==0) throw std::string("Problem IP2D");
  readBaseBTagAnaTree* readTreeIP2D= new readBaseBTagAnaTree(treeIP2D);
  
  cout << "instantiating IP3D " << endl;
  TTree* treeIP3D=(TTree*)inputFile.Get(baseBTag+jetCollection+suffixIP3D);
  if (treeIP3D==0) throw std::string("Problem IP3D");
  readBaseBTagAnaTree* readTreeIP3D= new readBaseBTagAnaTree(treeIP3D);

  cout << "instantiating SV1 " << endl;
  TTree* treeSV1=(TTree*)inputFile.Get(baseBTag+jetCollection+suffixSV1);
  if (treeSV1==0) throw std::string("Problem SV1");
  readBaseBTagAnaTree* readTreeSV1= new readBaseBTagAnaTree(treeSV1);

  cout << "instantiating COMB " << endl;
  TTree* treeCOMB=(TTree*)inputFile.Get(baseBTag+jetCollection+suffixCOMB);
  if (treeCOMB==0) throw std::string("Problem COMB");
  readBaseBTagAnaTree* readTreeCOMB= new readBaseBTagAnaTree(treeCOMB);

  cout << "instantiating JetFitterTag " << endl;
  TTree* treeJF=(TTree*)inputFile.Get(baseBTag+jetCollection+suffixJF);
  if (treeJF==0) throw std::string("Problem JF");
  readJFBTagAna* readTreeJF=new readJFBTagAna(treeJF);
  

  Int_t nVTX;
  Int_t nTracksAtVtx;
  Int_t nSingleTracks;
  Double_t energyFraction;
  Double_t mass;
  Double_t significance3d;

  Int_t cat_flavour;
  Int_t bottom;
  Int_t charm;
  Int_t light;

  Double_t discriminatorIP2D;
  Double_t discriminatorIP3D;
  Double_t discriminatorSV1;
  Double_t discriminatorCOMB;
 
  Double_t weight;
 
  Double_t deltaR;
  Double_t JetPt;
  Double_t JetEta;
  Int_t cat_pT;
  Int_t cat_eta;

  //for the NN you need to get the number of b,c or light jets

  Int_t num_entries=readTreeJF->fChain->GetEntries();

  int numberb=0;
  int numberc=0;
  int numberl=0;

  if (forNN) 
  {

    for (Long64_t i=0;i<num_entries;i++) {
      
      readTreeJF->GetEntry(i);
      
      if (readTreeJF->mass>-100)
      {
        if (abs(readTreeJF->Flavour)==5){
          numberb+=1;
        }
        if (abs(readTreeJF->Flavour)==4){
          numberc+=1;
        }
        if (abs(readTreeJF->Flavour==1)){
          numberl+=1;
        }
      }
    }
  }
  
  //now you have to calculate the weights...
  //(store them in a matrix for b,c or light jets...

  cout << " number of b found : " << numberb << " c: " << numberc << " l: " << numberl << endl;

  double correctionfactor=1;

  
  int numPtBins=getNPtBins();
  int numEtaBins=getNEtaBins();

  Double_t* weightsb=0;
  Double_t* weightsl=0;
  Double_t* weightsc=0;

  Double_t* countb=0;
  Double_t* countl=0;
  Double_t* countc=0;

  Double_t toleranceb=1;
  Double_t tolerancec=1;
  Double_t tolerancel=1;
  TRandom random;


  Double_t maxweightb=0;
  Double_t maxweightl=0;
  Double_t maxweightc=0;
  
  
  if (forNN) 
  {
    
    weightsb=new Double_t[numPtBins*numEtaBins];
    weightsl=new Double_t[numPtBins*numEtaBins];
    weightsc=new Double_t[numPtBins*numEtaBins];

    countb=new Double_t[numPtBins*numEtaBins];
    countl=new Double_t[numPtBins*numEtaBins];
    countc=new Double_t[numPtBins*numEtaBins];

    for (int i=0;i<numPtBins*numEtaBins;i++){
      weightsb[i]=0;
      weightsl[i]=0;
      weightsc[i]=0;
      countb[i]=0;
      countl[i]=0;
      countc[i]=0;
    }
    
    for (Long64_t i=0;i<num_entries;i++) {
      
      readTreeJF->GetEntry(i);
      
      if (readTreeJF->mass<-100) continue;
      
      if (fabs(readTreeJF->JetEta)>2.5||readTreeJF->JetPt<=15.)  continue;

      
      pair<int,double> ptInfo=getPtCategory(readTreeJF->JetPt);
      pair<int,double> etaInfo=getEtaCategory(readTreeJF->JetEta);

      int actualpT=ptInfo.first;
      int actualeta=etaInfo.first;
      
      int flavour=abs(readTreeJF->Flavour);

//      cout << " actualpT " << actualpT << " actualeta " << actualeta << endl;
      
      switch (flavour)
      {
      case 5:
        countb[actualpT+numPtBins*actualeta]+=1./(ptInfo.second*etaInfo.second);
        break;
      case 4:
        countc[actualpT+numPtBins*actualeta]+=1./(ptInfo.second*etaInfo.second);
        break;
      case 1:
        countl[actualpT+numPtBins*actualeta]+=1./(ptInfo.second*etaInfo.second);
        break;
      }

    }
    
      
    for (int i=0;i<numPtBins*numEtaBins;i++){
//      cout << " for i=ptxeta = " << i << " weightsb[i] " << weightsb[i]
//           << " weightsl[i] " << weightsl[i] << " weightsc[i] " << weightsc[i] << endl;
      weightsb[i]= (Double_t)numberb / (Double_t)countb[i] ;
      weightsl[i]= (Double_t)numberl / (Double_t)countl[i] ;
      weightsc[i]= (Double_t)numberc / (Double_t)countc[i];

      if (weightsb[i]>maxweightb) maxweightb=weightsb[i];
      if (weightsl[i]>maxweightl) maxweightl=weightsl[i];
      if (weightsc[i]>maxweightc) maxweightc=weightsc[i];

    }
  
  }
  

  cout << " maxweightb: " << maxweightb << " maxweightc: " << maxweightc << 
      " maxweightl: " << maxweightl << endl;

  TFile* file=0;
  
  TString filename("../reduceddatasets/reduceddataset_");
  filename+=jetCollectionName;
  if (forNN)
  {
    filename+="_forNN";
  }
  filename+=".root";

  file=new TFile(filename,"recreate");
  
  TTree* myTree=new TTree("SVTree","SVTree");

  
  myTree->Branch("nVTX",&nVTX,"nVTX/I");
  myTree->Branch("nTracksAtVtx",&nTracksAtVtx,"nTracksAtVtx/I");
  myTree->Branch("nSingleTracks",&nSingleTracks,"nSingleTracks/I");
  myTree->Branch("energyFraction",&energyFraction,"energyFraction/D");
  myTree->Branch("mass",&mass,"mass/D");
  myTree->Branch("significance3d",&significance3d,"significance3d/D");

  
  if (forNN)
  {
    myTree->Branch("cat_pT",&cat_pT,"cat_pT/I");
    myTree->Branch("cat_eta",&cat_eta,"cat_eta/I");
    myTree->Branch("weight",&weight,"weight/D");
    myTree->Branch("discriminatorIP3D",&discriminatorIP3D,"discriminatorIP3D/D");
    myTree->Branch("bottom",&bottom,"bottom/I");
    myTree->Branch("charm",&charm,"charm/I");
    myTree->Branch("light",&light,"light/I");
  }
  


  if (!forNN)
  {
    myTree->Branch("cat_flavour",&cat_flavour,"cat_flavour/I");
//    myTree->Branch("deltaR",&deltaR,"deltaR/D");    
//    myTree->Branch("JetPt",&JetPt,"JetPt/D");
//    myTree->Branch("JetEta",&JetEta,"JetEta/D");
//    myTree->Branch("discriminatorIP2D",&discriminatorIP2D,"discriminatorIP2D/D");
//    myTree->Branch("discriminatorSV1",&discriminatorSV1,"discriminatorSV1/D");
//    myTree->Branch("discriminatorCOMB",&discriminatorCOMB,"discriminatorCOMB/D");
  }
  

  


//  if (num_entries!=readTreeCOMB->fChain->GetEntries()) {
//    throw std::string("event number doesn't match");
//  }

  //now randomize entries

/*
  std::vector<int> inputvalues;
  
  for (Int_t i=0;i<num_entries;i++) 
  {
    inputvalues.push_back(i);
  }
  
  std::vector<int> outputvalues;
  int counter=0;
  do
  {

    int size=inputvalues.size();
    if (size==0)
    {
      break;
    }
    double randomNumber=random.Uniform(0,size);
    int nToTransfer=(int)(std::floor(randomNumber<size?randomNumber:0)+0.5);
    vector<int>::iterator begin=inputvalues.begin();
    begin+=nToTransfer;
    outputvalues.push_back(inputvalues[nToTransfer]);
    if (counter%100000==0)
    {
    cout << " Processing randomizing event: " << counter << 
        " inputvalue " << inputvalues[nToTransfer] <<  endl;
    }
    
    inputvalues.erase(begin);
    counter+=1;


  }
  while (true);
*/

  std::vector<number_pair> outputvalues;
  
  for (Int_t i=0;i<num_entries;i++) 
  {
    outputvalues.push_back(number_pair(i,random.Uniform()));
  }
  
  if (randomize)
  {
    
    cout << " Doing sorting... " << endl;
    std::sort (outputvalues.begin(), outputvalues.end());
    cout << " End sorting ... " << endl;
  }
  

  cout << "Total entries are: " << num_entries << endl;
  Int_t i=0;

  
  vector<number_pair>::const_iterator begin=outputvalues.begin();
  vector<number_pair>::const_iterator end=outputvalues.end();
  
  Int_t counter=0;
  for (vector<number_pair>::const_iterator iter=begin;iter!=end;++iter)
  {
    i=(*iter).first;

    //take only every fifth data point
    if (!forNN)
    {
      if (counter%5 != 0)
      {
        counter+=1;
        continue;
      }
    }
    

//    if (counter>1000000)
//      break;
//    }
    

    if (counter % 500000 == 0 ) {
      std::cout << " processing event number " << counter << " data event: " << i << " which was event n. " << std::endl;
    }
    
    counter+=1;
     
    
    readTreeJF->GetEntry(i);


    if (fabs(readTreeJF->JetEta)<2.5&&readTreeJF->JetPt>15.&&readTreeJF->mass>-100) {


      JetPt=readTreeJF->JetPt;
      JetEta=readTreeJF->JetEta;
      cat_pT=getPtCategory(JetPt).first;
      cat_eta=getEtaCategory(JetEta).first;
      
      cat_flavour=abs(readTreeJF->Flavour);
      if (forNN)
      {
        bottom=0;
        charm=0;
        light=0;

        bool throwevent(false);

        switch (cat_flavour)
        {
        case 5:
          bottom=1;
          weight=weightsb[cat_pT+numPtBins*cat_eta];

          if (forNN)
          {
            
            if (weight<maxweightb/toleranceb)
            {
              if (random.Uniform()>weight*toleranceb/maxweightb)
              {
                throwevent=true;
              }
              weight=1.;//maxweightb/toleranceb;
            }
            else
            {
              weight/=(maxweightb/toleranceb);
            }
          } 
            
          
          break;
        case 4:
          charm=1;
          weight=weightsc[cat_pT+numPtBins*cat_eta];

          if (forNN)
          {
            if (weight<maxweightc/tolerancec)
            {
              if (random.Uniform()>weight*tolerancec/maxweightc)
              {
                throwevent=true;
              }
              weight=1.;//maxweightc/tolerancec;
            }
            else
            {
              weight/=(maxweightc/tolerancec);
            }
          }
          


          break;
        case 1:
          light=1;
          weight=weightsl[cat_pT+numPtBins*cat_eta];

          if (forNN)
          {
            
            if (weight<maxweightl/tolerancel)
            {
              if (random.Uniform()>weight*tolerancel/maxweightl)
              {
                throwevent=true;
              }
              weight=1.;//maxweightl/tolerancel;
            }
            else 
            {
              weight/=(maxweightl/tolerancel);
            }
            
          }
          
          break;
        }

        if (throwevent) continue;

      }
      

 

      //read the others only on demand (faster)
      readTreeIP2D->GetEntry(i);
      readTreeIP3D->GetEntry(i);
      readTreeSV1->GetEntry(i);
      readTreeCOMB->GetEntry(i);

      discriminatorIP3D=readTreeIP3D->Discriminator;
//          readTreeIP3D->Discriminator>-10?readTreeIP3D->Discriminator:-10;
      
      if (!forNN)
      {
        discriminatorIP2D=readTreeIP2D->Discriminator;
        discriminatorSV1=readTreeSV1->Discriminator;
        discriminatorCOMB=readTreeCOMB->Discriminator;
        deltaR=readTreeCOMB->DeltaRtoBorCorTau;
      }
      

      nVTX=readTreeJF->nVTX;
      nSingleTracks=readTreeJF->nSingleTracks;
      nTracksAtVtx=readTreeJF->nTracksAtVtx;
      energyFraction=readTreeJF->energyFraction;
      mass=readTreeJF->mass;
      significance3d=readTreeJF->significance3d;
      myTree->Fill();
      
    }
    
  }


  myTree->Write();
  file->Write();
  file->Close();
  


  /*
  delete readTreeIP2D;
  delete readTreeIP3D;
  delete readTreeSV1;
  delete readTreeCOMB;
  delete readTreeJF;
  */

  inputFile.Close();

  
}
  
