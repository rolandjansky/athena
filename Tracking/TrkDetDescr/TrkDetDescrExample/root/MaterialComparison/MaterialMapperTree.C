/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #define MaterialMapperTree_cxx
#include "MaterialMapperTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

std::vector<TProfile*>* MaterialMapperTree::profileHistograms(int bins, TString type, bool etaphi)
{
   if (fChain == 0) return 0;

   Long64_t nentries = fChain->GetEntriesFast();

   Int_t nbytes = 0, nb = 0;

   std::vector<TProfile*>* profileHistos = new std::vector<TProfile*>;

   const int dets  = 12;
   const int hists = 9;

   
   if (etaphi){
      fChain->SetBranchAddress("Eta",&Angle);
   } else
      fChain->SetBranchAddress("Phi",&Angle);
   


   TString detTypes[dets]  
       = { "BeamPipe", "Pixel", "Sct", "Trt", "IDServices", "ID", "Solenoid", "LAr", "Tile", "CaloServ", "Calo", "Total" };

   TString histTypes[hists] = { "pathX0_", "pathL0_", "pathX0L0_",  "A_", "Z_", "ZA_", "Rho_", "ZA_Rho_", "Path_" };
   TString histNames[hists]  = { "t/X_0 ", "t/L_0", "X_0/L_0", "A ", "Z ", "Z/A", "#rho ", "#frac{Z}{A}#rho ", "t" };
   

   for (int idet=0; idet<dets; ++idet){

     double angleRange = etaphi ? 4. : 3.2;

     TString   X0Type  = histTypes[0];
               X0Type += detTypes[idet];
               X0Type += "_";
               X0Type += type;

     TString   X0Name  = histNames[0];
               X0Name += detTypes[idet];

     profileHistos->push_back(new TProfile(X0Type, X0Name, bins, -angleRange,angleRange,0.,300.));


     TString   L0Type  = histTypes[1];
               L0Type += detTypes[idet];
               L0Type += "_";
               L0Type += type;

     TString   L0Name  = histNames[1];
               L0Name += detTypes[idet];

     profileHistos->push_back(new TProfile(L0Type, L0Name, bins, -angleRange,angleRange,0.,300.));

     TString   X0L0Type  = histTypes[2];
               X0L0Type += detTypes[idet];
               X0L0Type += "_";
               X0L0Type += type;

     TString   X0L0Name  = histNames[2];
               X0L0Name += detTypes[idet];

     profileHistos->push_back(new TProfile(X0L0Type, X0L0Name, bins, -angleRange,angleRange,0.,300.));



     TString   AType  = histTypes[3];
               AType += detTypes[idet];
               AType += "_";
               AType += type;

     TString   AName  = histNames[3];
               AName += detTypes[idet];

     profileHistos->push_back(new TProfile(AType, AName, bins, -angleRange,angleRange,0.,300.));

     TString   ZType  = histTypes[4];
               ZType += detTypes[idet];
               ZType += "_";
               ZType += type;

     TString   ZName  = histNames[4];
               ZName += detTypes[idet];

     profileHistos->push_back(new TProfile(ZType, ZName, bins, -angleRange,angleRange,0.,100.));

     TString   ZAType  = histTypes[5];
               ZAType += detTypes[idet];
               ZAType += "_";
               ZAType += type;

     TString   ZAName  = histNames[5];
               ZAName += detTypes[idet];

     profileHistos->push_back(new TProfile(ZAType, ZAName, bins, -angleRange,angleRange,0.,0.8));

     TString   RhoType  = histTypes[6];
               RhoType += detTypes[idet];
               RhoType += "_";
               RhoType += type;

     TString   RhoName  = histNames[6];
               RhoName += detTypes[idet];

     profileHistos->push_back(new TProfile(RhoType, RhoName, bins, -angleRange,angleRange,0.,10.));

     TString   ZARhoType  = histTypes[7];
               ZARhoType += detTypes[idet];
               ZARhoType += "_";
               ZARhoType += type;

     TString   ZARhoName  = histNames[7];
               ZARhoName += detTypes[idet];

     profileHistos->push_back(new TProfile(ZARhoType, ZARhoName, bins, -angleRange,angleRange,0.,10.));
   
     TString   PathType  = histTypes[8];
               PathType += detTypes[idet];
               PathType += "_";
               PathType += type;

     TString   PathName  = histNames[8];
               PathName += detTypes[idet];

     profileHistos->push_back(new TProfile(PathType, PathName, bins, -angleRange,angleRange,0.,10000.));


   }

   std::cout << " [m] All Profile Histograms booked for input type : " << type << std::endl;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      // BeamPipe
      int curDet = 0;
      if (BeamPipePathInX0 > 0.) {
         ((*profileHistos)[curDet*hists + 0])-> Fill(Angle,BeamPipePathInX0);
         ((*profileHistos)[curDet*hists + 1])-> Fill(Angle,BeamPipePathInL0);
          if (BeamPipePathInL0 > 0.)
           ((*profileHistos)[curDet*hists + 2])-> Fill(Angle,BeamPipePathInX0/BeamPipePathInL0);
         ((*profileHistos)[curDet*hists + 3])-> Fill(Angle,BeamPipeA);
         ((*profileHistos)[curDet*hists + 4])-> Fill(Angle,BeamPipeZ);
       if (BeamPipeA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle, BeamPipeZ/BeamPipeA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,BeamPipeRho);
       if (BeamPipeA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle, BeamPipePath*BeamPipeZ/BeamPipeA * BeamPipeRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,BeamPipePath);
       }

      // Pixels
      ++curDet;
      if (PixelPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,PixelPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,PixelPathInL0);
          if (PixelPathInL0 > 0.)
           ((*profileHistos)[curDet*hists + 2])-> Fill(Angle,PixelPathInX0/PixelPathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,PixelA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,PixelZ);
       if (PixelA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,PixelZ/PixelA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,PixelRho);
       if (PixelA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,PixelZ/PixelA*PixelRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,PixelPath*PixelPath);
      }
      // Sct
      ++curDet;
      if (SctPathInX0 > 0.){
        ((*profileHistos)[curDet*hists + 0])->Fill(Angle,SctPathInX0);
        ((*profileHistos)[curDet*hists + 1])->Fill(Angle,SctPathInL0);
          if (SctPathInL0 > 0.)
           ((*profileHistos)[curDet*hists + 2])-> Fill(Angle,SctPathInX0/SctPathInL0);
        ((*profileHistos)[curDet*hists + 3])->Fill(Angle,SctA);
        ((*profileHistos)[curDet*hists + 4])->Fill(Angle,SctZ);
      if (SctA>0.) 
        ((*profileHistos)[curDet*hists + 5])->Fill(Angle,SctZ/SctA);
        ((*profileHistos)[curDet*hists + 6])->Fill(Angle,SctRho);
      if (SctA>0.) 
        ((*profileHistos)[curDet*hists + 7])->Fill(Angle,SctPath*SctZ/SctA*SctRho);
        ((*profileHistos)[curDet*hists + 8])->Fill(Angle,SctPath);
      }

      // Trt
      ++curDet;
      if (TrtPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,TrtPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,TrtPathInL0);
          if (TrtPathInL0 > 0.)
           ((*profileHistos)[curDet*hists + 2])-> Fill(Angle,TrtPathInX0/TrtPathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,TrtA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,TrtZ);
       if (TrtA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,TrtZ/TrtA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,TrtRho);
       if (TrtA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,TrtPath*TrtZ/TrtZ *TrtRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,TrtPath);
       }
      // InDet Services
      ++curDet;
      if (InDetServPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,InDetServPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,InDetServPathInL0);
          if (InDetServPathInL0 > 0.)
           ((*profileHistos)[curDet*hists + 2])-> Fill(Angle,InDetServPathInX0/InDetServPathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,InDetServA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,InDetServZ);
       if (InDetServA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,InDetServZ/InDetServA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,InDetServRho);
       if (InDetServA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,InDetServZ/InDetServZ*InDetServRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,InDetServPath);
      }
      // InDet
      ++curDet;
      if (InDetPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,InDetPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,InDetPathInL0);
          if (InDetPathInL0 > 0.)
           ((*profileHistos)[curDet*hists + 2])->Fill(Angle,InDetPathInX0/InDetPathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,InDetA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,InDetZ);
       if (InDetA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,InDetZ/InDetA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,InDetRho);
       if (InDetA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,InDetPath*InDetZ/InDetA*InDetRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,InDetPath);
      }
      // Solenoid
      ++curDet;
      if (SolenoidPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,SolenoidPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,SolenoidPathInL0);
          if (SolenoidPathInL0 > 0.)
           ((*profileHistos)[curDet*hists + 2])->Fill(Angle,SolenoidPathInX0/SolenoidPathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,SolenoidA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,SolenoidZ);
       if (SolenoidA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,SolenoidZ/SolenoidA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,SolenoidRho);
       if (SolenoidA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,SolenoidPath*SolenoidZ/SolenoidA*SolenoidRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,SolenoidPath);
      }
      // LAr
      ++curDet;
      if (LArPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,LArPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,LArPathInL0);
	  if (LArPathInL0 > 0.)
	   ((*profileHistos)[curDet*hists + 2])->Fill(Angle,LArPathInX0/LArPathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,LArA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,LArZ);
       if (LArA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,LArZ/LArA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,LArRho);
       if (LArA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,LArPath*LArZ/LArA*LArRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,LArPath);
      }
      // Tile
      ++curDet;
      if (TilePathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,TilePathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,TilePathInL0);
         if (TilePathInL0 > 0.)
	   ((*profileHistos)[curDet*hists + 2])->Fill(Angle, TilePathInX0/TilePathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,TileA);
	 ((*profileHistos)[curDet*hists + 4])->Fill(Angle,TileZ);
       if (TileA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,TileZ/TileA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,TileRho);
       if (TileA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,TilePath*TileZ/TileA*TileRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,TilePath);
      }
      // CaloServ
      ++curDet;
      if (CaloServPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,CaloServPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,CaloServPathInL0);
	 if (CaloServPathInL0 > 0.)
	   ((*profileHistos)[curDet*hists + 2])->Fill(Angle,CaloServPathInX0/CaloServPathInL0);	 
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,CaloServA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,CaloServZ);
       if (CaloServA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,CaloServZ/CaloServA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,CaloServRho);
       if (CaloServA>0.) 
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,CaloServPath*CaloServZ/CaloServA*CaloServRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,CaloServPath);
      }
      // Calo
      ++curDet;
      if (CaloPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,CaloPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,CaloPathInL0);
	  if (CaloPathInL0 > 0.)
           ((*profileHistos)[curDet*hists + 2])->Fill(Angle,CaloPathInX0/CaloPathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,CaloA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,CaloZ);
       if (CaloA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,CaloZ/CaloA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,CaloRho);
       if (CaloA>0.)
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,CaloPath*CaloZ/CaloA*CaloRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,CaloPath);
      }
      // Total
      ++curDet;
      if (TotalPathInX0 > 0.){
         ((*profileHistos)[curDet*hists + 0])->Fill(Angle,TotalPathInX0);
         ((*profileHistos)[curDet*hists + 1])->Fill(Angle,TotalPathInL0);
          if (TotalPathInL0 > 0.)
             ((*profileHistos)[curDet*hists + 2])->Fill(Angle,TotalPathInX0/TotalPathInL0);
         ((*profileHistos)[curDet*hists + 3])->Fill(Angle,TotalA);
         ((*profileHistos)[curDet*hists + 4])->Fill(Angle,TotalZ);
       if (TotalA>0.) 
         ((*profileHistos)[curDet*hists + 5])->Fill(Angle,TotalZ/TotalA);
         ((*profileHistos)[curDet*hists + 6])->Fill(Angle,TotalRho);
       if (TotalA>0.)
         ((*profileHistos)[curDet*hists + 7])->Fill(Angle,TotalPath*TotalZ/TotalA*TotalRho);
         ((*profileHistos)[curDet*hists + 8])->Fill(Angle,TotalPath);
       }
   }

   return profileHistos;
}

