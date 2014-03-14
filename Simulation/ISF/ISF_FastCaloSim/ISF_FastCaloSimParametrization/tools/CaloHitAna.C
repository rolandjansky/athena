/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define CaloHitAna_cxx
#include "CaloHitAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <TLorentzVector.h>

void CaloHitAna::Loop()
{
  std::cout <<"In loop: "<<std::endl;
//   In a ROOT session, you can do:
//      Root > .L CaloHitAna.C
//      Root > CaloHitAna t
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
   static const Int_t MAX_LAYER = 25; //can be fixed
   Float_t layer_e[MAX_LAYER];
   Float_t layer_e_abs[MAX_LAYER]; //absolute values of energies
   Float_t layer_depth[MAX_LAYER];
   Float_t truthEnergy = -1;
   Float_t total_depth = -1;
   Float_t total_e_abs = -1;
   Float_t layer_e_total = -1;
   TString name="output_layers_";
   name+=ffilename;
   TFile *fout =TFile::Open(name,"RECREATE");
   TTree *t = new TTree("T","Energy per layer");
   for (Int_t i=0; i<MAX_LAYER; i++)
     {
       TString branchname="layer_e_";
       branchname+=i;
       t->Branch(branchname, &(layer_e[i]));
       branchname="layer_showerdepth_";
       branchname+=i;
       t->Branch(branchname, &(layer_depth[i]));
     }
   t->Branch("truth_e",&truthEnergy);
   t->Branch("average_showerdepth",&total_depth);
   t->Branch("layer_e_total",&layer_e_total);
   Long64_t nentries = fChain->GetEntriesFast();
   std::cout <<nentries<<std::endl;
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      //Zero everything for each event
      std::cout <<jentry<<std::endl;
      for (Int_t i=0; i<MAX_LAYER; i++)
	{
	  layer_e[i] = 0.;
	  layer_e_abs[i] = 0.;
	  layer_depth[i] = 0.;
	}
      truthEnergy = -1;
      layer_e_total = 0.0;
      
      if (! (TruthE->size()))
        {
	  continue; //ignore 0 truth size
        }
      else 
        {
	  TLorentzVector truthVector;
	  truthVector.SetPxPyPzE((*TruthPx)[0],(*TruthPy)[0],(*TruthPz)[0],(*TruthE)[0]);
	  //truthVector.Print();
	  truthEnergy = (*TruthE)[0];
	  std::cout <<"Hit size: "<<HitE->size()<<std::endl;
	  for (UInt_t i=0; i<HitE->size(); i++)
	    {
	      Int_t layer = (*HitSampling)[i];
	      if (layer!=-1)
		{
		  Float_t correct_e = (*HitE)[i];
		  Float_t sampfrac =  (*HitSamplingFraction)[i];
		  Float_t hitr = sqrt( ( (*HitX)[i] * (*HitX)[i])+ ((*HitY)[i] * (*HitY)[i])+((*HitZ)[i] * (*HitZ)[i]));
		  //std::cout <<hitr<<std::endl;
		  //if (layer==0 || layer==4 || layer>20 || (layer>=8 && layer<=11) || (layer == 16)) std::cout <<i<<" layer: "<<layer<<" E: "<<correct_e<<"  SF: "<<sampfrac<<std::endl; 
		  if (!((*HitIsTile)[i])) 
		    {
		      if (fabs(sampfrac)>1e-6)
			{
			  correct_e/=sampfrac;
			}
		      else correct_e = 0.;
		    }
		  else correct_e*=sampfrac;
		  layer_e[layer]+=correct_e;
		  layer_e_abs[layer]+=fabs(correct_e);
		  
		  layer_depth[layer]+=(fabs(correct_e)*hitr);
		}
	      
	    }
	  total_depth = 0.;
	  total_e_abs = 0.;
	  layer_e_total = 0.;
	  //Save/print here
	  std::cout <<"Event: "<<jentry<<std::endl;
	  for (Int_t i=0; i<MAX_LAYER; i++)
	    {
	      //if (jentry %100 == 0)
		{
		  std::cout <<" Layer: "<<i<<" E: "<<layer_e[i]<<" fraction: "<<layer_e[i]/truthEnergy<<std::endl;
		}
	      total_depth+=layer_depth[i];
	      total_e_abs+=layer_e_abs[i];
	      layer_depth[i]/=layer_e_abs[i];
	      layer_e_total+=layer_e[i];
	    }
	  total_depth/=total_e_abs;
	  //don't put truth energy in last layer now:
	  //layer_e[MAX_LAYER-1] = truthEnergy;
	  std::cout <<"----------------------------"<<std::endl;
	  
	}
      t->Fill();
   }
   t->Write();
   fout->Close();
   
}
