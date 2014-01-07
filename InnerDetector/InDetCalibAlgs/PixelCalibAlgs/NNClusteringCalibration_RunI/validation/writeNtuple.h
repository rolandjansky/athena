/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jul 20 23:04:22 2011 by ROOT version 5.28/00e
// from TTree NNinput/NNtraining
// found on file: root://castoratlas//castor/cern.ch/grid/atlas/atlasscratchdisk/user.lorenzi/user.lorenzi.T1.TRKVAL.NNtrain.clustersOnTrack.110701122007_sub022943841/user.lorenzi.000160.TRKVAL._00027.root
//////////////////////////////////////////////////////////

#ifndef writeNtuple_h
#define writeNtuple_h

#include <TROOT.h>
#include <TSystem.h>
#include <TChain.h>
#include <TFile.h>
#include "RIOs.h"
#include <TString.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <TChainElement.h>

using namespace std;


#pragma link C++ class vector<int> +;
#pragma link C++ class vector<double> +;
#pragma link C++ class vector<float> +; Loader




class writeNtuple {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   TChain         *tree2;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   RIOs		  *fOther;
   // Declaration of leaf types
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
   vector<double>  *NN_localColumnWeightedPosition;
   vector<double>  *NN_localRowWeightedPosition;
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

        // --- ntuple items. Please document here so they can be automatically parsed for doxygen/wiki tables ---
   mutable std::vector<int>*   m_PixDetType; //!< Layer information (0, 1, 2 for barrel, 100, 101, 102 for endcap A side, -100, -101, -102 for endcap C side) it is 0 by default
   mutable std::vector<int>*   m_PixHasGanged; //!< if the cluster contains ganged pixels UNIT:1:yes
   mutable std::vector<int>*   m_PixIsFake;   //!< if the cluster is marked as fake UNIT:1:yes
   mutable std::vector<int>*   m_PixLVL1A;    //!< lower level1 accept among the pixel of the cluster
   mutable std::vector<int>*   m_PixToT;      //!< ToTal time over threshold (ToT) of the cluster UNIT:??
   mutable std::vector<float>* m_PixCharge;   //!< total charge of the cluster UNIT:??
   mutable std::vector<float>* m_PixTLorPhi;  //!< tangent of the lorentz angle (local x direction) UNIT:1
   mutable std::vector<float>* m_PixTLorEta;  //!< tangent of the lorentz angle (local y direction - should be 0!) UNIT:1
   mutable std::vector<float>* m_PixBiasVolt; //!< Bias voltage (from conditions service) UNIT:V ?
   mutable std::vector<float>* m_PixTemp;     //!< Temperature (from conditions service) UNIT:K ?
   mutable std::vector<float>* m_PixDepVolt;  //!< Depletion voltage (from conditions service) UNIT:kV?
   mutable std::vector<float>* m_PixDigResPhi; //!< local phi residual obtained using the cluster digital coordinates UNIT:mm
   mutable std::vector<float>* m_PixDigResEta; //!< local eta residual obtained from using the cluster digital coords UNIT:mm
   mutable std::vector<float>* m_PixResPhi; //!< local phi residual obtained using the cluster digital coordinates UNIT:mm
   mutable std::vector<float>* m_PixResEta; //!< local eta residual obtained from using the cluster digital coords UNIT:mm
   mutable std::vector<float>* m_PixErrPhi;    //!< PixelCluster position error (local x) UNIT:mm
   mutable std::vector<float>* m_PixErrEta;    //!< PixelCluster position error (local y) UNIT:mm
   mutable std::vector<float>* m_PixEta_t;       //!< pseudorapidity calculated from module position
   mutable std::vector<float>* m_PixGloX;      //!< global X position of the cluster UNIT:mm
   mutable std::vector<float>* m_PixGloY;      //!< global Y position of the cluster UNIT:mm
   mutable std::vector<float>* m_PixGloZ;      //!< global Z position of the cluster UNIT:mm
   mutable std::vector<float>* m_PixLocX;      //!< local x coordinate of the cluster UNIT:mm
   mutable std::vector<float>* m_PixLocY;      //!< local y coordinate of the cluster UNIT:mm
   mutable std::vector<int>*   m_PixEtaModule_t; //!< eta index of the module associated to the cluster
   mutable std::vector<int>*   m_PixPhiModule_t; //!< phi index of the module associated to the cluster
   mutable std::vector<float>* m_PixFirstRow;  //!< lower row of the pixels contained in the cluster UNIT:index
   mutable std::vector<float>* m_PixFirstCol;  //!< lower column of the pixels contained in the cluster UNIT:index
   mutable std::vector<float>* m_PixDeltaPhi_t;  //!< Pixel cluster size phi, in cell units UNIT:cells
   mutable std::vector<float>* m_PixDeltaEta_t;  //!< Pixel cluster size eta, in cell units UNIT:cells
   mutable std::vector<int>*   m_PixDeltaRow_t;  //!< Pixel cluster size phi or row UNIT:mm
   mutable std::vector<int>*   m_PixDeltaCol_t;  //!< Pixel cluster size eta or column UNIT:mm
   mutable std::vector<float>* m_PixOmegaPhi_t;  //!< Pixel charge sharing variable, phi clustering UNIT:1
   mutable std::vector<float>* m_PixOmegaEta_t;  //!< Pixel charge sharing variable, eta clustering UNIT:1
   mutable std::vector<float>* m_PixTrkAngle;  //!< track incidence angle on the module (rphi plane) in local reference frame UNIT:rad
   mutable std::vector<float>* m_PixTrkThetaI; //!< track incidence angle on the module (eta-normal plane) in local reference frame UNIT:rad
   mutable std::vector<float>* m_PixTrkEta;    //!< track incidence pseudorapidity in local module reference frame
   mutable std::vector<float>* m_PixTrkPt;     //!< track transverse momentum at module
   mutable std::vector<float>* m_PixTrkQ;      //!< charge of the associated track UNIT:+/-1
   mutable std::vector<float>* m_PixTrkClusGroupSize;  //!< total size of the cluster UNIT:cells
   mutable std::vector<std::vector<float> >* m_PixChargeList;//!< Charge of the pixel UNIT:electrons
   /*  mutable std::vector<std::vector<int> >*   m_PixToTList; //!< Time over Threshold of the pixel
       mutable std::vector<std::vector<int> >*   m_PixLVL1AList; //!< Level 1 accept of the pixel
       mutable std::vector<std::vector<int> >*   m_PixEtaIndex; //!< Cluster position converted into pixel column index (eta direction) UNIT:index
       mutable std::vector<std::vector<int> >*   m_PixPhiIndex; //!< Cluster position converted into pixel row index (phi direction) UNIT:index
   */
   mutable int m_nPixelHits;
   mutable int m_nTRTHits;
   mutable int m_nSCTHits;
   mutable std::vector<float>*   m_PixPullPhi;
   mutable std::vector<float>*   m_PixPullEta;

