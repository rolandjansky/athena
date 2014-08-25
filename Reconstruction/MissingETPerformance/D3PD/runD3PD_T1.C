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
#include <fstream>
using namespace std;

void runD3PD_T1(){
  //compile libraries and load them
  gROOT->LoadMacro("D3PD.h+");
  gROOT->LoadMacro("MissingETData.C+");
  gROOT->LoadMacro("BasicPlotsTool.C+");
  gROOT->LoadMacro("ResolutionTool.C+");
  gROOT->LoadMacro("LinearityTool.C+");
  gROOT->LoadMacro("MissingETEventSelector.C+");

  fstream file_counters;
  file_counters.open("EventCounters_T1.txt", ios::out);

  //input files
  //  TChain *chain = new TChain("JetMETD3PD");
  TChain *chain = new TChain("qcd");
  //chain->Add("/scratch2/ayurkewi/mc09_7TeV.105200.T1_McAtNlo_Jimmy.merge.NTUP_JETMET.e510_s765_s767_r1302_p235_tid160642_00/NTUP_JETMET.160642._0000*");
  chain->Add("/scratch2/ayurkewi/user.mtesta.mc09_7TeV.105200.T1_McAtNlo_Jimmy.merge.NTUP_JETMET.e510_s765_s767_r1302_p235.100902093129/user.mtesta.000102._0000*");

  //pass the input file chain to our data structure
  //The D3PD class is made from MakeClass, with only very small modifications
  //If we are using a 3PD with different variables, it should be simple to use MakeClass
  //on the new files and not have to change much

  D3PD *metFromD3PD = new D3PD(chain);
  //MissingETData class mimics MissingETPerformance/MissingETData class
  //to present (almost) the same interface
  //We use this intermediary data structure to avoid having to change much when we switch to
  //a different D3PD with different variables
  //The MissingET variables should be the same, and we just point to those in this class

  MissingETData data(metFromD3PD);
  //create the output file for our histograms
  TFile *out = new TFile("METPerformance.T1.root", "RECREATE");

  //create event selectors
  MissingETEventSelector METEvtSel;
  METEvtSel.initialize(metFromD3PD);

 //create our histogram-making tools
  //give them the output file pointer to create directories and histograms
  LinearityTool linearitytool;

  //linearitytool.m_lnumptt = 60;
  //linearitytool.m_lstartpt=0.;
  //linearitytool.m_lbinpt=5.;
  //linearitytool.m_fitrangel=2;
  //linearitytool.m_lnbini=100;
  //linearitytool.m_percerr=0.15;
  //linearitytool.m_entries=50;
  //linearitytool.m_curvaslin=0.3;
  //linearitytool.m_fitr=2.5;
  //linearitytool.m_linorm=1;
  //linearitytool.m_bin_rp3=1.;
  //linearitytool.m_suptmi=300.;
  //linearitytool.m_etmi_threshold=50.;
  //linearitytool.m_folderName="";
  linearitytool.initialize(out);

  //make copies
  LinearityTool linearitytool2(linearitytool);

  linearitytool2.m_folderName = "COLLCAND_CLEANING_CUTS";
  linearitytool2.initialize(out);

  //get number of entries in the input chain
  Long64_t nentries = chain->GetEntries();
  //nentries = 2500;
  cout << "Processing " << nentries << " events." << endl;

  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("MET*",1);

  chain->SetBranchStatus("RunNumber",1);
  chain->SetBranchStatus("EventNumber",1);
  chain->SetBranchStatus("lbn",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_n",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_n90",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_emscale_pt",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_pt",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_eta",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_phi",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_hecf",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_emfrac",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_timing",1);
  chain->SetBranchStatus("jet_AntiKt4H1Topo_quality",1);
  chain->SetBranchStatus("vxp_n",1);
  chain->SetBranchStatus("vxp_x",1);
  chain->SetBranchStatus("vxp_y",1);
  chain->SetBranchStatus("vxp_z",1);
  chain->SetBranchStatus("vxp_nTracks",1);
  chain->SetBranchStatus("vxp_type",1);
  chain->SetBranchStatus("collcand_passMBTSTime",1);
  chain->SetBranchStatus("collcand_passCaloTime",1);

  unsigned int i_GRL = 0;
  unsigned int i_COLLCAND = 0;
  unsigned int i_COLLCAND_CLEANING=0;
  unsigned int i_COLLCAND_CLEANING_1VERTEX=0;
  unsigned int i_COLLCAND_CLEANING_ONEJET=0;
  unsigned int i_COLLCAND_CLEANING_ONEJETGT20=0;
  unsigned int i_COLLCAND_CLEANING_TWOJETGT20=0;
  unsigned int num_outliers = 0;

   //loop over all entries
  for (Long64_t jentry=0; jentry < nentries; ++jentry) {
  
    //load the current event
    Long64_t ientry = metFromD3PD->LoadTree(jentry);
    if (ientry < 0) break;
    //chain->GetEntry(jentry);
    chain->GetEntry(jentry,0);

    //print out what event we are processing once in a while
    if (jentry%100==0)  cout << "event " << jentry << endl;

    //call execute() for each of our tools to fill histograms 
    //nocuts
    linearitytool.execute(&data);

    i_GRL++;

    //"COLLCAND"
    if (   METEvtSel.passesMBTSTime() || METEvtSel.passesCaloTime() ) {

      i_COLLCAND++;

      // "COLLCAND_CLEANING
      if (METEvtSel.passesN90AndHECFracCut() && METEvtSel.passes_fbadQCutAndEmFracCut() 
	  && METEvtSel.passes_JetTime() && METEvtSel.passes_hecf_quality()){

	i_COLLCAND_CLEANING++;
      
	linearitytool2.execute(&data);

      }//chiudo cleaning
    }//chiudo collcand
  }//done loop over all entries

  file_counters << "Total number of events                    = " << nentries << endl;
  file_counters << "passed GRL                                = " << i_GRL << endl;
  file_counters << "passed COLLCAND                           = " << i_COLLCAND  << endl;
  file_counters << "passed COLLCAND + CLEANING                = " << i_COLLCAND_CLEANING << endl;

  file_counters.close();
  file_counters.clear();

  cout << "writing output histogram file" << endl;

  //write output root file
  out->Write();
  out->Close();

}//end runD3PD_withSelection()
