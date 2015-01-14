/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCS_Cell.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <string>
#include <sstream>
#include <iostream>
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"
#include <stdlib.h>
#include "TLorentzVector.h"
#include "TH2D.h"

void CaloHitAna2(TString filename="output1.root", Int_t layer=0)
{
  gSystem->Load("FCS_Cell_h.so");
  TFile *fin = TFile::Open(filename,"READ");
  if (!fin) abort();
  TTree *t = (TTree*) fin->Get("FCS_ParametrizationInput");
  if (!t) abort();
  TString foutname = filename;
  TString outname=TString::Format("_output_%i.root",layer);
  foutname.ReplaceAll(".root",outname);

  TFile *fout = new TFile(foutname, "RECREATE");
  TH2D *h1 = new TH2D("h1","total_cell_energy",100,-5,5,600,0,60000);
  TH2D *h2 = new TH2D("h2","total_hit_energy",100,-5,5,600,0,60000);
  TH2D *h3 = new TH2D("h3","total_g4hit_energy",100,-5,5,600,0,60000);
  TH2D *hh1 = new TH2D("hh1","nhits",100,-5,5,1000,0,10000);
  TH2D *hh2 = new TH2D("hh2","ng4hits",100,-5,5,1000,0,10000);

  TH2D *hdiff1 = new TH2D("hdiff1","cell-hit_energy",100,-5,5,200,-10000,10000);
  TH2D *hdiff2 = new TH2D("hdiff2","cell-g4hit_energy",100,-5,5,200,-10000,10000);

  TString hdiffL1name; hdiffL1name.Form("hdiff1_%d",layer);
  TString hdiffL2name; hdiffL2name.Form("hdiff2_%d",layer);

  TH2D *hdiffL1 = new TH2D(hdiffL1name,hdiffL1name,100,-5,5,200,-10000,10000);
  TH2D *hdiffL2 = new TH2D(hdiffL2name,hdiffL2name,100,-5,5,200,-10000,10000);

  h1->Sumw2();
  h2->Sumw2();
  h3->Sumw2();
  hh1->Sumw2();
  hh2->Sumw2();
  hdiff1->Sumw2();
  hdiff2->Sumw2();
  hdiffL1->Sumw2();
  hdiffL2->Sumw2();

  FCS_matchedcellvector *vec=0;
  TString name="Sampling_";
  name+=layer;
  if (layer == -1)
    {
      name="AllCells";
    }
  t->SetBranchAddress(name,&vec);
  Float_t total_cell_energy;
  Float_t total_hit_energy;
  Float_t total_g4hit_energy;
  t->SetBranchAddress("total_cell_energy", &total_cell_energy);
  t->SetBranchAddress("total_hit_energy",  &total_hit_energy);
  t->SetBranchAddress("total_g4hit_energy",&total_g4hit_energy);
  std::vector<float> *truth_px=0;
  std::vector<float> *truth_py = 0;
  std::vector<float> *truth_pz = 0;
  std::vector<float> *truth_e = 0;


  t->SetBranchAddress("TruthE",&truth_e);
  t->SetBranchAddress("TruthPx",&truth_px);
  t->SetBranchAddress("TruthPy",&truth_py);
  t->SetBranchAddress("TruthPz",&truth_pz);

  std::cout <<"Entries: "<<t->GetEntries()<<std::endl;
  for (Long64_t i=0; i<t->GetEntries(); i++)
    {
      t->GetEntry(i);
      if (i%100 ==0) std::cout <<"Event: "<<i<<" layer "<<layer<< " has: "<<vec->size()<<" cells"<<std::endl;
      UInt_t totalhits=0;
      UInt_t totalg4hits=0;
      Float_t totalhitE = 0.; //these are per layer!
      Float_t totalcellE = 0.;
      Float_t totalg4hitE = 0.0;
      for (UInt_t j=0; j<(*vec).size(); j++)
	{
	  totalhits+=(*vec)[j].hit.size();
	  totalg4hits+=(*vec)[j].g4hit.size();
	  for (UInt_t k=0; k<(*vec)[j].hit.size(); k++)
	    {
	      totalhitE+=(*vec)[j].hit[k].hit_energy;
	    }
	  for (UInt_t k=0; k<(*vec)[j].g4hit.size(); k++)
	    {
	      totalg4hitE+=(*vec)[j].g4hit[k].hit_energy;
	    }
	  totalcellE+=(*vec)[j].cell.energy;
	}
      TLorentzVector pion;
      /*
      if (truth_e->size()>1)
	{
	  std::cout <<"Truth: "<<truth_e->size()<<" "<<(*truth_px)[0]<<std::endl;
	}
      else std::cout <<truth_e->size()<<std::endl;
      */
      if (truth_e->size()>=1)
	{
	  pion.SetPxPyPzE((*truth_px)[0], (*truth_py)[0], (*truth_pz)[0], (*truth_e)[0]); 
	}
      else continue;
      /*
      std::cout <<std::endl;
      pion.Print();
      std::cout <<std::endl;
      */
      //std::cout <<"Pion rap: "<<pion.Rapidity()<<std::endl;
      h1->Fill(pion.Rapidity(), total_cell_energy);
      h2->Fill(pion.Rapidity(), total_hit_energy);
      h3->Fill(pion.Rapidity(), total_g4hit_energy);
      hh1->Fill(pion.Rapidity(), totalhits);
      hh2->Fill(pion.Rapidity(), totalg4hits);
      hdiff1->Fill(pion.Rapidity(), total_cell_energy-total_hit_energy);
      hdiff2->Fill(pion.Rapidity(), total_cell_energy-total_g4hit_energy);
      hdiffL1->Fill(pion.Rapidity(), totalcellE-totalhitE);
      hdiffL2->Fill(pion.Rapidity(), totalcellE-totalg4hitE);
      if (i%100 ==0) std::cout <<" and "<<totalhits<<" hits and "<<totalg4hits<<" g4hits"<<std::endl;
    }    
  fout->cd();
  h1->Write();
  h2->Write();
  h3->Write();
  hh1->Write();
  hh2->Write();
  hdiff1->Write();
  hdiff2->Write();
  hdiffL1->Write();
  hdiffL2->Write();
  fout->Close();

};

int main(int argc, char *argv[])
{
  TString filename="output1.root";
  Int_t layer =2;
  if (argc <=1)
    {
      std::cout <<"Usage: ./CaloHitAna2 inputfile sampling "<<std::endl;
      exit(1);
    }

  if (argc>=2) 
    {
      filename=argv[1];
    }
  if (argc>=3)
    {
      TString param = argv[2];
      layer = param.Atoi();
    }
  std::cout <<"Parameters: inputfile: "<<filename<<" sampling: "<<layer<<std::endl;
  CaloHitAna2(filename, layer);
  return 0;
}