   TString  m_pathWeightsWithTracks;
   TString  m_pathWeightsWithoutTracks;

   writeNtuple(TTree *tree=0);
   virtual ~writeNtuple();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(int nParticlesTraining, bool useTrackEstimate, TString outFileName);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void     FillClusterInfo(int clus);
   virtual void     InitializeVectors();
   virtual void     ResetVectors();

}; 

#endif

#ifdef writeNtuple_cxx
/*
writeNtuple::writeNtuple(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.


  TString fName = "dummy.root";//"root://castoratlas//castor/cern.ch/grid/atlas/atlasscratchdisk/user.lorenzi/user.lorenzi.T1.TRKVAL.NNtrain.clustersOnTrack.110701122007_sub022943841/user.lorenzi.000160.TRKVAL._00027.root";

  if (tree == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("dummy.root");
    if (!f) {
      f = TFile::Open(fName);
      
    }
    tree = (TTree*)gDirectory->Get("Validation/NNinput");
    tree2=0;
  }
  Init(tree);
  
  if (tree2 == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("dummy.root");
    if (!f) {
      f = TFile::Open(fName);
    }
    tree2 = (TTree*)gDirectory->Get("Validation/PixelRIOs");
    fOther = new RIOs( tree2 );
    
    
  }else{

    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("dummy.root");
    if (!f) {
      f = tree->GetCurrentFile();
    }     
    tree2 = (TTree*)gDirectory->Get("Validation/PixelRIOs");
    fOther = new RIOs( tree2 );
    
  }





 
}


*/

