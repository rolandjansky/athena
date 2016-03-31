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

//another example how to read matched ntuples

void CaloHitAna3(TString filename="output1.root", Int_t llayer=0, Long64_t max_entries=-1, float timing_cut=99999)
{
  gSystem->Load("./FCS_Cell_h.so");
  TFile *fin = TFile::Open(filename,"READ");
  if (!fin) abort();
  TTree *t = (TTree*) fin->Get("FCS_ParametrizationInput");
  if (!t) abort();
  TString foutname = filename;
  TString outname="_output_";
  outname+=llayer;
  outname+=".root";
  foutname.ReplaceAll(".root",outname);

  TFile *fout = new TFile(foutname, "RECREATE");


  FCS_matchedcellvector *vec;
  TString name="Sampling_";
  name+=llayer;
  if (llayer == -1)
    {
      name="AllCells";
    }
  std::cout <<"Reading branch: "<<name<<" for layer: "<<llayer<<std::endl;
  t->SetBranchAddress(name,&vec);
     
  std::vector<float> *truth_px=0;
  std::vector<float> *truth_py = 0;
  std::vector<float> *truth_pz = 0;
  std::vector<float> *truth_e = 0;
  
  
  t->SetBranchAddress("TruthE",&truth_e);
  t->SetBranchAddress("TruthPx",&truth_px);
  t->SetBranchAddress("TruthPy",&truth_py);
  t->SetBranchAddress("TruthPz",&truth_pz);
  
  std::cout <<"Entries: "<<t->GetEntries()<<std::endl;
  if (max_entries == -1 ) 
    {
      max_entries = t->GetEntries();
    }
  
  for (Long64_t i=0; i<max_entries; i++)
    {
      //std::cout <<llayer<<std::endl;
      if (i%100 ==0) std::cout<<std::endl;
      t->GetEntry(i);
      //if (i%100 ==0) std::cout <<"Event: "<<i<<" layer "<<layer<< " "<<name<<" has: "<<vec->size()<<" cells"<<std::endl;
      //sort cells and hits
      vec->sort();
      //trim timing
      if (timing_cut<999999.)
	{
	  if (i%100==0) std::cout <<"Trimming: orig size: "<<vec->size()<<" cells"<<std::endl;
	  vec->time_trim(timing_cut);
	  if (i%100==0) std::cout <<"Remaining non empty cells: "<<vec->size()<<std::endl;
	}
      else
	{
	  if (i==0) std::cout<<"No time trimming"<<std::endl;
	}
      
      if (i%100 ==0)
	{
	  std::cout <<"======================================"<<std::endl;
	  std::cout <<"=          EVENT  "<<i<<"               ="<<std::endl;
	  std::cout <<"======================================"<<std::endl;
	  std::cout <<"Layer: "<<llayer<<" : "<<vec->size()<< " cells"<<std::endl;
	 
	  float totalhitelayer = 0.;
	  float totalcellelayer = 0.;
	  for (UInt_t j=0; j<vec->size(); j++)
	    {
	      FCS_matchedcell cell = (*vec)[j];
	      FCS_cell ccell = cell.cell;
	      float ccelle = ccell.energy;
	      UInt_t hitsize = cell.hit.size();
	      std::cout <<"  Cell "<<j<<" E: "<<ccell.energy<<" id: "<<ccell.cell_identifier<<" hits: "<<hitsize<<std::endl;
	      float tothite=0.;
	      totalcellelayer+=ccell.energy;	      
	      for (UInt_t k=0; k<hitsize; k++)
		{
		  FCS_hit hit = cell.hit[k];
		  float hite = hit.hit_energy;
		  float hitt = hit.hit_time;
		  //		  float hitt = (*vec)[j].hit[k].hit_time;
		  tothite+=hite;
		  totalhitelayer+=hite;
		  if (i%100 ==0) std::cout <<"     Hit "<<k<<" HitE: "<<hite<<" Time: "<<hitt<<std::endl;
		  if (i%100 ==0 && hitt>1000.) std::cout <<"HERE"<<std::endl;
		}

	      float frac = 0.;
	      //long emptycell =0;
	      if (fabs(ccelle)>0.)
		{
		  frac = tothite/ccelle;
		}
	      //	      else emptycell++;
	      std::cout <<"     Total hitE in cell: "<<tothite<<" / cellE: "<<ccelle<<"  ( "<<frac<<" ) "<<std::endl;//<<" empty cells: "<<emptycell<<std::endl;
		      
	    }
	  //	  std::cout <<"----"<<std::endl;
	  //float frac2 = 0.;
	  //if (fabs(totalcellelayer)>0.) frac2= totalhitelayer/totalcellelayer;
	  std::cout <<"Total check: layer: SUMhit: "<<totalhitelayer<<" / SUMcell: "<<totalcellelayer<<std::endl;//" ( "<<frac2<<" ) "<<std::endl;
	  std::cout <<"======================================"<<std::endl;
	  std::cout <<std::endl;
	}
      
      /*
      UInt_t totalhits=0;
      UInt_t totalg4hits=0;
      Float_t totalhitE = 0.; //these are per layer!
      Float_t totalcellE = 0.;
      Float_t totalg4hitE = 0.0;
      for (UInt_t j=0; j<(*vec).size(); j++)
	{
	  if (i%100 ==0)
	    {
	      std::cout <<"Hit size: "<<(*vec)[j].hit.size()<<" g4hit size: "<<(*vec)[j].g4hit.size()<<std::endl; 
	      std::cout <<"Reading in cell: "<<j<<std::endl;
	    }
	  totalhits+=(*vec)[j].hit.size();
	  totalg4hits+=(*vec)[j].g4hit.size();
	  for (UInt_t k=0; k<(*vec)[j].hit.size(); k++)
	    {
	      if (i%100 ==0) std::cout <<"Reading hit: "<<k<<" e: "<<(*vec)[j].hit[k].hit_energy<<" tot: "<<totalhitE<<std::endl;
	      totalhitE+=(*vec)[j].hit[k].hit_energy;
	    }
	  for (UInt_t k=0; k<(*vec)[j].g4hit.size(); k++)
	    {
	      if (i%100 ==0) std::cout <<"Reading hit: "<<k<<" e: "<<(*vec)[j].g4hit[k].hit_energy<<" tot: "<<totalg4hitE<<std::endl;
	      totalg4hitE+=(*vec)[j].g4hit[k].hit_energy;
	    }
	  totalcellE+=(*vec)[j].cell.energy;
	}
      if (i%100 ==0) std::cout <<"Test? "<<totalhits<<" hits and "<<totalg4hits<<" g4hits with C: "<<total_cell_energy<<" / "<<totalcellE<<" H: "<<total_hit_energy<<" / "<<totalhitE<<" G4: "<<total_g4hit_energy<<" / "<<totalg4hitE<<std::endl;
      */
      TLorentzVector pion;
      if (truth_e->size()>=1)
	{
	  pion.SetPxPyPzE((*truth_px)[0], (*truth_py)[0], (*truth_pz)[0], (*truth_e)[0]); 
	}
      else continue;
    }    
  fout->cd();
  fout->Close();

};

int main(int argc, char *argv[])
{
  TString filename="output1.root";
  Int_t layer =2;
  float timing_cut=999999;
  Long64_t max_entries = -1;
  std::cout <<"Usage: ./CaloHitAna3 inputfile sampling max_entries timing_cut"<<std::endl;
  if (argc>=2) 
    {
      filename=argv[1];
    }
  if (argc>=3)
    {
      TString param = argv[2];
      layer = param.Atoi();
    }
  if (argc>=4)
    {
      TString param = argv[3];
      max_entries = param.Atoi();
    }
  if (argc>=5)
    {
      TString param = argv[4];
      timing_cut = param.Atof();
    }
  std::cout <<"Parameters: inputfile: "<<filename<<" sampling: "<<layer<<" max entries: "<<max_entries<<" timing cut: "<<timing_cut<<std::endl;
  CaloHitAna3(filename, layer,max_entries,timing_cut);
  return 0;
}
