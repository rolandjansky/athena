/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "efficiency_step_by_step.h"
#include "TChain.h"
#include "TSystem.h"
#include "TGraphAsymmErrors.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include <vector>

void Init() {
  // define the boundaries for
  nbins = 50;
  maxntracks = 500;
  d0min = -2.;
  d0max = 2.;
  z0min = -110.;
  z0max = 110.;
  phimin = -TMath::Pi();
  phimax = TMath::Pi();
  //phimin = 2.3;
  //phimax = 2.6;
  abscurvmax = .5;
  //etamin = -1.1;
  //etamax = 0.15;
  etamin = -2.5;
  etamax = 2.5;
  ptmax = 100;
  Dd0 = 0.2;
  Drelpt = 1.;
  Dcurv = .05;
  Dphi = .01;
  Dz0 = 1.5;
  Deta = .02;


  // Int_t nptbins = 24;
  // Double_t ptbins[] = {0,1,2,3,4,5,6,8,10,12,14,16,18,20,25,30,35,40,45,50,60,70,80,90,100};
  // TDR binning
  Int_t nptbins = 21;
  Double_t ptbins[] = {0,2,4,6,8,10,12,14,16,18,20,25,30,35,40,45,50,60,70,80,90,100};

  // create instances for the FTK generic distributions

  histopt_ftk = new TH1F("histopt_ftk",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_ftk = new TH1F("histoeta_ftk",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_ftk = new TH1F("histophi_ftk",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_ftk_pre_hw = new TH1F("histopt_ftk_pre_hw",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_ftk_pre_hw = new TH1F("histoeta_ftk_pre_hw",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_ftk_pre_hw = new TH1F("histophi_ftk_pre_hw",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_ftkI = new TH1F("histopt_ftkI",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_ftkI = new TH1F("histoeta_ftkI",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_ftkI = new TH1F("histophi_ftkI",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_ftkI_pre_hw = new TH1F("histopt_ftkI_pre_hw",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_ftkI_pre_hw = new TH1F("histoeta_ftkI_pre_hw",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_ftkI_pre_hw = new TH1F("histophi_ftkI_pre_hw",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_ftk_pattern = new TH1F("histopt_ftk_pattern",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_ftk_pattern = new TH1F("histoeta_ftk_pattern",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_ftk_pattern = new TH1F("histophi_ftk_pattern",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_ftk_hits = new TH1F("histopt_ftk_hits",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_ftk_hits = new TH1F("histoeta_ftk_hits",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_ftk_hits = new TH1F("histophi_ftk_hits",";#phi;N Tracks",nbins,phimin,phimax);

  // create instances for the fakes distribtuions

  histopt_goodftk = new TH1F("histopt_goodftk",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftk = new TH1F("histoeta_goodftk",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftk = new TH1F("histophi_goodftk",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftk_pre_hw = new TH1F("histopt_goodftk_pre_hw",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftk_pre_hw = new TH1F("histoeta_goodftk_pre_hw",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftk_pre_hw = new TH1F("histophi_goodftk_pre_hw",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftkI = new TH1F("histopt_goodftkI",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftkI = new TH1F("histoeta_goodftkI",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftkI = new TH1F("histophi_goodftkI",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftkI_pre_hw = new TH1F("histopt_goodftkI_pre_hw",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftkI_pre_hw = new TH1F("histoeta_goodftkI_pre_hw",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftkI_pre_hw = new TH1F("histophi_goodftkI_pre_hw",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftk_pattern = new TH1F("histopt_goodftk_pattern",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftk_pattern = new TH1F("histoeta_goodftk_pattern",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftk_pattern = new TH1F("histophi_goodftk_pattern",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftk_hits = new TH1F("histopt_goodftk_hits",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftk_hits = new TH1F("histoeta_goodftk_hits",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftk_hits = new TH1F("histophi_goodftk_hits",";#phi;N Tracks",nbins,phimin,phimax);


  histopt_goodftkU = new TH1F("histopt_goodftkU",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftkU = new TH1F("histoeta_goodftkU",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftkU = new TH1F("histophi_goodftkU",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftkU_pre_hw = new TH1F("histopt_goodftkU_pre_hw",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftkU_pre_hw = new TH1F("histoeta_goodftkU_pre_hw",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftkU_pre_hw = new TH1F("histophi_goodftkU_pre_hw",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftkUI = new TH1F("histopt_goodftkUI",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftkUI = new TH1F("histoeta_goodftkUI",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftkUI = new TH1F("histophi_goodftkUI",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftkUI_pre_hw = new TH1F("histopt_goodftkUI_pre_hw",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftkUI_pre_hw = new TH1F("histoeta_goodftkUI_pre_hw",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftkUI_pre_hw = new TH1F("histophi_goodftkUI_pre_hw",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftkU_pattern = new TH1F("histopt_goodftkU_pattern",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftkU_pattern = new TH1F("histoeta_goodftkU_pattern",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftkU_pattern = new TH1F("histophi_goodftkU_pattern",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_goodftkU_hits = new TH1F("histopt_goodftkU_hits",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_goodftkU_hits = new TH1F("histoeta_goodftkU_hits",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_goodftkU_hits = new TH1F("histophi_goodftkU_hits",";#phi;N Tracks",nbins,phimin,phimax);

  // create instances for the truth and efficiency distributions

  histopt_truth = new TH1F("histopt_truth",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_truth = new TH1F("histoeta_truth",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_truth = new TH1F("histophi_truth",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_truthM = new TH1F("histopt_truthM",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_truthM = new TH1F("histoeta_truthM",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_truthM = new TH1F("histophi_truthM",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_truthM_pre_hw = new TH1F("histopt_truthM_pre_hw",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_truthM_pre_hw = new TH1F("histoeta_truthM_pre_hw",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_truthM_pre_hw = new TH1F("histophi_truthM_pre_hw",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_truthMI = new TH1F("histopt_truthMI",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_truthMI = new TH1F("histoeta_truthMI",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_truthMI = new TH1F("histophi_truthMI",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_truthMI_pre_hw = new TH1F("histopt_truthMI_pre_hw",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_truthMI_pre_hw = new TH1F("histoeta_truthMI_pre_hw",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_truthMI_pre_hw = new TH1F("histophi_truthMI_pre_hw",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_truthM_pattern = new TH1F("histopt_truthM_pattern",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_truthM_pattern = new TH1F("histoeta_truthM_pattern",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_truthM_pattern = new TH1F("histophi_truthM_pattern",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_truthM_hits = new TH1F("histopt_truthM_hits",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_truthM_hits = new TH1F("histoeta_truthM_hits",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_truthM_hits = new TH1F("histophi_truthM_hits",";#phi;N Tracks",nbins,phimin,phimax);

  histopt_truthM_cond = new TH1F("histopt_truthM_cond",";p_{T} (GeV);N Tracks",nptbins,ptbins);
  histoeta_truthM_cond = new TH1F("histoeta_truthM_cond",";#eta;N Tracks",nbins,etamin,etamax);
  histophi_truthM_cond = new TH1F("histophi_truthM_cond",";#phi;N Tracks",nbins,phimin,phimax);

  ientry2=0;
}


void Process(Long64_t ientry) {

  t_ftkdata->GetEntry(ientry);

  // collect information from the run-evt from the truth
  Int_t evtnumber_ftk = tracks->eventNumber();
  Int_t runnumber_ftk = tracks->runNumber();

  if (ientry%10==0) {
    Info("Process","Event %lld, (Run,Evt) = (%d,%d)",ientry, runnumber_ftk, evtnumber_ftk);
  }

  // first try to start from last number
  for (; ientry2 < t_truth->GetEntries(); ientry2++) {
    t_evtinfo->GetEntry(ientry2);
    if (EventNumber==evtnumber_ftk && RunNumber==runnumber_ftk) break;
  }

  // only enter this loop if we didn't find it above
  if (ientry2 == t_truth->GetEntries()) {
    for (ientry2 = 0; ientry2 < t_truth->GetEntries(); ientry2++) {
      t_evtinfo->GetEntry(ientry2);
      if (EventNumber==evtnumber_ftk && RunNumber==runnumber_ftk) break;
    }
  }

  if (ientry2 == t_truth->GetEntries()) {
    ientry2=0;
    return;
  }

  t_truth->GetEntry(ientry2);

  // Study the list of the FTK tracks to collect elements for the tracks efficiency
  Int_t ntracks = tracks->getNTracks();
  Int_t ntracks_pre_hw = tracks->getNTracks_pre_hw();
  Int_t ntracksI = tracks->getNTracksI();
  Int_t ntracksI_pre_hw = tracks->getNTracksI_pre_hw();
  Int_t ntracks_pattern = tracks->getNTracks_pattern();
  Int_t ntracks_hits = tracks->getNTracks_hits();

  FTKBarcodeMM ftkmatchinfo;
  FTKBarcodeMM ftkmatchinfo_pre_hw;
  FTKBarcodeMM ftkmatchinfoI;
  FTKBarcodeMM ftkmatchinfoI_pre_hw;
  FTKBarcodeMM ftkmatchinfo_pattern;
  FTKBarcodeMM ftkmatchinfo_hits;

  const FTKTrack *curtrack;


  // loop over the 2nd-stage FTK tracks
    for (Int_t itrk=0;itrk!=ntracks;++itrk) { 

      curtrack = tracks->getTrack(itrk);

      double temp_phi = curtrack->getPhi();
      double temp_z0 = curtrack->getZ0();
      double thisz0 = temp_z0 + ((cos(temp_phi) *dx - sin(temp_phi)*dy))*curtrack->getCotTheta();

      double d0 = curtrack->getIP();
      double z0 = thisz0;
      double pt = TMath::Abs(curtrack->getPt())*1e-3;
      double eta = curtrack->getEta();
      double phi = curtrack->getPhi();
      double curv = curtrack->getHalfInvPt()*.5e3;
      
      histopt_ftk->Fill(pt);
      histoeta_ftk->Fill(eta);
      histophi_ftk->Fill(phi);

      if (curtrack->getEventIndex()==0) {
        // Information on FTK tracks relative to the hard-scattering events
        // are collected in a vector and later used to build matching maps
        if (curtrack->getBarcodeFrac()>.5) {
          // ftkmatchinfo.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),curtrack->getEventIndex()),curtrack));
          ftkmatchinfo.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),0),curtrack));
        }
      }

      // apply some basic quality selection
      if (d0<d0min || d0>d0max) continue;
      if (z0<z0min || z0>z0max) continue;
      if (phi<phimin || phi > phimax) continue;
      if (curv<-abscurvmax || curv>abscurvmax) continue;
      if (eta<etamin || eta>etamax) continue;

      histopt_goodftk->Fill(pt);
      histoeta_goodftk->Fill(eta);
      histophi_goodftk->Fill(phi);

      if (curtrack->getBarcodeFrac()<.5) {
        histopt_goodftkU->Fill(pt);
        histoeta_goodftkU->Fill(eta);
        histophi_goodftkU->Fill(phi);
      }

    } // end loop over the 2nd-stage FTK tracks


    // loop over the 2nd-stage FTK tracks before Hit Warrior
    for (Int_t itrk=0;itrk!=ntracks_pre_hw;++itrk) { 

      curtrack = tracks->getTrack_pre_hw(itrk);

      double temp_phi = curtrack->getPhi();
      double temp_z0 = curtrack->getZ0();
      double thisz0 = temp_z0 + ((cos(temp_phi) *dx - sin(temp_phi)*dy))*curtrack->getCotTheta();

      double d0 = curtrack->getIP();
      double z0 = thisz0;
      double pt = TMath::Abs(curtrack->getPt())*1e-3;
      double eta = curtrack->getEta();
      double phi = curtrack->getPhi();
      double curv = curtrack->getHalfInvPt()*.5e3;
      
      histopt_ftk_pre_hw->Fill(pt);
      histoeta_ftk_pre_hw->Fill(eta);
      histophi_ftk_pre_hw->Fill(phi);

      if (curtrack->getEventIndex()==0) {
        // Information on FTK tracks relative to the hard-scattering events
        // are collected in a vector and later used to build matching maps
        if (curtrack->getBarcodeFrac()>.5) {
          // ftkmatchinfo_pre_hw.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),curtrack->getEventIndex()),curtrack));
          ftkmatchinfo_pre_hw.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),0),curtrack));
        }
      }

      // apply some basic quality selection
      if (d0<d0min || d0>d0max) continue;
      if (z0<z0min || z0>z0max) continue;
      if (phi<phimin || phi > phimax) continue;
      if (curv<-abscurvmax || curv>abscurvmax) continue;
      if (eta<etamin || eta>etamax) continue;

      histopt_goodftk_pre_hw->Fill(pt);
      histoeta_goodftk_pre_hw->Fill(eta);
      histophi_goodftk_pre_hw->Fill(phi);

      if (curtrack->getBarcodeFrac()<.5) {
        histopt_goodftkU_pre_hw->Fill(pt);
        histoeta_goodftkU_pre_hw->Fill(eta);
        histophi_goodftkU_pre_hw->Fill(phi);
      }

    } // end loop over the 2nd-stage FTK tracks before Hit Warrior


    // loop over the 1st-stage FTK tracks
    for (Int_t itrk=0;itrk!=ntracksI;++itrk) {

      curtrack = tracks->getTrackI(itrk);

      double temp_phi = curtrack->getPhi();
      double temp_z0 = curtrack->getZ0();
      double thisz0 = temp_z0 + ((cos(temp_phi) *dx - sin(temp_phi)*dy))*curtrack->getCotTheta();

      double d0 = curtrack->getIP();
      double z0 = thisz0;
      double pt = TMath::Abs(curtrack->getPt())*1e-3;
      double eta = curtrack->getEta();
      double phi = curtrack->getPhi();
      double curv = curtrack->getHalfInvPt()*.5e3;

      histopt_ftkI->Fill(pt);
      histoeta_ftkI->Fill(eta);
      histophi_ftkI->Fill(phi);

      if (curtrack->getEventIndex()==0) {
        // Information on FTK tracks relative to the hard-scattering events
        // are collected in a vector and later used to build matching maps
        if (curtrack->getBarcodeFrac()>.5) {
          // ftkmatchinfoI.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),curtrack->getEventIndex()),curtrack));
          ftkmatchinfoI.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),0),curtrack));
        }
      }

      // apply some basic quality selection
      //double d0 = curtrack->getD0();
      if (d0<d0min || d0>d0max) continue;
      if (z0<z0min || z0>z0max) continue;
      if (phi<phimin || phi > phimax) continue;
      if (curv<-abscurvmax || curv>abscurvmax) continue;
      if (eta<etamin || eta>etamax) continue;

      histopt_goodftkI->Fill(pt);
      histoeta_goodftkI->Fill(eta);
      histophi_goodftkI->Fill(phi);

      if (curtrack->getBarcodeFrac()<.5) {
        histopt_goodftkUI->Fill(pt);
        histoeta_goodftkUI->Fill(eta);
        histophi_goodftkUI->Fill(phi);
      }
    } // end loop over the 1st-stage FTK tracks


    // loop over the 1st-stage FTK tracks before Hit Warrior
    for (Int_t itrk=0;itrk!=ntracksI_pre_hw;++itrk) {

      curtrack = tracks->getTrackI_pre_hw(itrk);

      double temp_phi = curtrack->getPhi();
      double temp_z0 = curtrack->getZ0();
      double thisz0 = temp_z0 + ((cos(temp_phi) *dx - sin(temp_phi)*dy))*curtrack->getCotTheta();

      double d0 = curtrack->getIP();
      double z0 = thisz0;
      double pt = TMath::Abs(curtrack->getPt())*1e-3;
      double eta = curtrack->getEta();
      double phi = curtrack->getPhi();
      double curv = curtrack->getHalfInvPt()*.5e3;

      histopt_ftkI_pre_hw->Fill(pt);
      histoeta_ftkI_pre_hw->Fill(eta);
      histophi_ftkI_pre_hw->Fill(phi);

      if (curtrack->getEventIndex()==0) {
        // Information on FTK tracks relative to the hard-scattering events
        // are collected in a vector and later used to build matching maps
        if (curtrack->getBarcodeFrac()>.5) {
          // ftkmatchinfoI_pre_hw.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),curtrack->getEventIndex()),curtrack));
          ftkmatchinfoI_pre_hw.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),0),curtrack));
        }
      }

      // apply some basic quality selection
      //double d0 = curtrack->getD0();
      if (d0<d0min || d0>d0max) continue;
      if (z0<z0min || z0>z0max) continue;
      if (phi<phimin || phi > phimax) continue;
      if (curv<-abscurvmax || curv>abscurvmax) continue;
      if (eta<etamin || eta>etamax) continue;

      histopt_goodftkI_pre_hw->Fill(pt);
      histoeta_goodftkI_pre_hw->Fill(eta);
      histophi_goodftkI_pre_hw->Fill(phi);

      if (curtrack->getBarcodeFrac()<.5) {
        histopt_goodftkUI_pre_hw->Fill(pt);
        histoeta_goodftkUI_pre_hw->Fill(eta);
        histophi_goodftkUI_pre_hw->Fill(phi);
      }
    } // end loop over the 1st-stage FTK tracks before Hit Warrior


    // loop over the FTK tracks with patterns
    for (Int_t itrk=0;itrk!=ntracks_pattern;++itrk) {

      curtrack = tracks->getTrack_pattern(itrk);

      double temp_phi = curtrack->getPhi();
      double temp_z0 = curtrack->getZ0();
      double thisz0 = temp_z0 + ((cos(temp_phi) *dx - sin(temp_phi)*dy))*curtrack->getCotTheta();

      double d0 = curtrack->getIP();
      double z0 = thisz0;
      double pt = TMath::Abs(curtrack->getPt())*1e-3;
      double eta = curtrack->getEta();
      double phi = curtrack->getPhi();
      double curv = curtrack->getHalfInvPt()*.5e3;

      histopt_ftk_pattern->Fill(pt);
      histoeta_ftk_pattern->Fill(eta);
      histophi_ftk_pattern->Fill(phi);

      if (curtrack->getEventIndex()==0) {
        // Information on FTK tracks relative to the hard-scattering events
        // are collected in a vector and later used to build matching maps
        if (curtrack->getBarcodeFrac()>.5) {
          // ftkmatchinfo_pattern.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),curtrack->getEventIndex()),curtrack));
          ftkmatchinfo_pattern.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),0),curtrack));
        }
      }

      // apply some basic quality selection
      //double d0 = curtrack->getD0();
      if (d0<d0min || d0>d0max) continue;
      if (z0<z0min || z0>z0max) continue;
      if (phi<phimin || phi > phimax) continue;
      if (curv<-abscurvmax || curv>abscurvmax) continue;
      if (eta<etamin || eta>etamax) continue;

      histopt_goodftk_pattern->Fill(pt);
      histoeta_goodftk_pattern->Fill(eta);
      histophi_goodftk_pattern->Fill(phi);

      if (curtrack->getBarcodeFrac()<.5) {
        histopt_goodftkU_pattern->Fill(pt);
        histoeta_goodftkU_pattern->Fill(eta);
        histophi_goodftkU_pattern->Fill(phi);
      }
    } // end loop over the FTK tracks with patterns


    // loop over the FTK tracks passing hits requirements
    for (Int_t itrk=0;itrk!=ntracks_hits;++itrk) {

      curtrack = tracks->getTrack_hits(itrk);

      double temp_phi = curtrack->getPhi();
      double temp_z0 = curtrack->getZ0();
      double thisz0 = temp_z0 + ((cos(temp_phi) *dx - sin(temp_phi)*dy))*curtrack->getCotTheta();

      double d0 = curtrack->getIP();
      double z0 = thisz0;
      double pt = TMath::Abs(curtrack->getPt())*1e-3;
      double eta = curtrack->getEta();
      double phi = curtrack->getPhi();
      double curv = curtrack->getHalfInvPt()*.5e3;

      histopt_ftk_hits->Fill(pt);
      histoeta_ftk_hits->Fill(eta);
      histophi_ftk_hits->Fill(phi);

      if (curtrack->getEventIndex()==0) {
        // Information on FTK tracks relative to the hard-scattering events
        // are collected in a vector and later used to build matching maps
        if (curtrack->getBarcodeFrac()>.5) {
          // ftkmatchinfo_hits.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),curtrack->getEventIndex()),curtrack));
          ftkmatchinfo_hits.insert(pair<MatchInfo, const FTKTrack*>(MatchInfo(curtrack->getBarcode(),0),curtrack));
        }
      }

      // apply some basic quality selection
      //double d0 = curtrack->getD0();
      if (d0<d0min || d0>d0max) continue;
      if (z0<z0min || z0>z0max) continue;
      if (phi<phimin || phi > phimax) continue;
      if (curv<-abscurvmax || curv>abscurvmax) continue;
      if (eta<etamin || eta>etamax) continue;

      histopt_goodftk_hits->Fill(pt);
      histoeta_goodftk_hits->Fill(eta);
      histophi_goodftk_hits->Fill(phi);

      if (curtrack->getBarcodeFrac()<.5) {
        histopt_goodftkU_hits->Fill(pt);
        histoeta_goodftkU_hits->Fill(eta);
        histophi_goodftkU_hits->Fill(phi);
      }
    } // end loop over the FTK tracks passing hits requirements


  /*
   * Study the list of the truth tracks to collect elements useful for the tracks
   * efficiency
   */
  vector<FTKTruthTrack>::const_iterator itr = truthTracks->begin();
  vector<FTKTruthTrack>::const_iterator itrE = truthTracks->end();
  for (;itr!=itrE;++itr) { // loop over the truth tracks
    const FTKTruthTrack &curtruth = (*itr);

    int barcode = curtruth.getBarcode();
    if (barcode>100000 || barcode==0) continue;

    // select only good truth tracks
    // double dx = curtruth.getX();
    // double dy = curtruth.getY();
    // double d0 = -1e-3*(dx*py-dy*px)*invpt;

    double px = curtruth.getPX();
    double py = curtruth.getPY();
    double pt = 1e-3*TMath::Sqrt(px*px+py*py);
    double invpt = 1./pt;
    double d0 = curtruth.getD0();
    double z0 = curtruth.getZ();
    double curv = .5*curtruth.getQ()*invpt;
    double phi = curtruth.getPhi();
    double eta = curtruth.getEta();

    if ( pt < ptmincut ) continue;
    if (d0<d0min || d0>d0max) continue;
    if (z0<z0min || z0>z0max) continue;
    if (curv<-abscurvmax || curv>abscurvmax) continue;
    if (phi<phimin || phi>phimax) continue;
    if (eta<etamin || eta>etamax) continue;
    // if (curtruth.getEventIndex()!=0 && curtruth.getQ()==0) continue;
    if (curtruth.getQ()==0) continue;

    // Fill the histogram for the generic truth distribution
    histopt_truth->Fill(pt);
    histoeta_truth->Fill(eta);
    histophi_truth->Fill(phi);

    // match the barcode and event index values
    // MatchInfo reftruth(barcode,curtruth.getEventIndex());
    MatchInfo reftruth(barcode,0);
    pair<FTKBarcodeMM::const_iterator,FTKBarcodeMM::const_iterator> mrange = ftkmatchinfo.equal_range(reftruth);
    pair<FTKBarcodeMM::const_iterator,FTKBarcodeMM::const_iterator> mrange_pre_hw = ftkmatchinfo_pre_hw.equal_range(reftruth);
    pair<FTKBarcodeMM::const_iterator,FTKBarcodeMM::const_iterator> mrangeI = ftkmatchinfoI.equal_range(reftruth);
    pair<FTKBarcodeMM::const_iterator,FTKBarcodeMM::const_iterator> mrangeI_pre_hw = ftkmatchinfoI_pre_hw.equal_range(reftruth);
    pair<FTKBarcodeMM::const_iterator,FTKBarcodeMM::const_iterator> mrange_pattern = ftkmatchinfo_pattern.equal_range(reftruth);
    pair<FTKBarcodeMM::const_iterator,FTKBarcodeMM::const_iterator> mrange_hits = ftkmatchinfo_hits.equal_range(reftruth);

    if (mrange.first != mrange.second) {
      histopt_truthM->Fill(pt);
      histoeta_truthM->Fill(eta);
      histophi_truthM->Fill(phi);      
    }
    if (mrange_pre_hw.first != mrange_pre_hw.second) {
      histopt_truthM_pre_hw->Fill(pt);
      histoeta_truthM_pre_hw->Fill(eta);
      histophi_truthM_pre_hw->Fill(phi);      
    }
    if (mrangeI.first != mrangeI.second) {
      histopt_truthMI->Fill(pt);
      histoeta_truthMI->Fill(eta);
      histophi_truthMI->Fill(phi);
      pair<FTKBarcodeMM::const_iterator,FTKBarcodeMM::const_iterator> mrange_cond = ftkmatchinfo.equal_range(reftruth);
      if (mrange_cond.first != mrange_cond.second) {
        histopt_truthM_cond->Fill(pt);  
        histoeta_truthM_cond->Fill(eta);
        histophi_truthM_cond->Fill(phi);    
      }
    }
    if (mrangeI_pre_hw.first != mrangeI_pre_hw.second) {
      histopt_truthMI_pre_hw->Fill(pt);
      histoeta_truthMI_pre_hw->Fill(eta);
      histophi_truthMI_pre_hw->Fill(phi);     
    }
    if (mrange_pattern.first != mrange_pattern.second) {
      histopt_truthM_pattern->Fill(pt);
      histoeta_truthM_pattern->Fill(eta);
      histophi_truthM_pattern->Fill(phi);     
    }
    if (mrange_hits.first != mrange_hits.second) {
      histopt_truthM_hits->Fill(pt);
      histoeta_truthM_hits->Fill(eta);
      histophi_truthM_hits->Fill(phi);     
    }
  } // end loop over truth tracks

}


void Terminate(std::string& outputname) {

  Info("Terminate","Adding the histograms to the file: %s", outputname.c_str());
  TFile *ofile = TFile::Open(outputname.c_str(), "recreate");

  // calculate some efficiency plots
 
  TGraphAsymmErrors *greffpt = new TGraphAsymmErrors(histopt_truthM,histopt_truth);
  greffpt->SetNameTitle("greffpt",";p_{T} (GeV);2nd-stage Merger Efficiency w.r.t. Truth");
  ofile->Add(greffpt);
  TGraphAsymmErrors *greffeta = new TGraphAsymmErrors(histoeta_truthM,histoeta_truth);
  greffeta->SetNameTitle("greffeta",";#eta;2nd-stage Merger Efficiency w.r.t. Truth");
  ofile->Add(greffeta);
  TGraphAsymmErrors *greffphi = new TGraphAsymmErrors(histophi_truthM,histophi_truth);
  greffphi->SetNameTitle("greffphi",";#phi (rad);2nd-stage Merger Efficiency w.r.t. Truth");
  ofile->Add(greffphi);

  TGraphAsymmErrors *greffpt_pre_hw = new TGraphAsymmErrors(histopt_truthM_pre_hw,histopt_truth);
  greffpt_pre_hw->SetNameTitle("greffpt_pre_hw",";p_{T} (GeV);2nd-stage Fitter Efficiency w.r.t. Truth");
  ofile->Add(greffpt_pre_hw);
  TGraphAsymmErrors *greffeta_pre_hw = new TGraphAsymmErrors(histoeta_truthM_pre_hw,histoeta_truth);
  greffeta_pre_hw->SetNameTitle("greffeta_pre_hw",";#eta;2nd-stage Fitter Efficiency w.r.t. Truth");
  ofile->Add(greffeta_pre_hw);
  TGraphAsymmErrors *greffphi_pre_hw = new TGraphAsymmErrors(histophi_truthM_pre_hw,histophi_truth);
  greffphi_pre_hw->SetNameTitle("greffphi_pre_hw",";#phi (rad);2nd-stage Fitter Efficiency w.r.t. Truth");
  ofile->Add(greffphi_pre_hw);

  TGraphAsymmErrors *greffptI = new TGraphAsymmErrors(histopt_truthMI,histopt_truth);
  greffptI->SetNameTitle("greffptI",";p_{T} (GeV);1nd-stage Merger Efficiency w.r.t. Truth");
  ofile->Add(greffptI);
  TGraphAsymmErrors *greffetaI = new TGraphAsymmErrors(histoeta_truthMI,histoeta_truth);
  greffetaI->SetNameTitle("greffetaI",";#eta;1nd-stage Merger Efficiency w.r.t. Truth");
  ofile->Add(greffetaI);
  TGraphAsymmErrors *greffphiI = new TGraphAsymmErrors(histophi_truthMI,histophi_truth);
  greffphiI->SetNameTitle("greffphiI",";#phi (rad);1nd-stage Merger Efficiency w.r.t. Truth");
  ofile->Add(greffphiI);

  TGraphAsymmErrors *greffptI_pre_hw = new TGraphAsymmErrors(histopt_truthMI_pre_hw,histopt_truth);
  greffptI_pre_hw->SetNameTitle("greffptI_pre_hw",";p_{T} (GeV);1nd-stage Fitter Efficiency w.r.t. Truth");
  ofile->Add(greffptI_pre_hw);
  TGraphAsymmErrors *greffetaI_pre_hw = new TGraphAsymmErrors(histoeta_truthMI_pre_hw,histoeta_truth);
  greffetaI_pre_hw->SetNameTitle("greffetaI_pre_hw",";#eta;1nd-stage Fitter Efficiency w.r.t. Truth");
  ofile->Add(greffetaI_pre_hw);
  TGraphAsymmErrors *greffphiI_pre_hw = new TGraphAsymmErrors(histophi_truthMI_pre_hw,histophi_truth);
  greffphiI_pre_hw->SetNameTitle("greffphiI_pre_hw",";#phi (rad);1nd-stage Fitter Efficiency w.r.t. Truth");
  ofile->Add(greffphiI_pre_hw);

  TGraphAsymmErrors *greffpt_pattern = new TGraphAsymmErrors(histopt_truthM_pattern,histopt_truth);
  greffpt_pattern->SetNameTitle("greffpt_pattern",";p_{T} (GeV);Bank Efficiency w.r.t. Truth");
  ofile->Add(greffpt_pattern);
  TGraphAsymmErrors *greffeta_pattern = new TGraphAsymmErrors(histoeta_truthM_pattern,histoeta_truth);
  greffeta_pattern->SetNameTitle("greffeta_pattern",";#eta;Bank Efficiency w.r.t. Truth");
  ofile->Add(greffeta_pattern);
  TGraphAsymmErrors *greffphi_pattern = new TGraphAsymmErrors(histophi_truthM_pattern,histophi_truth);
  greffphi_pattern->SetNameTitle("greffphi_pattern",";#phi (rad);Bank Efficiency w.r.t. Truth");
  ofile->Add(greffphi_pattern);

  TGraphAsymmErrors *greffpt_hits = new TGraphAsymmErrors(histopt_truthM_hits,histopt_truth);
  greffpt_hits->SetNameTitle("greffpt_hits",";p_{T} (GeV);Configuration Efficiency w.r.t. Truth");
  ofile->Add(greffpt_hits);
  TGraphAsymmErrors *greffeta_hits = new TGraphAsymmErrors(histoeta_truthM_hits,histoeta_truth);
  greffeta_hits->SetNameTitle("greffeta_hits",";#eta;Configuration Efficiency w.r.t. Truth");
  ofile->Add(greffeta_hits);
  TGraphAsymmErrors *greffphi_hits = new TGraphAsymmErrors(histophi_truthM_hits,histophi_truth);
  greffphi_hits->SetNameTitle("greffphi_hits",";#phi (rad);Configuration Efficiency w.r.t. Truth");
  ofile->Add(greffphi_hits);

  TGraphAsymmErrors *greffpt_cond = new TGraphAsymmErrors(histopt_truthM_cond,histopt_truthMI);
  greffpt_cond->SetNameTitle("greffpt_cond",";p_{T} (GeV);2nd-stage Merger Efficiency w.r.t 1st stage");
  ofile->Add(greffpt_cond);
  TGraphAsymmErrors *greffeta_cond = new TGraphAsymmErrors(histoeta_truthM_cond,histoeta_truthMI);
  greffeta_cond->SetNameTitle("greffeta_cond",";#eta;2nd-stage Merger Efficiency w.r.t 1st stage");
  ofile->Add(greffeta_cond);
  TGraphAsymmErrors *greffphi_cond = new TGraphAsymmErrors(histophi_truthM_cond,histophi_truthMI);
  greffphi_cond->SetNameTitle("greffphi_cond",";#phi (rad);2nd-stage Merger Efficiency w.r.t 1st stage");
  ofile->Add(greffphi_cond);


  TGraphAsymmErrors *grfakept = new TGraphAsymmErrors(histopt_goodftkU,histopt_goodftk);
  grfakept->SetNameTitle("grfakept",";p_{T} (GeV);2nd-stage Merger Fake Rate");
  ofile->Add(grfakept);
  TGraphAsymmErrors *grfakeeta = new TGraphAsymmErrors(histoeta_goodftkU,histoeta_goodftk);
  grfakeeta->SetNameTitle("grfakeeta",";#eta;2nd-stage Merger Fake Rate");
  ofile->Add(grfakeeta);
  TGraphAsymmErrors *grfakephi = new TGraphAsymmErrors(histophi_goodftkU,histophi_goodftk);
  grfakephi->SetNameTitle("grfakephi",";#phi (rad);2nd-stage Merger Fake Rate");
  ofile->Add(grfakephi);

  TGraphAsymmErrors *grfakept_pre_hw = new TGraphAsymmErrors(histopt_goodftkU_pre_hw,histopt_goodftk_pre_hw);
  grfakept_pre_hw->SetNameTitle("grfakept_pre_hw",";p_{T} (GeV);2nd-stage Fitter Fake Rate");
  ofile->Add(grfakept_pre_hw);
  TGraphAsymmErrors *grfakeeta_pre_hw = new TGraphAsymmErrors(histoeta_goodftkU_pre_hw,histoeta_goodftk_pre_hw);
  grfakeeta_pre_hw->SetNameTitle("grfakeeta_pre_hw",";#eta;2nd-stage Fitter Fake Rate");
  ofile->Add(grfakeeta_pre_hw);
  TGraphAsymmErrors *grfakephi_pre_hw = new TGraphAsymmErrors(histophi_goodftkU_pre_hw,histophi_goodftk_pre_hw);
  grfakephi_pre_hw->SetNameTitle("grfakephi_pre_hw",";#phi (rad);2nd-stage Fitter Fake Rate");
  ofile->Add(grfakephi_pre_hw);

  TGraphAsymmErrors *grfakeptI = new TGraphAsymmErrors(histopt_goodftkUI,histopt_goodftkI);
  grfakeptI->SetNameTitle("grfakeptI",";p_{T} (GeV);1st-stage Merger Fake Rate");
  ofile->Add(grfakeptI);
  TGraphAsymmErrors *grfakeetaI = new TGraphAsymmErrors(histoeta_goodftkUI,histoeta_goodftkI);
  grfakeetaI->SetNameTitle("grfakeetaI",";#eta;1st-stage Merger Fake Rate");
  ofile->Add(grfakeetaI);
  TGraphAsymmErrors *grfakephiI = new TGraphAsymmErrors(histophi_goodftkUI,histophi_goodftkI);
  grfakephiI->SetNameTitle("grfakephiI",";#phi (rad);1st-stage Merger Fake Rate");
  ofile->Add(grfakephiI);

  TGraphAsymmErrors *grfakeptI_pre_hw = new TGraphAsymmErrors(histopt_goodftkUI_pre_hw,histopt_goodftkI_pre_hw);
  grfakeptI_pre_hw->SetNameTitle("grfakeptI_pre_hw",";p_{T} (GeV);1st-stage Fitter Fake Rate");
  ofile->Add(grfakeptI_pre_hw);
  TGraphAsymmErrors *grfakeetaI_pre_hw = new TGraphAsymmErrors(histoeta_goodftkUI_pre_hw,histoeta_goodftkI_pre_hw);
  grfakeetaI_pre_hw->SetNameTitle("grfakeetaI_pre_hw",";#eta;1st-stage Fitter Fake Rate");
  ofile->Add(grfakeetaI_pre_hw);
  TGraphAsymmErrors *grfakephiI_pre_hw = new TGraphAsymmErrors(histophi_goodftkUI_pre_hw,histophi_goodftkI_pre_hw);
  grfakephiI_pre_hw->SetNameTitle("grfakephiI_pre_hw",";#phi (rad);1st-stage Fitter Fake Rate");
  ofile->Add(grfakephiI_pre_hw);

  TGraphAsymmErrors *grfakept_pattern = new TGraphAsymmErrors(histopt_goodftkU_pattern,histopt_goodftk_pattern);
  grfakept_pattern->SetNameTitle("grfakept_pattern",";p_{T} (GeV);Bank Fake Rate");
  ofile->Add(grfakept_pattern);
  TGraphAsymmErrors *grfakeeta_pattern = new TGraphAsymmErrors(histoeta_goodftkU_pattern,histoeta_goodftk_pattern);
  grfakeeta_pattern->SetNameTitle("grfakeeta_pattern",";#eta;Bank Fake Rate");
  ofile->Add(grfakeeta_pattern);
  TGraphAsymmErrors *grfakephi_pattern = new TGraphAsymmErrors(histophi_goodftkU_pattern,histophi_goodftk_pattern);
  grfakephi_pattern->SetNameTitle("grfakephi_pattern",";#phi (rad);Bank Fake Rate");
  ofile->Add(grfakephi_pattern);

  TGraphAsymmErrors *grfakept_hits = new TGraphAsymmErrors(histopt_goodftkU_hits,histopt_goodftk_hits);
  grfakept_hits->SetNameTitle("grfakept_hits",";p_{T} (GeV);Configuration Fake Rate");
  ofile->Add(grfakept_hits);
  TGraphAsymmErrors *grfakeeta_hits = new TGraphAsymmErrors(histoeta_goodftkU_hits,histoeta_goodftk_hits);
  grfakeeta_hits->SetNameTitle("grfakeeta_hits",";#eta;Configuration Fake Rate");
  ofile->Add(grfakeeta_hits);
  TGraphAsymmErrors *grfakephi_hits = new TGraphAsymmErrors(histophi_goodftkU_hits,histophi_goodftk_hits);
  grfakephi_hits->SetNameTitle("grfakephi_hits",";#phi (rad);Configuration Fake Rate");
  ofile->Add(grfakephi_hits);

  ofile->Add(histopt_truth);
  ofile->Add(histopt_truthM);

  ofile->ls();
  ofile->Write();
  ofile->Close();

}

int main(int argc, char **argv) {
  int events;
  std::string output;
  std::vector<std::string> files;
  ptmincut = 0;
  dx = -0.5;
  dy = -0.5;

  try {
    std::string appName = boost::filesystem::basename(argv[0]);
    // Define and parse the program options
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print this help message")
      ("output,o", po::value<std::string>(&output)->default_value("ftk_efficiency.root"), "The name out of the output file")
      ("events,e", po::value<int>(&events)->default_value(-1), "The number of events to run over. Set to -1 to use all events")
      ("tower,t", po::value<int>(&towerNumber)->default_value(-1), "the number of the tower in ftkdata output data. Use -1 if you have fully merged output")
      ("files", po::value<std::vector<std::string> >(&files)->multitoken(), "FTK NTUP files")
      ("ptmincut", po::value<double>(&ptmincut), "min pt cut on truth tracks")
      ("dx", po::value<double>(&dx)->default_value(-0.5), "dx")
      ("dy", po::value<double>(&dy)->default_value(-0.5), "dx");

    po::variables_map vm;
    try
    {
      po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
      // --help option
      if (vm.count("help"))
      {
        cout << desc << "\n";
        return 1;
      }
      po::notify(vm);
    }
    catch(std::exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      return 1;
    }

    Init();
    t_ftkdata = new TChain("ftkdata");
    t_truth = new TChain("truthtracks");
    t_evtinfo = new TChain("evtinfo");

    // add the input files
    std::vector<std::string>::const_iterator files_it = files.begin();
    for (; files_it != files.end(); ++files_it) {
      t_ftkdata->Add((*files_it).c_str());
      t_truth->Add((*files_it).c_str());
      t_evtinfo->Add((*files_it).c_str());
    }

    t_ftkdata->AddFriend(t_truth);
    t_ftkdata->AddFriend(t_evtinfo);
    t_evtinfo->SetBranchAddress("RunNumber",&RunNumber);
    t_evtinfo->SetBranchAddress("EventNumber",&EventNumber);
    //  t_ftkdata->SetBranchAddress("FTKMergedTracksStream",&tracks);
    if (towerNumber != -1) {
      t_ftkdata->SetBranchAddress(Form("FTKMergedTracksStream%d",towerNumber),&tracks);
    } else {
      t_ftkdata->SetBranchAddress(Form("FTKMergedTracksStream"),&tracks);
    }

    t_truth->SetBranchAddress("TruthTracks",&truthTracks);

    Int_t nloop = t_ftkdata->GetEntries();
    if (events > 0) {
      nloop = events;
    }
    if (nloop > t_ftkdata->GetEntries()) {
      nloop = t_ftkdata->GetEntries();
    }
    for (int ientry = 0; ientry < nloop; ++ientry) {
      Process(ientry);
    }
    Terminate(output);
  } // end try
  catch(std::exception& e)
  {
    std::cerr << "Unhandled Exception reached the top of main: "
              << e.what() << ", application will now exit" << std::endl;
    return 1;
  }
  return 0;
}