writeNtuple::writeNtuple(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("AnalysisSkeleton.aan.root");
      if (!f) {
         f = new TFile("AnalysisSkeleton.aan.root");
      }
      tree = (TTree*)f->Get("Validation/NNinput");
   }
   Init(tree);

   /*
   if (tree->IsA() == TChain::Class()) {
     tree2 = (TTree*)tree->Clone("Validation/PixelRIOs");  // Create a copy of the TChain with a different name but the same list of files.
   } else {
     TFile *f = tree->GetTree()->GetCurrentFile();
     tree2 = (TTree*)f->Get("Validation/PixelRIOs");
     if (tree2 == 0) {
       f = (TFile*)gROOT->GetListOfFiles()->FindObject("AnalysisSkeleton.aan.root");
       if (!f) {
	 f = new TFile("AnalysisSkeleton.aan.root");
       }
       tree2 = (TTree*)f->Get("Validation/PixelRIOs");
     }
    }*/
/*
     TFile *f1 = tree->GetTree()->GetCurrentFile();
     tree2 = (TTree*)f1->Get("Validation/PixelRIOs");

     tree2  = (TTree*)tree->Clone("Validation/PixelRIOs"); 
*/

/*
   if (tree->IsA() == TChain::Class()) {
     // Make a copy of the TChain with a new name to point to the other set of TTree.
   tree2 = (TTree*)tree->Clone("Validation/PixelRIOs");
//    tree2= new TChain("Validation/PixelRIOs");
   } else {
     // We have a real TTree.
     TFile *f = tree->GetCurrentFile();
     if (f == 0) {
       cout << "Error in constructor" << endl; 
       // Fatal("Constructor","The TTree we are being passed in not disk resident");
     }
     // This assumes that the other TTree (Photon) is located in the top level directory of the TFile.
     tree2 = (TTree*)f->Get("Validation/PixelRIOs");
   }
   // Create the secondary MakeClass/MakeSelector object pointing to the secondary TTree.
   */


   tree2 = new TChain("Validation/PixelRIOs");
   TIter next( ((TChain*)tree)->GetListOfFiles());
   TObject *obj;
   while( ( obj = (TChainElement*) next() ) ) {
     tree2->Add(obj->GetTitle());
   }
   

   //  tree2->Print();

   fOther = new RIOs( tree2 );
   //   fOther->fChain->Print();

}
   




