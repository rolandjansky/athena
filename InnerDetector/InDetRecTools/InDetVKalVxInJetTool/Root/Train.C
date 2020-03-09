/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*-------------------------------------------------------------------------
   Script for multiclass TMVA training using track origin information.
   Don't forget to set AntiGarbageCut=1 in SSVF to create the training data.
   Author: V.Kostyukhin
*/
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"


void TrainMulti(  )
{
   TString outfileName( "TMVAMulti.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );
   TMVA::Factory *factory = new TMVA::Factory( "TMVAMulticlass", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;P;G,D:AnalysisType=multiclass" );
   TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");

   //dataloader->AddVariable( "prbS", 'F' );
   dataloader->AddVariable( "Sig3D", 'F' );
   dataloader->AddVariable( "prbP", 'F' );
   dataloader->AddVariable( "pTvsJet", 'F' );
   //dataloader->AddVariable( "prodTJ", 'F' );
   dataloader->AddVariable( "d0", 'F' );
   dataloader->AddVariable( "SigR", 'F' );
   dataloader->AddVariable( "SigZ", 'F' );
   dataloader->AddVariable( "ptjet", 'F' );
   dataloader->AddVariable( "ibl", 'I' );
   dataloader->AddVariable( "bl", 'I' );
   dataloader->AddVariable( "etatrk", 'F' );
   //dataloader->AddVariable( "vChi2", 'F' );
   //dataloader->AddSpectator( "idMC",  'I' );

   TChain *chainB =new TChain("/stat/SVrtInJetTrackExample.BJetSVFinder/Tracks");
   chainB->Add("../run/allcalib.root");
   chainB->Add("../runZp1500/allcalib.root");
   chainB->Add("../runZp3000/allcalib.root");
   chainB->Add("../runZp5000/allcalib.root");
   chainB->Add("../runLJ4/allcalib.root");
   chainB->Add("../runLJ6/allcalib.root");
   chainB->Add("../runLJ8/allcalib.root");
   TChain *chainL =new TChain("/stat/SVrtInJetTrackExample.LJetSVFinder/Tracks");
   chainL->Add("../run/allcalib.root");
   chainL->Add("../runZp1500/allcalib.root");
   chainL->Add("../runZp3000/allcalib.root");
   chainL->Add("../runZp5000/allcalib.root");
   chainL->Add("../runLJ4/allcalib.root");
   chainL->Add("../runLJ6/allcalib.root");
   chainL->Add("../runLJ8/allcalib.root");

   dataloader->AddTree(chainB,"Signal");
   dataloader->AddTree(chainL,"Signal");
   dataloader->AddTree(chainB,"Fragment");
   dataloader->AddTree(chainL,"Fragment");
   dataloader->AddTree(chainB,"Garbage");
   dataloader->AddTree(chainL,"Garbage");
   //dataloader->AddTree(chainB,"Pileup");
   //dataloader->AddTree(chainL,"Pileup");

   //dataloader->SetCut(TCut("idMC==2&&prbS>0&&(-3)<d0&&d0<5.&&(-8)<Z0&&Z0<12."),"Signal");
   //dataloader->SetCut(TCut("idMC==0&&prbS>0&&(-3)<d0&&d0<5.&&(-8)<Z0&&Z0<12."),"Fragment");
   //dataloader->SetCut(TCut("idMC==1&&prbS>0&&(-3)<d0&&d0<5.&&(-8)<Z0&&Z0<12."),"Garbage");
   //dataloader->SetCut(TCut("idMC==3&&prbS>0&&(-3)<d0&&d0<5.&&(-8)<Z0&&Z0<12."),"Pileup");
   dataloader->SetCut(TCut("idMC==2&&(SigR*SigR+SigZ*SigZ)>1.&&(-3)<d0&&d0<5.&&(-8)<Z0&&Z0<12.&&ptjet<7000000"),"Signal");
   dataloader->SetCut(TCut("idMC==0&&(SigR*SigR+SigZ*SigZ)>1.&&(-3)<d0&&d0<5.&&(-8)<Z0&&Z0<12.&&ptjet<7000000"),"Fragment");
   dataloader->SetCut(TCut("(idMC==1||idMC==3)&&(SigR*SigR+SigZ*SigZ)>1.&&(-3)<d0&&d0<5.&&(-8)<Z0&&Z0<12.&&ptjet<7000000"),"Garbage");

   dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" );
   //dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V:nTrain_Signal=5000:nTrain_Fragment=5000:nTrain_Garbage=10000:nTest_Signal=5000:nTest_Fragment=5000:nTest_Garbage=10000" );
   //dataloader->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V:nTrain_Signal=5000:nTrain_Fragment=5000:nTrain_Garbage=5000:nTrain_Pileup=5000:nTest_Signal=5000:nTest_Fragment=5000:nTest_Garbage=5000:nTest_Pileup=5000" );

   factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG", "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.25:UseBaggedBoost:BaggedSampleFraction=0.50:nCuts=20:MaxDepth=5");
   //factory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam", "!H:!V:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );
  // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------
   
   // Save the output
   outputFile->Close();
   
   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;
   
   delete factory;
}

//

void TMVAMultiApply( )
{
   // create the Reader object
   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );    
   Float_t prbS, prbP, ptjet, etajet;
   Int_t ntrk;
   reader->AddVariable( "prbS", &prbS );
   reader->AddVariable( "prbP", &prbP );
   reader->AddVariable( "ptjet", &ptjet );
   reader->AddVariable( "etajet", &etajet );
   //reader->AddSpectator( "ntrk",  &ntrk );

   TString methodName = "BDTG";
   TString weightfile = "weights/TMVAMulticlass_BDTG.weights.xml";
   reader->BookMVA( methodName, weightfile ); 

   prbS=0.6;
   prbP=0.9;
   ptjet=250000.;
   etajet=1.;
   const std::vector<Float_t> &result=reader->EvaluateMulticlass("BDTG");
   std::cout<<" TEST="<<result.size()<<" wgt="<<result[0]<<","<<result[1]<<","<<result[2]<<'\n';

   delete reader;
}

