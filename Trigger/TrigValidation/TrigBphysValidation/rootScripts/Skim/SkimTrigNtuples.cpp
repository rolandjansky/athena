/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// SkimTrigNtuples class
///---------------------------------------------------
/// Skim the trigger ntuples with trigger or pass-through
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of TriggerJpsiAnalysis
///===================================================

#include "SkimTrigNtuples.h"
#include "TTree.h"

///===================================================
void SkimTrigNtuples::skim(string settings,vector<string> triggersIn) {
  Log log("SkimTrigNtuples::skim", ol());

  log.info("Settings: " + settings);

  string outFileName = "skim.root";
  getValue(settings, "OutFileName=",outFileName);

  std::string trackCollName = "UsedTracks";
  getValue(settings, "TrackCollName=",trackCollName);
  std::string muonCollName = "Muons";
  getValue(settings, "MuonCollName=",muonCollName);

  // create selection algorithm
  CutsSkim cuts(ol(), settings);

  //loop over all entries and all files
  log.info("******************************************************************");
  log.info("** BEGINNING OF THE LOOP                                        **");
  log.info("******************************************************************");

  //loop over files
  resetIterator();
  while( iterateFiles() ) {
    //load trees
    TTree* tJpsi    = loadTree("Jpsi");
    TTree* tTrig    = loadTree("TRIG");
    TTree* tTracks  = loadTree("UsedTracks");
    TTree* tMuons   = loadTree("Muons");

    std::vector<TTree*> tChains;
    std::vector<string> triggers;

    vector<string>::iterator triggersItr = triggersIn.begin();
    for(; triggersItr!=triggersIn.end(); ++triggersItr) {
      log.info("Loading " + (*triggersItr));
      TTree* tmp = loadTree(*triggersItr);
      if( tmp!= NULL) {
        tChains.push_back( tmp );
        triggers.push_back( *triggersItr );
      }
    }
    log.debug("Trees loaded");

    //check that all the trees are loaded correctly
    if(tJpsi == NULL || tTrig == NULL || tTracks == NULL || tMuons == NULL || tChains.size() == 0) {
      log.warning("Some of the trees were not loaded");
      addCorruptedFile(currentFileName());
      continue;
    }

    //skip files with no entries
    if(tJpsi->GetEntries() == 0) {
      log.debug("No entries, skipping file");
      continue;
    }

    //prepare NtupleReader
    log.debug("Preparing ntuples");
    if(tTrig->GetEntries() > 0)   tTrig  ->BuildIndex("TRIG_runNumber", "TRIG_eventNumber");
    if(tMuons->GetEntries() > 0)  tMuons ->BuildIndex((muonCollName+"_runNumber").c_str(), (muonCollName+"_eventNumber").c_str());
    if(tTracks->GetEntries() > 0) tTracks->BuildIndex((trackCollName+"_runNumber").c_str(), (trackCollName+"_eventNumber").c_str());
    log.debug("done");

    // trigger
    std::vector<std::string>* trigger1 = new std::vector<std::string>;
    std::vector<std::string>* trigger2 = new std::vector<std::string>;
    tTrig->SetBranchAddress("TRIG_EF_trigger_name", &trigger1);  // main trigger selection
    tTrig->SetBranchAddress("TRIG_L2_trigger_name", &trigger2);  // secondary trigger selection

    // trigger pass-through
    std::vector<std::string>* passThrough = new std::vector<std::string>;
    if( settings.find("UseEFPassThrough") != std::string::npos) {
      log.info("UseEFPassThrough");
      tTrig->SetBranchAddress("TRIG_EF_pass_through", &passThrough);
    }else{
      tTrig->SetBranchAddress("TRIG_L2_pass_through", &passThrough);
    }

    // make BCompositeParticle & chain
    BCompositeParticle* Jpsi      = new BCompositeParticle(tJpsi, "Jpsi");

    /// create output file
    TFile* outFile = TFile::Open((currentFileName() + "." + outFileName).c_str(), "RECREATE");

    // clone trees
    TTree* tJpsiClone    = tJpsi->CloneTree(0);
    TTree* tTrigClone    = tTrig->CloneTree(0);
    TTree* tTracksClone  = tTracks->CloneTree(0);
    TTree* tMuonsClone   = tMuons->CloneTree(0);

    std::vector<TTree*> tChainClones;

    vector<TTree*>::iterator tChainsItr  = tChains.begin();
    for(; tChainsItr!=tChains.end(); ++tChainsItr) {
      tChainClones.push_back( (*tChainsItr)->CloneTree(0) );
    }

    // event info
    int oldRun = -1;
    int oldEvt = -1;

    //loop over entries
    int step = tJpsi->GetEntries()/3;
    for(int entry = 0; entry < (int)tJpsi->GetEntries(); ++entry) {
      if(step == 0 || entry%step == 0) log.info("   ===> Entry " + iToStr(entry));

      // load BCompositeParticles using NtupleReader
      Jpsi->GetEntry(entry);

      vector<TTree*>::iterator tChainsItr  = tChains.begin();
      for(; tChainsItr!=tChains.end(); ++tChainsItr) {
        (*tChainsItr)->GetEntry(entry);
      }

      int trigIndex   = tTrig  ->GetEntryNumberWithIndex(Jpsi->runNumber, Jpsi->eventNumber);
      int muonsIndex  = tMuons ->GetEntryNumberWithIndex(Jpsi->runNumber, Jpsi->eventNumber);
      int tracksIndex = tTracks->GetEntryNumberWithIndex(Jpsi->runNumber, Jpsi->eventNumber);
      if( trigIndex==-1 || muonsIndex==-1 || tracksIndex==-1) {
        log.error("Event not found: ");
        log.error("  trigIndex=="+iToStr(trigIndex));
        log.error("  muonsIndex=="+iToStr(muonsIndex));
        log.error("  tracksIndex=="+iToStr(tracksIndex));
        continue;
      }

      tTrig  ->GetEntry(trigIndex);
      tMuons ->GetEntry(muonsIndex);
      tTracks->GetEntry(tracksIndex);

      /// level-1 jpsi =============================================================

      if( !cuts.selectTrigger(trigger1, trigger2, passThrough) ) continue;
      if( !cuts.selectGRL(Jpsi->runNumber, Jpsi->lumiBlock) ) continue;

      /// fill clones =============================================================
      tJpsiClone  ->Fill();

      vector<TTree*>::iterator tChainClonesItr  = tChainClones.begin();
      for(; tChainClonesItr!=tChainClones.end(); ++tChainClonesItr) {
        (*tChainClonesItr)->Fill();
      }

      // save only unique events for Tracks, Muons and trig. decisions
      if(oldRun!=Jpsi->runNumber || oldEvt!=Jpsi->eventNumber) {
        tTrigClone  ->Fill();
        tTracksClone->Fill();
        tMuonsClone ->Fill();

        oldEvt = oldEvt!=Jpsi->eventNumber;
        oldRun = oldRun!=Jpsi->runNumber;
      }

    }

    /// save file
    outFile->cd();

//     tJpsiClone  ->Write();
//     tTrigClone  ->Write();
//     tTracksClone->Write();
//     tMuonsClone ->Write();
//
//     vector<TTree*>::iterator tChainClonesItr  = tChainClones.begin();
//     for(; tChainClonesItr!=tChainClones.end(); ++tChainClonesItr) {
//       (*tChainClonesItr)->Write();
//     }

    outFile->Write();
    outFile->Close();

    log.info("File saved: " + currentFileName() + "." + outFileName);

    /// cleanup

    delete Jpsi  ;
/*    delete tJpsi  ;

    delete trigger1;
    delete trigger2;
    delete passThrough;
    delete tTrig;

    delete tTracks;
    delete tMuons;

    delete tJpsiClone  ;
    delete tTrigClone  ;
    delete tTracksClone;
    delete tMuonsClone ;

    tChainsItr       = tChains.begin();
    vector<TTree*>::iterator tChainClonesItr  = tChainClones.begin();
    for(; tChainClonesItr!=tChainClones.end(); ++tChainsItr, ++tChainClonesItr) {
      delete *tChainsItr;
      delete *tChainClonesItr;
    }*/
  }
}

///===================================================
///      . .
///====o==V==o========================================