writeNtuple::~writeNtuple()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t writeNtuple::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t writeNtuple::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void writeNtuple::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
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
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;

   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("NN_sizeX", &NN_sizeX, &b_NN_sizeX);
   fChain->SetBranchAddress("NN_sizeY", &NN_sizeY, &b_NN_sizeY);
   fChain->SetBranchAddress("NN_matrixOfToT", &NN_matrixOfToT, &b_NN_matrixOfToT);
   fChain->SetBranchAddress("NN_vectorOfPitchesY", &NN_vectorOfPitchesY, &b_NN_vectorOfPitchesY);
   fChain->SetBranchAddress("NN_ClusterPixLayer", &NN_ClusterPixLayer, &b_NN_ClusterPixLayer);
   fChain->SetBranchAddress("NN_ClusterPixBarrelEC", &NN_ClusterPixBarrelEC, &b_NN_ClusterPixBarrelEC);
   fChain->SetBranchAddress("NN_phiBS", &NN_phiBS, &b_NN_phiBS);
   fChain->SetBranchAddress("NN_thetaBS", &NN_thetaBS, &b_NN_thetaBS);
   fChain->SetBranchAddress("NN_etaModule", &NN_etaModule, &b_NN_etaModule);
   fChain->SetBranchAddress("NN_useTrackInfo", &NN_useTrackInfo, &b_NN_useTrackInfo);
   fChain->SetBranchAddress("NN_columnWeightedPosition", &NN_columnWeightedPosition, &b_NN_columnWeightedPosition);
   fChain->SetBranchAddress("NN_rowWeightedPosition", &NN_rowWeightedPosition, &b_NN_rowWeightedPosition);
   fChain->SetBranchAddress("NN_localColumnWeightedPosition", &NN_localColumnWeightedPosition, &b_NN_localColumnWeightedPosition);
   fChain->SetBranchAddress("NN_localRowWeightedPosition", &NN_localRowWeightedPosition, &b_NN_localRowWeightedPosition);
   fChain->SetBranchAddress("NN_positionX", &NN_positionX, &b_NN_positionX);
   fChain->SetBranchAddress("NN_positionY", &NN_positionY, &b_NN_positionY);
   fChain->SetBranchAddress("NN_position_idX", &NN_position_idX, &b_NN_position_idX);
   fChain->SetBranchAddress("NN_position_idY", &NN_position_idY, &b_NN_position_idY);
   fChain->SetBranchAddress("NN_theta", &NN_theta, &b_NN_theta);
   fChain->SetBranchAddress("NN_phi", &NN_phi, &b_NN_phi);
     

   //InitializeVectors();
   
  m_PixDetType    = new std::vector<int>();
  m_PixHasGanged  = new std::vector<int>();
  m_PixIsFake     = new std::vector<int>();
  m_PixLVL1A      = new std::vector<int>();
  m_PixToT        = new std::vector<int>();
  m_PixCharge     = new std::vector<float>();
  m_PixTLorPhi    = new std::vector<float>();
  m_PixTLorEta    = new std::vector<float>();
  m_PixBiasVolt   = new std::vector<float>();
  m_PixTemp       = new std::vector<float>();
  m_PixDepVolt    = new std::vector<float>();
  m_PixLocX       = new std::vector<float>();
  m_PixLocY       = new std::vector<float>();
  m_PixEtaModule_t= new std::vector<int>();
  m_PixPhiModule_t= new std::vector<int>();
  m_PixGloX       = new std::vector<float>();
  m_PixGloY       = new std::vector<float>();
  m_PixGloZ       = new std::vector<float>();
  m_PixEta_t      = new std::vector<float>();
  m_PixFirstRow   = new std::vector<float>();
  m_PixFirstCol   = new std::vector<float>();
  m_PixDigResPhi  = new std::vector<float>();
  m_PixDigResEta  = new std::vector<float>();
  m_PixResPhi  = new std::vector<float>();
  m_PixResEta  = new std::vector<float>();
  m_PixErrPhi     = new std::vector<float>();
  m_PixErrEta     = new std::vector<float>();
  m_PixDeltaRow_t   = new std::vector<int>();
  m_PixDeltaCol_t   = new std::vector<int>();
  m_PixDeltaPhi_t   = new std::vector<float>();
  m_PixDeltaEta_t   = new std::vector<float>();
  m_PixOmegaPhi_t   = new std::vector<float>();
  m_PixOmegaEta_t   = new std::vector<float>();
  /*    m_PixChargeList = new std::vector<std::vector<float> >();
	m_PixToTList    = new std::vector<std::vector<int> >();
	m_PixLVL1AList  = new std::vector<std::vector<int> >();
	m_PixEtaIndex   = new std::vector<std::vector<int> >();
	m_PixPhiIndex   = new std::vector<std::vector<int> >();
  */
  m_PixTrkAngle   = new std::vector<float>();
  m_PixTrkThetaI  = new std::vector<float>();
  m_PixTrkEta     = new std::vector<float>();
  m_PixTrkPt      = new std::vector<float>();
  m_PixTrkQ       = new std::vector<float>();
  m_PixTrkClusGroupSize = new std::vector<float>();

  m_PixPullPhi = new std::vector<float>();
  m_PixPullEta = new std::vector<float>();

   Notify();
}