MaterialMapperTree::MaterialMapperTree(TTree *tree)
{

   if (tree == 0) {
     std::cout << " No tree specified ... expect a segfault " << std::endl;
   }
   Init(tree);
}

MaterialMapperTree::~MaterialMapperTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MaterialMapperTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MaterialMapperTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() > TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() > fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void MaterialMapperTree::Init(TTree *tree)
{

   // Set branch addresses
   if (tree == 0) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Eta",&Eta);
   fChain->SetBranchAddress("Phi",&Phi);

   fChain->SetBranchAddress("BeamPipePathInX0",&BeamPipePathInX0);
   fChain->SetBranchAddress("BeamPipePathInL0",&BeamPipePathInL0);
   fChain->SetBranchAddress("BeamPipeA",&BeamPipeA);
   fChain->SetBranchAddress("BeamPipeZ",&BeamPipeZ);
   fChain->SetBranchAddress("BeamPipeRho",&BeamPipeRho);
   fChain->SetBranchAddress("BeamPipePath",&BeamPipePath);

   fChain->SetBranchAddress("PixelPathInX0",&PixelPathInX0);
   fChain->SetBranchAddress("PixelPathInL0",&PixelPathInL0);
   fChain->SetBranchAddress("PixelA",&PixelA);
   fChain->SetBranchAddress("PixelZ",&PixelZ);
   fChain->SetBranchAddress("PixelRho",&PixelRho);
   fChain->SetBranchAddress("PixelPath",&PixelPath);

   fChain->SetBranchAddress("SctPathInX0",&SctPathInX0);
   fChain->SetBranchAddress("SctPathInL0",&SctPathInL0);
   fChain->SetBranchAddress("SctA",&SctA);
   fChain->SetBranchAddress("SctZ",&SctZ);
   fChain->SetBranchAddress("SctRho",&SctRho);
   fChain->SetBranchAddress("SctPath",&SctPath);

   fChain->SetBranchAddress("TrtPathInX0",&TrtPathInX0);
   fChain->SetBranchAddress("TrtPathInL0",&TrtPathInL0);
   fChain->SetBranchAddress("TrtA",&TrtA);
   fChain->SetBranchAddress("TrtZ",&TrtZ);
   fChain->SetBranchAddress("TrtRho",&TrtRho);
   fChain->SetBranchAddress("TrtPath",&TrtPath);

   fChain->SetBranchAddress("InDetServPathInX0",&InDetServPathInX0);
   fChain->SetBranchAddress("InDetServPathInL0",&InDetServPathInL0);
   fChain->SetBranchAddress("InDetServA",&InDetServA);
   fChain->SetBranchAddress("InDetServZ",&InDetServZ);
   fChain->SetBranchAddress("InDetServRho",&InDetServRho);
   fChain->SetBranchAddress("InDetServPath",&InDetServPath);

   fChain->SetBranchAddress("InDetPathInX0",&InDetPathInX0);
   fChain->SetBranchAddress("InDetPathInL0",&InDetPathInL0);
   fChain->SetBranchAddress("InDetA",&InDetA);
   fChain->SetBranchAddress("InDetZ",&InDetZ);
   fChain->SetBranchAddress("InDetRho",&InDetRho);
   fChain->SetBranchAddress("InDetPath",&InDetPath);

   fChain->SetBranchAddress("SolenoidPathInX0",&SolenoidPathInX0);
   fChain->SetBranchAddress("SolenoidPathInL0",&SolenoidPathInL0);
   fChain->SetBranchAddress("SolenoidA",&SolenoidA);
   fChain->SetBranchAddress("SolenoidZ",&SolenoidZ);
   fChain->SetBranchAddress("SolenoidRho",&SolenoidRho);
   fChain->SetBranchAddress("SolenoidPath",&SolenoidPath);

   fChain->SetBranchAddress("LArPathInX0",&LArPathInX0);
   fChain->SetBranchAddress("LArPathInL0",&LArPathInL0);
   fChain->SetBranchAddress("LArA",&LArA);
   fChain->SetBranchAddress("LArZ",&LArZ);
   fChain->SetBranchAddress("LArRho",&LArRho);
   fChain->SetBranchAddress("LArPath",&LArPath);

   fChain->SetBranchAddress("TilePathInX0",&TilePathInX0);
   fChain->SetBranchAddress("TilePathInL0",&TilePathInL0);
   fChain->SetBranchAddress("TileA",&TileA);
   fChain->SetBranchAddress("TileZ",&TileZ);
   fChain->SetBranchAddress("TileRho",&TileRho);
   fChain->SetBranchAddress("TilePath",&TilePath);

   fChain->SetBranchAddress("CaloServPathInX0",&CaloServPathInX0);
   fChain->SetBranchAddress("CaloServPathInL0",&CaloServPathInL0);
   fChain->SetBranchAddress("CaloServA",&CaloServA);
   fChain->SetBranchAddress("CaloServZ",&CaloServZ);
   fChain->SetBranchAddress("CaloServRho",&CaloServRho);
   fChain->SetBranchAddress("CaloServPath",&CaloServPath);

   fChain->SetBranchAddress("CaloPathInX0",&CaloPathInX0);
   fChain->SetBranchAddress("CaloPathInL0",&CaloPathInL0);
   fChain->SetBranchAddress("CaloA",&CaloA);
   fChain->SetBranchAddress("CaloZ",&CaloZ);
   fChain->SetBranchAddress("CaloRho",&CaloRho);
   fChain->SetBranchAddress("CaloPath",&CaloPath);

   fChain->SetBranchAddress("TotalPathInX0",&TotalPathInX0);
   fChain->SetBranchAddress("TotalPathInL0",&TotalPathInL0);
   fChain->SetBranchAddress("TotalA",&TotalA);
   fChain->SetBranchAddress("TotalZ",&TotalZ);
   fChain->SetBranchAddress("TotalRho",&TotalRho);
   fChain->SetBranchAddress("TotalPath",&TotalPath);

   Notify();
}

