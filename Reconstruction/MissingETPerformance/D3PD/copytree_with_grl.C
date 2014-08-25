/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//macro to read in D3PD files and run some tools
//should get same results as running MissingETPerformance package over AOD/ESD/DPD
//to run it:
//root runD3PD_withSelection.C
//
#include <TFile.h>
#include <TEventList.h>
#include <TChain.h>
#include <iostream>
#include <fstream>
using namespace std;

void copytree_with_grl() {
  //compile libraries and load them
  gSystem->Load("/afs/cern.ch/user/a/ayurkewi/scratch0/GRL/DataQuality/GoodRunsLists/i686-slc5-gcc43-opt/libGoodRunsListsLib.so");
  gROOT->LoadMacro("D3PD.h+");
  gROOT->LoadMacro("MissingETData.C+");
  gROOT->LoadMacro("MissingETEventSelector.C+");

  Root::TGoodRunsListReader reader;
  reader.AddXMLFile("GRLs/data10_7TeV.periodA.152166-153200_LBSUMM_DetStatus-v03-repro04-01_jetetmiss_7TeV.xml");
  reader.AddXMLFile("GRLs/data10_7TeV.periodB.153565-155160_LBSUMM_DetStatus-v03-repro04-01_jetetmiss_7TeV.xml");
  reader.AddXMLFile("GRLs/data10_7TeV.periodC.155228-156682_LBSUMM_DetStatus-v03-pass1-analysis-2010C_data-jetetmiss_7TeV.xml");
  reader.Interpret();
  Root::TGoodRunsList m_grl = reader.GetMergedGoodRunsList() ;

  bool grl(D3PD* filedata2,Root::TGoodRunsList &m_grl){ return  m_grl.HasRunLumiBlock(filedata2->RunNumber,filedata2->lbn); }

  //input files
  TChain *chain = new TChain("qcd");

  //chain->Add("/scratch2/ayurkewi/tails/MET_Tails.root");
  chain->Add("skim.root");

  D3PD *metFromD3PD = new D3PD(chain);
  MissingETData data(metFromD3PD);

  //create event selector
  MissingETEventSelector METEvtSel;
  METEvtSel.initialize(metFromD3PD);

  //get number of entries in the input chain
  Long64_t nentries = chain->GetEntries();
  cout << "Processing " << nentries << " events." << endl;

  TEventList t;

  //loop over all entries
  for (Long64_t jentry=0; jentry < nentries; ++jentry) {
 
    //load the current event
    Long64_t ientry = metFromD3PD->LoadTree(jentry);
    if (ientry < 0) break;
    chain->GetEntry(jentry);

    //print out what event we are processing once in a while
    if (jentry%1000==0)  cout << "event " << jentry << endl;

    //"GRL"
    if  (grl(metFromD3PD,m_grl)){ 
      
      //"COLLCAND"
      if ( METEvtSel.passesL1J5() && METEvtSel.passesVertex() && 
	   (METEvtSel.passesMBTSTime() || METEvtSel.passesCaloTime()) ) {
       
	// "COLLCAND_CLEANING
	if (METEvtSel.passesN90AndHECFracCut() && METEvtSel.passes_fbadQCutAndEmFracCut() 
	    && METEvtSel.passes_JetTime()){

          if (METEvtSel.passes_hecf_quality()) {

	    //if (metFromD3PD-> MET_Topo_et > 40000 || metFromD3PD-> MET_LocHadTopo_et > 40000 || metFromD3PD-> MET_CorrTopo_et > 40000) {
	    if (metFromD3PD-> MET_Topo_et > 60000) {

	      t.Enter(jentry);

	    }//outlier
	  }
	}//chiudo cleaning
      }//chiudo collcand
    }//chiudoGRL
  }//done loop over all entries

  cout << "writing output file" << endl;

  //write skimmed root file
  chain->SetEventList(&t);
  TFile *f2 = new TFile("met_topo_gt60.root","recreate");
  TTree *small = chain->CopyTree("");
  small->Write();
  small->Print();
  f2->Close();

}//end copytree_with_grl