Bool_t writeNtuple::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void writeNtuple::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t writeNtuple::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


void writeNtuple::InitializeVectors(){
  /*
  m_PixDetType    = new std::vector<int>();
  m_PixHasGanged  = new std::vector<int>();
  m_PixIsFake     = new std::vector<int>();
  m_PixLVL1A      = new std::vector<int>();
  m_PixToT        = new std::vector<int>();
  m_PixCharge     = new std::vector<float>();
  m_PixTLorPhi    = new std::vector<float>();
  m_PixTLorEta    = new std::vector<float>();
  m_PixBiasVolt   = new std::vector<float>();
  m_PixTemp       = new std::vector<float>();
  m_PixDepVolt    = new std::vector<float>();
  m_PixLocX       = new std::vector<float>();
  m_PixLocY       = new std::vector<float>();
  m_PixEtaModule_t= new std::vector<int>();
  m_PixPhiModule_t= new std::vector<int>();
  m_PixGloX       = new std::vector<float>();
  m_PixGloY       = new std::vector<float>();
  m_PixGloZ       = new std::vector<float>();
  m_PixEta_t      = new std::vector<float>();
  m_PixFirstRow   = new std::vector<float>();
  m_PixFirstCol   = new std::vector<float>();
  m_PixDigResPhi  = new std::vector<float>();
  m_PixDigResEta  = new std::vector<float>();
  m_PixErrPhi     = new std::vector<float>();
  m_PixErrEta     = new std::vector<float>();
  m_PixDeltaRow_t   = new std::vector<int>();
  m_PixDeltaCol_t   = new std::vector<int>();
  m_PixDeltaPhi_t   = new std::vector<float>();
  m_PixDeltaEta_t   = new std::vector<float>();
  m_PixOmegaPhi_t   = new std::vector<float>();
  m_PixOmegaEta_t   = new std::vector<float>();
  */  
/*    m_PixChargeList = new std::vector<std::vector<float> >();
	m_PixToTList    = new std::vector<std::vector<int> >();
	m_PixLVL1AList  = new std::vector<std::vector<int> >();
	m_PixEtaIndex   = new std::vector<std::vector<int> >();
	m_PixPhiIndex   = new std::vector<std::vector<int> >();
  */
  /* m_PixTrkAngle   = new std::vector<float>();
  m_PixTrkThetaI  = new std::vector<float>();
  m_PixTrkEta     = new std::vector<float>();
  m_PixTrkPt      = new std::vector<float>();
  m_PixTrkQ       = new std::vector<float>();
  m_PixTrkClusGroupSize = new std::vector<float>();
  */


  return;

}

