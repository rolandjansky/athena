/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//macro to read in D3PD files and run some tools
//should get same results as running MissingETPerformance package over AOD/ESD/DPD
//to run it:
//root runD3PD_withSelection.C
//
#include <TFile.h>
#include <TChain.h>
#include <iostream>
using namespace std;

void runD3PD_withSelection() {
  //compile libraries and load them
  gROOT->LoadMacro("D3PD.h+");
  gROOT->LoadMacro("MissingETData.C+");
  gROOT->LoadMacro("BasicPlotsTool.C+");
  gROOT->LoadMacro("ResolutionTool.C+");
  gROOT->LoadMacro("MissingETEventSelector.C+");

  //input files
  TChain *chain = new TChain("JetMETD3PD");
  //chain->Add("/scratch/ayurkewi/METPerformance.silvia.fromkoji.root");
  chain->Add("/scratch/ayurkewi/fromkojiMETD3PD.root");

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
  TFile *out = new TFile("METPerformance.fromD3PD.root", "RECREATE");

  //create event selectors
  MissingETEventSelector METEvtSel;
  METEvtSel.initialize(metFromD3PD);
  METEvtSel._selectionList.push_back("passesN90CellsCut");
  METEvtSel._selectionList.push_back("hasMinPerJetEMfraction");

  MissingETEventSelector METEvtSel2;
  METEvtSel2.initialize(metFromD3PD);
  METEvtSel2._selectionList.push_back("passesN90CellsCut");
  METEvtSel2._selectionList.push_back("hasMinPerJetEMfraction");
  METEvtSel2._selectionList.push_back("passesMBTSCut");
  METEvtSel2._selectionList.push_back("passesCALOCut");

  //create our histogram-making tools
  //give them the output file pointer to create directories and histograms
  BasicPlotsTool basicTool;
  basicTool.m_iset=5001;
  basicTool.m_nbinp=40;
  basicTool.m_suptmi=40.;
  basicTool.m_nbinE=100;
  basicTool.m_lowET=-5.;
  basicTool.m_nbinpxy=80;
  basicTool.m_suptmixy=40.;
  basicTool.m_nbinphi=63;
  basicTool.m_binphi=3.15;
  basicTool.m_nbinsig=500;
  basicTool.m_binsig=50.;
  basicTool.m_nbineta=50;
  basicTool.m_bineta=5.0;
  basicTool.m_folderName="nocut";
  basicTool.m_suET=200.+basicTool.m_lowET;
  basicTool.initialize(out);

  //make copies
  BasicPlotsTool basicTool1(basicTool);
  BasicPlotsTool basicTool2(basicTool);
  BasicPlotsTool basicTool3(basicTool);
  BasicPlotsTool basicTool4(basicTool);
  //BasicPlotsTool basicTool5(basicTool);
  BasicPlotsTool basicTool6(basicTool);
  BasicPlotsTool basicTool7(basicTool);

  basicTool1.m_folderName = "MBTS";
  basicTool2.m_folderName = "emfraccut";
  basicTool3.m_folderName = "n90cut";
  basicTool4.m_folderName = "n90_and_emfrac_cuts";
  //basicTool5.m_folderName = "GOODNESS";
  basicTool6.m_folderName = "CALO";
  basicTool7.m_folderName = "n90_emfrac_MBTS_CALO_cuts";

  basicTool1.initialize(out);
  basicTool2.initialize(out);
  basicTool3.initialize(out);
  basicTool4.initialize(out);
  //basicTool5.initialize(out);
  basicTool6.initialize(out);
  basicTool7.initialize(out);

  ResolutionTool resolutionTool;
  resolutionTool.m_numptt=20;
  resolutionTool.m_startpt=0.;
  resolutionTool.m_binpt=5.;
  resolutionTool.m_scares=20.;
  resolutionTool.m_curvasres=10.;
  resolutionTool.m_fitrange=3;
  resolutionTool.m_ns=2.;
  resolutionTool.m_percerr=0.15;
  resolutionTool.m_entries=100;
  resolutionTool.m_iset=5001;
  resolutionTool.m_nbinres1=100;
  resolutionTool.m_scares1=50.;
  resolutionTool.m_nbinres2=100;
  resolutionTool.m_scares2=500.;
  resolutionTool.m_nbinres3=200;
  resolutionTool.m_scares3=100.;
  resolutionTool.m_nbinphires1=315;
  resolutionTool.m_lowET=-5.;
  resolutionTool.m_suET=95.;
  resolutionTool.m_useTruth=0;
  resolutionTool.m_use_sumet=2;
  resolutionTool.m_folderName="nocut";
  resolutionTool.initialize(out);

  //make copies
  ResolutionTool resolutionTool1(resolutionTool);
  ResolutionTool resolutionTool2(resolutionTool);
  ResolutionTool resolutionTool3(resolutionTool);
  ResolutionTool resolutionTool4(resolutionTool);
  //ResolutionTool resolutionTool5(resolutionTool);
  ResolutionTool resolutionTool6(resolutionTool);
  ResolutionTool resolutionTool7(resolutionTool);

  resolutionTool1.m_folderName = "MBTS";
  resolutionTool2.m_folderName = "emfraccut";
  resolutionTool3.m_folderName = "n90cut";
  resolutionTool4.m_folderName = "n90_and_emfrac_cuts";
  //resolutionTool5.m_folderName = "GOODNESS";
  resolutionTool6.m_folderName = "CALO";
  resolutionTool7.m_folderName = "n90_emfrac_MBTS_CALO_cuts";

  resolutionTool1.initialize(out);
  resolutionTool2.initialize(out);
  resolutionTool3.initialize(out);
  resolutionTool4.initialize(out);
  //resolutionTool5.initialize(out);
  resolutionTool6.initialize(out);
  resolutionTool7.initialize(out);

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

    if (METEvtSel.passesMBTSCut()) {
      basicTool1.execute(&data);
      resolutionTool1.execute(&data);
    }
    if (METEvtSel.hasMinPerJetEMfraction()) {
      basicTool2.execute(&data);
      resolutionTool2.execute(&data);
    }
    if (METEvtSel.passesN90CellsCut()) {
      basicTool3.execute(&data);
      resolutionTool3.execute(&data);
    }
    if (METEvtSel.passes_L1_MBTS_1_1() && METEvtSel.ANDedSelection()) {
      basicTool4.execute(&data);
      resolutionTool4.execute(&data);
    }

    if (METEvtSel.passesCALOCut()) {
      basicTool6.execute(&data);
      resolutionTool6.execute(&data);
    }

    if (METEvtSel.passes_L1_MBTS_1_1() && METEvtSel2.ANDedSelection()) {
      basicTool7.execute(&data);
      resolutionTool7.execute(&data);
    }
  }//done loop over all entries

  //write output root file
  out->Write();
  out->Close();

}//end runD3PD_withSelection()
