/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//macro to read in D3PD files and run some tools
//should get same results as running MissingETPerformance package over AOD/ESD/DPD
//to run it:
//root runD3PD.C
//
#include <TFile.h>
#include <TChain.h>
#include <iostream>
using namespace std;

void runD3PD() {
  //compile libraries and load them
  gROOT->LoadMacro("D3PD.h+");
  gROOT->LoadMacro("MissingETData.C+");
  gROOT->LoadMacro("BasicPlotsTool.C+");
  gROOT->LoadMacro("ResolutionTool.C+");
  gROOT->LoadMacro("LinearityTool.C+");

  //input files
  TChain *chain = new TChain("metPerf");
  chain->Add("/scratch/ayurkewi/METPerformance.d3pd.test.root");

  //pass the input file chain to our data structure
  //The D3PD class is made from MakeClass, with only very small modifications
  //If we are using a D3PD with different variables, it should be simple to use MakeClass
  //on the new files and not have to change much
  D3PD *metFromD3PD = new D3PD(chain);
  //MissingETData class mimics MissingETPerformance/MissingETData class
  //to present (almost) the same interface
  //We use this intermediary data structure to avoid having to change much when we switch to
  //a different D3PD with different variables
  //The MissingET variables should be the same, and we just point to those in this class
  MissingETData data(metFromD3PD);

  //create the output file for our histograms
  TFile *out = new TFile("output.root", "RECREATE");

  //create our histogram-making tools
  //give them the output file pointer to create directories and histograms
  BasicPlotsTool basicTool;
  //uncomment any variables to set them to something different
  //basicTool.m_iset=5200;
  //basicTool.m_nbinp=250;
  //basicTool.m_suptmi=500.;
  //basicTool.m_nbinE=200;
  //basicTool.m_lowET=-5.;
  //basicTool.m_nbinpxy=500;
  //basicTool.m_suptmixy=250.;
  //basicTool.m_nbinphi=63;
  //basicTool.m_binphi=3.15;
  //basicTool.m_nbinsig=500;
  //basicTool.m_binsig=50.;
  //basicTool.m_nbineta=50;
  //basicTool.m_bineta=5.0;
  //basicTool.m_folderName="";
  basicTool.initialize(out);

  ResolutionTool resolutionTool;
  //uncomment any variables to set them to something different
  //resolutionTool.m_numptt=80;
  //resolutionTool.m_startpt=5.;
  //resolutionTool.m_binpt=20.;
  //resolutionTool.m_scares=100.;
  //resolutionTool.m_curvasres=30.;
  //resolutionTool.m_fitrange=3;
  //resolutionTool.m_ns=2.;
  //resolutionTool.m_percerr=0.15;
  //resolutionTool.m_entries=100;
  //resolutionTool.m_iset=5013;
  //resolutionTool.m_nbinres1=100;
  //resolutionTool.m_scares1=100.;
  //resolutionTool.m_nbinres2=100;
  //resolutionTool.m_scares2=500.;
  //resolutionTool.m_nbinres3=200;
  //resolutionTool.m_scares3=100.;
  //resolutionTool.m_nbinphires1=315;
  //resolutionTool.m_lowET=-5.;
  //resolutionTool.m_suET=1595.;
  //resolutionTool.m_useTruth=1;
  //resolutionTool.m_use_sumet=1;
  //resolutionTool.m_folderName="";
  resolutionTool.initialize(out);

  LinearityTool linearityTool;
  //uncomment any variables to set them to something different
  //linearityTool.m_lnumptt = 40;
  //linearityTool.m_lstartpt=0.;  
  //linearityTool.m_lbinpt=5.;    
  //linearityTool.m_fitrangel=2;  
  //linearityTool.m_lnbini=100;    
  //linearityTool.m_percerr=0.15; 
  //linearityTool.m_entries=50;   
  //linearityTool.m_curvaslin=0.3;
  //linearityTool.m_fitr=2.5;     
  //linearityTool.m_linorm=1;
  //linearityTool.m_bin_rp3=1.;
  //linearityTool.m_suptmi=300.;
  //linearityTool.m_etmi_threshold=50.;
  //linearityTool.m_folderName="";
  linearityTool.initialize(out);

  //get number of entries in the input chain
  Long64_t nentries = chain->GetEntriesFast();

  //loop over all entries
  for (Long64_t jentry=0; jentry < nentries; ++jentry) {
    //load the current event
    Long64_t ientry = metFromD3PD->LoadTree(jentry);
    if (ientry < 0) break;
    chain->GetEntry(jentry);

    //print out what event we are processing once in a while
    if (jentry%1000==0)  cout << "event " << jentry << endl;

    //call execute() for each of our tools to fill histograms 
    basicTool.execute(&data);
    resolutionTool.execute(&data);
    linearityTool.execute(&data);

  }//done loop over all entries

  //write output root file
  out->Write();
  out->Close();

}//end runD3PD()