void writeNtuple::ResetVectors(){
  
  m_PixDetType->clear();
  m_PixHasGanged->clear();
  m_PixIsFake->clear();
  m_PixLVL1A->clear();
  m_PixToT->clear();
  m_PixCharge->clear();
  m_PixTLorPhi->clear();
  m_PixTLorEta->clear();
  m_PixBiasVolt->clear();
  m_PixTemp->clear();
  m_PixDepVolt->clear();
  m_PixDigResPhi->clear();
  m_PixDigResEta->clear();
  m_PixResPhi->clear();
  m_PixResEta->clear();
  m_PixErrPhi->clear();
  m_PixErrEta->clear();
  m_PixEta_t->clear();
  m_PixGloX->clear();
  m_PixGloY->clear();
  m_PixGloZ->clear();
  m_PixLocX->clear();
  m_PixLocY->clear();
  m_PixEtaModule_t->clear();
  m_PixPhiModule_t->clear();
  m_PixFirstRow->clear();
  m_PixFirstCol->clear();
  m_PixDeltaPhi_t->clear();
  m_PixDeltaEta_t->clear();
  m_PixDeltaRow_t->clear();
  m_PixDeltaCol_t->clear();
  m_PixOmegaPhi_t->clear();
  m_PixOmegaEta_t->clear();
  
/*   m_PixChargeList->clear(); */
/*   m_PixToTList->clear(); */
/*   m_PixLVL1AList->clear(); */
/*   m_PixEtaIndex->clear(); */
/*   m_PixPhiIndex->clear(); */
 
  m_PixTrkAngle->clear();
  m_PixTrkThetaI->clear();
  m_PixTrkEta->clear();
  m_PixTrkPt->clear();
  m_PixTrkQ->clear();
  m_PixTrkClusGroupSize->clear();

  m_PixPullPhi->clear();
  m_PixPullEta->clear();

   m_nPixelHits=0;
   m_nSCTHits=0;
   m_nTRTHits=0;

  return;

}


void writeNtuple::FillClusterInfo(int clus)
{

  



  m_PixEta_t->push_back( fOther->PixClusEta->at(clus)   );
  m_PixGloX->push_back(  fOther->PixClusGloX->at(clus)  );
  m_PixGloY->push_back(  fOther->PixClusGloY->at(clus)  );
  m_PixGloZ->push_back(  fOther->PixClusGloZ->at(clus)  );
  


  m_PixEtaModule_t->push_back( fOther->PixEtaModule->at(clus));
  m_PixPhiModule_t->push_back( fOther->PixPhiModule->at(clus));
 


  m_PixDeltaRow_t->push_back(fOther->PixDeltaRow->at(clus));
  m_PixDeltaCol_t->push_back(fOther->PixDeltaCol->at(clus));

/*   m_PixDeltaPhi->push_back(fOther->PixDeltaPhi); */
/*   m_PixDeltaEta->push_back(fOther->PixDeltaEta); */

  m_PixOmegaPhi_t->push_back(fOther->PixOmegaPhi->at(clus));
  m_PixOmegaEta_t->push_back(fOther->PixOmegaEta->at(clus));

  float PixTLorPhi    = 0.;
  float PixTLorEta    = -1000.;
  float PixBiasVolt    = -1000.;
  float PixTemp    = -1000.;
  float PixDepVolt    = -1000.;
  

  m_PixTLorPhi->push_back(PixTLorPhi);
  m_PixTLorEta->push_back(PixTLorEta);
  m_PixBiasVolt->push_back(PixBiasVolt);
  m_PixTemp->push_back(PixTemp);
  m_PixDepVolt->push_back(PixDepVolt);
  

  m_PixHasGanged->push_back(fOther->PixClusGanged->at(clus));
  m_PixIsFake->push_back(fOther->PixClusFake->at(clus));
  m_PixLVL1A->push_back(fOther->PixClusLvl1->at(clus));
  m_PixToT->push_back(fOther->PixClusToT->at(clus));
  m_PixCharge->push_back(fOther->PixClusCharge->at(clus));


  int   PixDetType    = 0;


//!< Layer information (0, 1, 2 for barrel, 100, 101, 102 for endcap A side, -100, -101, -102 for endcap C side) it is 0 by default

  int ECBarrel =  fOther->PixECBarrel->at(clus);
  int Layer    =  fOther->PixClusLayer->at(clus);
   
//PixDetType = 100*ECBarrel/2+Layer ;   

  if( ECBarrel == 0 ){
    PixDetType = Layer;
  } 
  
  if( ECBarrel == 2 ){
    PixDetType = 100+Layer;
  }
  
  if( ECBarrel == -2 ){
    PixDetType = -100-Layer;
  }
  
  
  m_PixDetType->push_back(PixDetType);
  
  
  return;

}

#endif // #ifdef writeNtuple_cxx
