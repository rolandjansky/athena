/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloHitAna.C"
#include <iostream>
#include <stdlib.h>
void run(TString filename="ISF_HitAnalysispion_eta1.root",TString outputname="output1.root", std::vector<Int_t> settings =std::vector<Int_t>(), Float_t TimingCut=999999., Int_t debug = 0)
{
  //CaloHitAna t;
  CaloHitAna t(filename,outputname, settings, TimingCut,debug,0);
  t.Loop();
  //No need to call finish now
}


int main(int argc, char *argv[])
{
  TString filename="ISF_HitAnalysispion_eta1.root";
  TString outputname="output1.root";
  if (argc <=1)
    {
      std::cout <<"Usage: ./CaloHitAna input.root output.root save_all_cells save_cells_per_sampling save_energies_per_sampling hit_timing_cut"<<std::endl;
      exit(1);
    }
  if (argc >=2)
    {
      filename = argv[1];
    }      
  std::cout <<"Processing "<<filename<<std::endl;
    
  if (argc >=3)
    {
      outputname = argv[2];
    }     
  std::cout <<"Saving output to "<<outputname<<std::endl;
  std::vector<Int_t> settings(0);
  Float_t TimingCut = 999999.;
  Int_t Debug = 0;
  if (argc >=4)
    {
      for (Int_t i=3; i<argc; i++)
	{
	  TString parameter = argv[i];
	  if (! (parameter.Contains("tc") || parameter.Contains("debug")))
	    {
	      Int_t pari = parameter.Atoi();
	      settings.push_back(pari);
	    }
	  else if (parameter.Contains("tc"))
	    {
	      parameter.ReplaceAll("tc=","");
	      TimingCut = parameter.Atof();
	    }
	  else if (parameter.Contains("debug"))
	    {
	      parameter.ReplaceAll("debug=","");
	      Debug = parameter.Atoi();
	      std::cout <<Debug<<std::endl;
	    }
	}
    }
  else
    {
      settings.push_back(1);
    }
  std::cout <<"Settings: ";
  for (UInt_t i=0; i<settings.size(); i++)
    {
      std::cout <<settings[i]<<" ";
    }
  std::cout <<std::endl;
  std::cout <<"Timing cut: "<<TimingCut<<std::endl;
  std::cout <<"Debug: "<<Debug<<std::endl;
//  TFile *ftest = TFile::Open(filename,"READ");
//  if (!ftest)
//    {
//      std::cout <<"File "<<filename<<"doesn't exist!"<<std::endl;
//      exit(1);
//    }
//  else
//    {
//      ftest->Close();
//    }
//  delete ftest;
//  std::cout <<std::endl;
  std::cout <<"Running"<<std::endl;
  run(filename, outputname, settings, TimingCut,Debug);

  return 0;
}