Bool_t MaterialMapperTree::Notify()
{

   // Get branch pointers
   b_Eta = fChain->GetBranch("Eta");
   b_Phi = fChain->GetBranch("Phi");

   b_BeamPipePathInX0 = fChain->GetBranch("BeamPipePathInX0");
   b_BeamPipePathInL0 = fChain->GetBranch("BeamPipePathInL0");
   b_BeamPipeA = fChain->GetBranch("BeamPipeA");
   b_BeamPipeZ = fChain->GetBranch("BeamPipeZ");
   b_BeamPipeRho = fChain->GetBranch("BeamPipeRho");
   b_BeamPipePath = fChain->GetBranch("BeamPipePath");

   b_PixelPathInX0 = fChain->GetBranch("PixelPathInX0");
   b_PixelPathInL0 = fChain->GetBranch("PixelPathInL0");
   b_PixelA = fChain->GetBranch("PixelA");
   b_PixelZ = fChain->GetBranch("PixelZ");
   b_PixelRho = fChain->GetBranch("PixelRho");
   b_PixelPath = fChain->GetBranch("PixelPath");

   b_SctPathInX0 = fChain->GetBranch("SctPathInX0");
   b_SctPathInL0 = fChain->GetBranch("SctPathInL0");
   b_SctA = fChain->GetBranch("SctA");
   b_SctZ = fChain->GetBranch("SctZ");
   b_SctRho = fChain->GetBranch("SctRho");
   b_SctPath = fChain->GetBranch("SctPath");

   b_TrtPathInX0 = fChain->GetBranch("TrtPathInX0");
   b_TrtPathInL0 = fChain->GetBranch("TrtPathInL0");
   b_TrtA = fChain->GetBranch("TrtA");
   b_TrtZ = fChain->GetBranch("TrtZ");
   b_TrtRho = fChain->GetBranch("TrtRho");
   b_TrtPath = fChain->GetBranch("TrtPath");

   b_InDetServPathInX0 = fChain->GetBranch("InDetServPathInX0");
   b_InDetServPathInL0 = fChain->GetBranch("InDetServPathInL0");
   b_InDetServA = fChain->GetBranch("InDetServA");
   b_InDetServZ = fChain->GetBranch("InDetServZ");
   b_InDetServRho = fChain->GetBranch("InDetServRho");
   b_InDetServPath = fChain->GetBranch("InDetServPath");

   b_InDetPathInX0 = fChain->GetBranch("InDetPathInX0");
   b_InDetPathInL0 = fChain->GetBranch("InDetPathInL0");
   b_InDetA = fChain->GetBranch("InDetA");
   b_InDetZ = fChain->GetBranch("InDetZ");
   b_InDetRho = fChain->GetBranch("InDetRho");
   b_InDetPath = fChain->GetBranch("InDetPath");

   b_SolenoidPathInX0 = fChain->GetBranch("SolenoidPathInX0");
   b_SolenoidPathInL0 = fChain->GetBranch("SolenoidPathInL0");
   b_SolenoidA = fChain->GetBranch("SolenoidA");
   b_SolenoidZ = fChain->GetBranch("SolenoidZ");
   b_SolenoidRho = fChain->GetBranch("SolenoidRho");
   b_SolenoidPath = fChain->GetBranch("SolenoidPath");

   b_LArPathInX0 = fChain->GetBranch("LArPathInX0");
   b_LArPathInL0 = fChain->GetBranch("LArPathInL0");
   b_LArA = fChain->GetBranch("LArA");
   b_LArZ = fChain->GetBranch("LArZ");
   b_LArRho = fChain->GetBranch("LArRho");
   b_LArPath = fChain->GetBranch("LArPath");

   b_TilePathInX0 = fChain->GetBranch("TilePathInX0");
   b_TilePathInL0 = fChain->GetBranch("TilePathInL0");
   b_TileA = fChain->GetBranch("TileA");
   b_TileZ = fChain->GetBranch("TileZ");
   b_TileRho = fChain->GetBranch("TileRho");
   b_TilePath = fChain->GetBranch("TilePath");

   b_CaloServPathInX0 = fChain->GetBranch("CaloServPathInX0");
   b_CaloServPathInL0 = fChain->GetBranch("CaloServPathInL0");
   b_CaloServA = fChain->GetBranch("CaloServA");
   b_CaloServZ = fChain->GetBranch("CaloServZ");
   b_CaloServRho = fChain->GetBranch("CaloServRho");
   b_CaloServPath = fChain->GetBranch("CaloServPath");

   b_CaloPathInX0 = fChain->GetBranch("CaloPathInX0");
   b_CaloPathInL0 = fChain->GetBranch("CaloPathInL0");
   b_CaloA = fChain->GetBranch("CaloA");
   b_CaloZ = fChain->GetBranch("CaloZ");
   b_CaloRho = fChain->GetBranch("CaloRho");
   b_CaloPath = fChain->GetBranch("CaloPath");

   b_TotalPathInX0 = fChain->GetBranch("TotalPathInX0");
   b_TotalPathInL0 = fChain->GetBranch("TotalPathInL0");
   b_TotalA = fChain->GetBranch("TotalA");
   b_TotalZ = fChain->GetBranch("TotalZ");
   b_TotalRho = fChain->GetBranch("TotalRho");
   b_TotalPath = fChain->GetBranch("TotalPath");

   return kTRUE;
}

void MaterialMapperTree::Show(Long64_t entry)
{  if (!fChain) return;
   fChain->Show(entry);}


Int_t MaterialMapperTree::Cut(Long64_t entry)
{return 1;}

