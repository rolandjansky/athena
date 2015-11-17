/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "efficiency.h"
#include "TChain.h"
#include "TSystem.h"
#include "TGraphAsymmErrors.h"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"


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

  Int_t nptbins = 21;
  Double_t ptbins[] = {0,2,4,6,8,10,12,14,16,18,20,25,30,35,40,45,50,60,70,80,90,100};

  // create instances for the FTK generic distributions
  histocoordmasketa_ftk = new TH2F("histocoordmasketa_ftk","Track probability of using a coordinate;Coordinate bit;#eta;Prob.",16,0,16,nbins,etamin,etamax);
  histocoordmaskphi_ftk = new TH2F("histocoordmaskphi_ftk","Track probability of using a coordinate;Coordinate bit;#phi;Prob.",16,0,16,nbins,phimin,phimax);
  histonmisseta_ftk = new TH2F("histonmisseta_ftk","Track probability of missing N layers;N Miss;#eta;Prob.",6,0,6,nbins,etamin,etamax);
  histochisqndfeta_ftk = new TH2F("histochisqndfeta_ftk","Track probability of #chi^{2};#chi^{2}/ndf;#eta;Prob.",20,0,20,nbins,etamin,etamax);

  histod0_ftk = new TH1F("histod0_ftk",";d_{0} (mm);N Tracks",nbins,d0min,d0max);
  histoz0_ftk = new TH1F("histoz0_ftk",";z_{0} (mm);N Tracks",nbins,z0min,z0max);
  histophi_ftk = new TH1F("histophi_ftk",";#phi;N Tracks",nbins,phimin,phimax);
  histophid0_ftk = new TH2F("histophid0_ftk",";#phi;d_{0}",nbins,phimin,phimax,nbins,d0min,d0max);
  histocurv_ftk = new TH1F("histocurv_ftk",";1/(2p_{T}) (GeV^{-1});N Tracks",nbins,-abscurvmax,abscurvmax);
  histoeta_ftk = new TH1F("histoeta_ftk",";#eta;N Tracks",nbins,etamin,etamax);
  histoetaphi_ftk = new TH2F("histoetaphi_ftk",";#eta;#phi",nbins,etamin,etamax,nbins,phimin,phimax);
  histontracks_ftk = new TH1F("histontracks_ftk",";N Tracks;N Events",nbins,0,maxntracks);
  histopt_ftk = new TH1F("histopt_ftk",";p_{T} (GeV);N Tracks",nptbins,ptbins);

  histoetaz0_ftk = new TH2F("histoetaz0_ftk with IBL hits",";#eta;z_{0} (mm)",nbins,etamin,etamax,nbins,z0min,z0max);

  // create instances for the fakes distribtuions
  histod0_goodftk = new TH1F("histod0_goodftk",";d_{0} (mm);N Tracks",nbins,d0min,d0max);
  histoz0_goodftk = new TH1F("histoz0_goodftk",";z_{0} (mm);N Tracks",nbins,z0min,z0max);
  histophi_goodftk = new TH1F("histophi_goodftk",";#phi;N Tracks",nbins,phimin,phimax);
  histocurv_goodftk = new TH1F("histocurv_goodftk",";1/(2p_{T}) (GeV^{-1});N Tracks",nbins,-abscurvmax,abscurvmax);
  histoeta_goodftk = new TH1F("histoeta_goodftk",";#eta;N Tracks",nbins,etamin,etamax);
  histontracks_goodftk = new TH1F("histontracks_goodftk",";N Tracks;N Events",nbins,0,maxntracks);
  histopt_goodftk = new TH1F("histopt_goodftk",";p_{T} (GeV);N Tracks",nptbins,ptbins);

  histod0_goodftkU = new TH1F("histod0_goodftkU",";d_{0} (mm);N Tracks",nbins,d0min,d0max);
  histoz0_goodftkU = new TH1F("histoz0_goodftkU",";z_{0} (mm);N Tracks",nbins,z0min,z0max);
  histophi_goodftkU = new TH1F("histophi_goodftkU",";#phi;N Tracks",nbins,phimin,phimax);
  histocurv_goodftkU = new TH1F("histocurv_goodftkU",";1/(2p_{T}) (GeV^{-1});N Tracks",nbins,-abscurvmax,abscurvmax);
  histoeta_goodftkU = new TH1F("histoeta_goodftkU",";#eta;N Tracks",nbins,etamin,etamax);
  histontracks_goodftkU = new TH1F("histontracks_goodftkU",";N Tracks;N Events",nbins,0,maxntracks);
  histopt_goodftkU = new TH1F("histopt_goodftkU",";p_{T} (GeV);N Tracks",nptbins,ptbins);

  // create instances for the truth and efficiency distributions
  histod0_truth = new TH1F("histod0_truth",";d_{0} (mm);N Tracks",nbins,d0min,d0max);
  histoz0_truth = new TH1F("histoz0_truth",";z_{0} (mm);N Tracks",nbins,z0min,z0max);
  histophi_truth = new TH1F("histophi_truth",";#phi;N Tracks",nbins,phimin,phimax);
  histocurv_truth = new TH1F("histocurv_truth",";1/(2p_{T}) (GeV^{-1});N Tracks",nbins,-abscurvmax,abscurvmax);
  histoeta_truth = new TH1F("histoeta_truth",";#eta;N Tracks",nbins,etamin,etamax);
  histontracks_truth = new TH1F("histontracks_truth",";N Tracks;N Events",nbins,0,maxntracks);
  histopt_truth = new TH1F("histopt_truth",";p_{T} (GeV);N Tracks",nptbins,ptbins);

  histoetaphi_truth = new TH2F("histoetaphi_truth",";#eta;#phi",nbins,etamin,etamax,nbins,phimin,phimax);
  histoetaphi_truthM = new TH2F("histoetaphi_truthM",";#eta;#phi",nbins,etamin,etamax,nbins,phimin,phimax);

  histoetaz0_truth = new TH2F("histoetaz0_truth",";#eta;z_{0} (mm)",nbins,etamin,etamax,nbins,z0min,z0max);
  histoetaz0_truthM = new TH2F("histoetaz0_truthM",";#eta;z_{0} (mm)",nbins,etamin,etamax,nbins,z0min,z0max);

  histod0_truthM = new TH1F("histod0_truthM",";d_{0} (mm);N Tracks",nbins,d0min,d0max);
  histoz0_truthM = new TH1F("histoz0_truthM",";z_{0} (mm);N Tracks",nbins,z0min,z0max);
  histophi_truthM = new TH1F("histophi_truthM",";#phi;N Tracks",nbins,phimin,phimax);
  histocurv_truthM = new TH1F("histocurv_truthM",";1/(2p_{T}) (GeV^{-1});N Tracks",nbins,-abscurvmax,abscurvmax);
  histoeta_truthM = new TH1F("histoeta_truthM",";#eta;N Tracks",nbins,etamin,etamax);
  histontracks_truthM = new TH1F("histontracks_truthM",";N Tracks;N Events",nbins,0,maxntracks);
  histopt_truthM = new TH1F("histopt_truthM",";p_{T} (GeV);N Tracks",nptbins,ptbins);

  histod0res = new TH1F("histod0res",";#Delta d_{0} (mm);N Tracks", nbins,-Dd0,Dd0);
  histoz0res = new TH1F("histoz0res",";#Delta z_{0} (mm);N Tracks",nbins,-Dz0,Dz0);
  histocurvres = new TH1F("histocurvres",";#Delta 1/(2p_{T}) (GeV^{-1});Ntracks",nbins,-Dcurv,Dcurv);
  histoetares = new TH1F("histoetares",";#Delta #eta;N Tracks",nbins,-Deta,Deta);
  histophires = new TH1F("histophires",";#Delta #phi;N Tracks",nbins,-Dphi,Dphi);
  historelptres = new TH1F("historelptres",";#Delta p_{T} (GeV);N Tracks",nbins,-Drelpt,Drelpt);
  historelptrespt = new TH2F("historelptrespt",";p_{T} (GeV^{-1});#Delta p_{T} (GeV);N Tracks",nptbins,ptbins,nbins,-Drelpt,Drelpt);

  // create instances for the truth muon to evaluate efficiency and other distributions
  histod0_truth_muon = new TH1F("histod0_truth_muon",";d_{0} (mm);N Tracks",nbins,d0min,d0max);
  histoz0_truth_muon = new TH1F("histoz0_truth_muon",";z_{0} (mm);N Tracks",nbins,z0min,z0max);
  histophi_truth_muon = new TH1F("histophi_truth_muon",";#phi;N Tracks",nbins,phimin,phimax);
  histocurv_truth_muon = new TH1F("histocurv_truth_muon",";1/(2p_{T}) (GeV^{-1});N Tracks",nbins,-abscurvmax,abscurvmax);
  histoeta_truth_muon = new TH1F("histoeta_truth_muon",";#eta;N Tracks",nbins,etamin,etamax);
  histontracks_truth_muon = new TH1F("histontracks_truth_muon",";N Tracks;N Events",nbins,0,maxntracks);
  histopt_truth_muon = new TH1F("histopt_truth_muon",";p_{T} (GeV);N Tracks",nptbins,ptbins);

  histod0_truthM_muon = new TH1F("histod0_truthM_muon",";d_{0} (mm);N Tracks",nbins,d0min,d0max);
  histoz0_truthM_muon = new TH1F("histoz0_truthM_muon",";z_{0} (mm);N Tracks",nbins,z0min,z0max);
  histophi_truthM_muon = new TH1F("histophi_truthM_muon",";#phi;N Tracks",nbins,phimin,phimax);
  histocurv_truthM_muon = new TH1F("histocurv_truthM_muon",";1/(2p_{T}) (GeV^{-1});N Tracks",nbins,-abscurvmax,abscurvmax);
  histoeta_truthM_muon = new TH1F("histoeta_truthM_muon",";#eta;N Tracks",nbins,etamin,etamax);
  histontracks_truthM_muon = new TH1F("histontracks_truthM_muon",";N Tracks;N Events",nbins,0,maxntracks);
  histopt_truthM_muon = new TH1F("histopt_truthM_muon",";p_{T} (GeV);N Tracks",nptbins,ptbins);

  ientry2=0;
}


void Process(Long64_t ientry) {

  t_ftkdata->GetEntry(ientry);

  // collect information from the run-evt from the truth
  Int_t evtnumber_ftk = tracks->eventNumber();
  Int_t runnumber_ftk = tracks->runNumber();

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
    cerr << Form("Mismatch between entries: (%d,%d)",runnumber_ftk,evtnumber_ftk) << endl;
    ientry2=0;
    return;
  }

  t_truth->GetEntry(ientry2);

  if (ientry%1000==0) {
    Info("Process","Event %lld, (Run,Evt) = (%d,%d)",ientry, runnumber_ftk, evtnumber_ftk);
  }

  // Study the list of the FTK tracks to collect elements for the tracks efficiency
  Int_t ntracks = tracks->getNTracks();
  if (Use1stStage)
    ntracks = tracks->getNTracksI();
  histontracks_ftk->Fill(ntracks);
  FTKBarcodeMM ftkmatchinfo;
  const FTKTrack *curtrack;
  for (Int_t itrk=0;itrk!=ntracks;++itrk) { // loop over the FTK tracks
    curtrack = (Use1stStage ? tracks->getTrackI(itrk) : tracks->getTrack(itrk));
    histod0_ftk->Fill(curtrack->getIP());
    histoz0_ftk->Fill(curtrack->getZ0());
    histocurv_ftk->Fill(curtrack->getHalfInvPt()*1e3);
    histopt_ftk->Fill(TMath::Abs(curtrack->getPt())*1e-3);
    double eta = curtrack->getEta();
    histoeta_ftk->Fill(eta);
    histophi_ftk->Fill(curtrack->getPhi());
    histophid0_ftk->Fill(curtrack->getPhi(),curtrack->getIP());
    // histophid0_ftk->Fill(curtrack->getPhi(),curtrack->getD0());
    histochisqndfeta_ftk->Fill(curtrack->getChi2ndof(),eta);
    histonmisseta_ftk->Fill(curtrack->getNMissing(),eta);
    for (Int_t icoord=0;icoord!=curtrack->getNCoords();++icoord) {
      if (curtrack->getBitmask()&(1<<icoord)) {
        histocoordmasketa_ftk->Fill(icoord,eta);
      }
      if (curtrack->getBitmask()&(1<<icoord)) {
        histocoordmaskphi_ftk->Fill(icoord,curtrack->getPhi());
      }
      if (icoord == 0 && curtrack->getBitmask()&(1<<icoord)) {
        histoetaz0_ftk->Fill(eta,curtrack->getZ0());
      }
    }

    if (curtrack->getEventIndex()==0) {
      // Information on FTK tracks relative to the hard-scattering events
      // are collected in a vector and later used to build matching maps
      if (curtrack->getBarcodeFrac()>.5) {
        ftkmatchinfo.insert(pair<MatchInfo, const FTKTrack*>(
          MatchInfo(curtrack->getBarcode(),curtrack->getEventIndex()),curtrack));
      }
    }

    // apply some basic quality selection
    //double d0 = curtrack->getD0();
    double d0 = curtrack->getIP();
    if (d0<d0min || d0>d0max) continue;

    double z0 = curtrack->getZ0();
    if (z0<z0min || z0>z0max) continue;

    double phi = curtrack->getPhi();
    if (phi<phimin || phi > phimax) continue;

    double curv = curtrack->getHalfInvPt()*.5e3;
    if (curv<-abscurvmax || curv>abscurvmax) continue;

    if (eta<etamin || eta>etamax) continue;

    histod0_goodftk->Fill(d0);
    histoz0_goodftk->Fill(z0);
    histophi_goodftk->Fill(curtrack->getPhi());
    histocurv_goodftk->Fill(curv);
    histoeta_goodftk->Fill(eta);
    histopt_goodftk->Fill(curtrack->getPt()*1e-3);

    if (curtrack->getBarcodeFrac()<.5) {
      histod0_goodftkU->Fill(d0);
      histoz0_goodftkU->Fill(z0);
      histophi_goodftkU->Fill(curtrack->getPhi());
      histocurv_goodftkU->Fill(curv);
      histoeta_goodftkU->Fill(eta);
      histopt_goodftkU->Fill(curtrack->getPt()*1e-3);
    }
  } // end loop over the FTK tracks

  /*
   * Study the list of the truth tracks to collect elements useful for the tracks
   * efficiency
   */
  Int_t ntruth = truthTracks->size();
  Int_t ntruth_good(0);
  Int_t ntruth_good_muon(0);
  histontracks_truth->Fill(ntruth);
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
    if (d0<d0min || d0>d0max) continue;

    double z0 = curtruth.getZ();
    if (z0<z0min || z0>z0max) continue;

    double curv = .5*curtruth.getQ()*invpt;
    if (curv<-abscurvmax || curv>abscurvmax) continue;

    double phi = curtruth.getPhi();
    if (phi<phimin || phi>phimax) continue;

    double eta = curtruth.getEta();
    if (eta<etamin || eta>etamax) continue;

    if (curtruth.getEventIndex()!=0 && curtruth.getQ()==0) continue;

    ntruth_good += 1;

    // Fill the histogram for the generic truth distribution
    histod0_truth->Fill(d0);
    histoz0_truth->Fill(z0);
    histocurv_truth->Fill(curv);
    histophi_truth->Fill(curtruth.getPhi());
    histoetaphi_truth->Fill(eta, curtruth.getPhi());
    histoetaz0_truth->Fill(eta, z0);
    histoeta_truth->Fill(eta);
    histopt_truth->Fill(pt);

    int pdgcode = curtruth.getPDGCode();
    if (pdgcode==13 || pdgcode==-13) {
      // muon block
      ntruth_good_muon += 1;

      histod0_truth_muon->Fill(d0);
      histoz0_truth_muon->Fill(z0);
      histocurv_truth_muon->Fill(curv);
      histophi_truth_muon->Fill(curtruth.getPhi());
      histoeta_truth_muon->Fill(eta);
      histopt_truth_muon->Fill(pt);
    }

    // match the barcode and event index values
    MatchInfo reftruth(barcode,curtruth.getEventIndex());
    pair<FTKBarcodeMM::const_iterator,FTKBarcodeMM::const_iterator> mrange = ftkmatchinfo.equal_range(reftruth);
    if (mrange.first != mrange.second) {
      histod0_truthM->Fill(d0);
      histoz0_truthM->Fill(z0);
      histocurv_truthM->Fill(curv);
      histophi_truthM->Fill(curtruth.getPhi());
      histoetaphi_truthM->Fill(eta, curtruth.getPhi());
      histoetaz0_truthM->Fill(eta, z0);
      histoeta_truthM->Fill(eta);
      histopt_truthM->Fill(pt);

      if (pdgcode==13 || pdgcode==-13) {
        // matched muon block
        histod0_truthM_muon->Fill(d0);
        histoz0_truthM_muon->Fill(z0);
        histocurv_truthM_muon->Fill(curv);
        histophi_truthM_muon->Fill(curtruth.getPhi());
        histoeta_truthM_muon->Fill(eta);
        histopt_truthM_muon->Fill(pt);
      }

      const FTKTrack *bestftk(0x0);
      for(FTKBarcodeMM::const_iterator ftkI = mrange.first;ftkI!=mrange.second;++ftkI) {
        if (!bestftk) {
          bestftk = (*ftkI).second;
        } else if (bestftk->getBarcodeFrac()<(*ftkI).second->getBarcodeFrac()) {
          bestftk = (*ftkI).second;
        }
      }

      histod0res->Fill(d0-bestftk->getIP());
      histoz0res->Fill(z0-bestftk->getZ0());
      histocurvres->Fill(curv-bestftk->getHalfInvPt()*1e3);
      histophires->Fill(curtruth.getPhi()-bestftk->getPhi());
      histoetares->Fill(eta-bestftk->getEta());
      historelptres->Fill((pt-TMath::Abs(bestftk->getPt())*1e-3)*invpt);
      historelptrespt->Fill(pt,(pt-TMath::Abs(bestftk->getPt())*1e-3)*invpt);
    }
  } // end loop over truth tracks

  // fill counter histograms
  histontracks_truthM->Fill(ntruth_good);
  histontracks_truthM_muon->Fill(ntruth_good_muon);
}


void Terminate(std::string& outputname) {

  Info("Terminate","Adding the histograms to the file: %s", outputname.c_str());
  TFile *ofile = TFile::Open(outputname.c_str(), "recreate");

  for (int ibin=1;ibin!=(int)(nbins+1);++ibin) {
    Int_t binscale = histophi_ftk->GetNbinsX()/histocoordmaskphi_ftk->GetNbinsY();
    Int_t binmin = (ibin-1)*binscale+1;
    Double_t colnorm = 1./histophi_ftk->Integral(binmin,binmin+binscale-1);
    for (int jbin=1;jbin!=histocoordmaskphi_ftk->GetNbinsX()+1;++jbin) {
      histocoordmaskphi_ftk->SetBinContent(jbin,ibin,histocoordmaskphi_ftk->GetBinContent(jbin,ibin)*colnorm);
    }
  }

  for (int ibin=1;ibin!=(int)(nbins+1);++ibin) {
    Int_t binscale = histoeta_ftk->GetNbinsX()/histocoordmasketa_ftk->GetNbinsY();
    Int_t binmin = (ibin-1)*binscale+1;
    Double_t colnorm = 1./histoeta_ftk->Integral(binmin,binmin+binscale-1);

    for (int jbin=1;jbin!=histocoordmasketa_ftk->GetNbinsX()+1;++jbin) {
      histocoordmasketa_ftk->SetBinContent(jbin,ibin,histocoordmasketa_ftk->GetBinContent(jbin,ibin)*colnorm);
    }
    for (int jbin=1;jbin!=histonmisseta_ftk->GetNbinsX()+1;++jbin) {
      histonmisseta_ftk->SetBinContent(jbin,ibin,histonmisseta_ftk->GetBinContent(jbin,ibin)*colnorm);
    }
    for (int jbin=1;jbin!=histochisqndfeta_ftk->GetNbinsX()+1;++jbin) {
      histochisqndfeta_ftk->SetBinContent(jbin,ibin,histochisqndfeta_ftk->GetBinContent(jbin,ibin)*colnorm);
    }
  }

  // calculate some efficiency plots
  TGraphAsymmErrors *greffd0 = new TGraphAsymmErrors(histod0_truthM,histod0_truth);
  greffd0->SetNameTitle("greffd0",";d_{0} (mm);Efficiency w.r.t. Truth");
  ofile->Add(greffd0);
  TGraphAsymmErrors *greffz0 = new TGraphAsymmErrors(histoz0_truthM,histoz0_truth);
  greffz0->SetNameTitle("greffz0",";z_{0} (mm);Efficiency w.r.t. Truth");
  ofile->Add(greffz0);
  TGraphAsymmErrors *greffcurv = new TGraphAsymmErrors(histocurv_truthM,histocurv_truth);
  greffcurv->SetNameTitle("greffcurv",";1/2p_{T} (GeV^{-1});Efficiency w.r.t. Truth");
  ofile->Add(greffcurv);
  TGraphAsymmErrors *greffeta = new TGraphAsymmErrors(histoeta_truthM,histoeta_truth);
  greffeta->SetNameTitle("greffeta",";#eta;Efficiency w.r.t. Truth");
  ofile->Add(greffeta);
  TGraphAsymmErrors *greffphi = new TGraphAsymmErrors(histophi_truthM,histophi_truth);
  greffphi->SetNameTitle("greffphi",";#phi (rad);Efficiency w.r.t. Truth");
  ofile->Add(greffphi);
  TGraphAsymmErrors *greffpt = new TGraphAsymmErrors(histopt_truthM,histopt_truth);
  greffpt->SetNameTitle("greffpt",";p_{T} (GeV);Efficiency w.r.t. Truth");
  ofile->Add(greffpt);

  // calculate some efficiency plots for muons
  TGraphAsymmErrors *greffd0_muon = new TGraphAsymmErrors(histod0_truthM_muon,histod0_truth_muon);
  greffd0_muon->SetNameTitle("greffd0_muon",";d_{0} (mm);Efficiency w.r.t. Truth");
  ofile->Add(greffd0_muon);
  TGraphAsymmErrors *greffz0_muon = new TGraphAsymmErrors(histoz0_truthM_muon,histoz0_truth_muon);
  greffz0_muon->SetNameTitle("greffz0_muon",";z_{0} (mm);Efficiency w.r.t. Truth");
  ofile->Add(greffz0_muon);
  TGraphAsymmErrors *greffcurv_muon = new TGraphAsymmErrors(histocurv_truthM_muon,histocurv_truth_muon);
  greffcurv_muon->SetNameTitle("greffcurv_muon",";1/2p_{T} (GeV^{-1});Efficiency w.r.t. Truth");
  ofile->Add(greffcurv_muon);
  TGraphAsymmErrors *greffeta_muon = new TGraphAsymmErrors(histoeta_truthM_muon,histoeta_truth_muon);
  greffeta_muon->SetNameTitle("greffeta_muon",";#eta;Efficiency w.r.t. Truth");
  ofile->Add(greffeta_muon);
  TGraphAsymmErrors *greffphi_muon = new TGraphAsymmErrors(histophi_truthM_muon,histophi_truth_muon);
  greffphi_muon->SetNameTitle("greffphi_muon",";#phi (rad);Efficiency w.r.t. Truth");
  ofile->Add(greffphi_muon);
  TGraphAsymmErrors *greffpt_muon = new TGraphAsymmErrors(histopt_truthM_muon,histopt_truth_muon);
  greffpt_muon->SetNameTitle("greffpt_muon",";p_{T} (GeV);Efficiency w.r.t. Truth");
  ofile->Add(greffpt_muon);

  TGraphAsymmErrors *grfaked0 = new TGraphAsymmErrors(histod0_goodftkU,histod0_goodftk);
  grfaked0->SetNameTitle("grfaked0",";d_{0} (mm);Efficiency w.r.t. Truth");
  ofile->Add(grfaked0);
  TGraphAsymmErrors *grfakez0 = new TGraphAsymmErrors(histoz0_goodftkU,histoz0_goodftk);
  grfakez0->SetNameTitle("grfakez0",";z_{0} (mm);Efficiency w.r.t. Truth");
  ofile->Add(grfakez0);
  TGraphAsymmErrors *grfakecurv = new TGraphAsymmErrors(histocurv_goodftkU,histocurv_goodftk);
  grfakecurv->SetNameTitle("grfakecurv",";1/2p_{T} (GeV^{-1});Efficiency w.r.t. Truth");
  ofile->Add(grfakecurv);
  TGraphAsymmErrors *grfakeeta = new TGraphAsymmErrors(histoeta_goodftkU,histoeta_goodftk);
  grfakeeta->SetNameTitle("grfakeeta",";#eta;Efficiency w.r.t. Truth");
  ofile->Add(grfakeeta);
  TGraphAsymmErrors *grfakephi = new TGraphAsymmErrors(histophi_goodftkU,histophi_goodftk);
  grfakephi->SetNameTitle("grfakephi",";#phi (rad);Efficiency w.r.t. Truth");
  ofile->Add(grfakephi);
  TGraphAsymmErrors *grfakept = new TGraphAsymmErrors(histopt_goodftkU,histopt_goodftk);
  grfakept->SetNameTitle("grfakept",";p_{T} (GeV);Efficiency w.r.t. Truth");
  ofile->Add(grfakept);

  ofile->Add(histocoordmasketa_ftk);
  ofile->Add(histocoordmaskphi_ftk);
  ofile->Add(histonmisseta_ftk);
  ofile->Add(histochisqndfeta_ftk);
  ofile->Add(histontracks_ftk);
  ofile->Add(histod0_ftk);
  ofile->Add(histoz0_ftk);
  ofile->Add(histophi_ftk);
  ofile->Add(histophid0_ftk);
  ofile->Add(histocurv_ftk);
  ofile->Add(histoeta_ftk);
  ofile->Add(histoetaphi_ftk);
  ofile->Add(histoetaphi_truth);
  ofile->Add(histoetaphi_truthM);
  ofile->Add(histoetaz0_ftk);
  ofile->Add(histoetaz0_truth);
  ofile->Add(histoetaz0_truthM);
  ofile->Add(histopt_ftk);
  ofile->Add(histontracks_goodftk);
  ofile->Add(histod0_goodftk);
  ofile->Add(histoz0_goodftk);
  ofile->Add(histocurv_goodftk);
  ofile->Add(histoeta_goodftk);
  ofile->Add(histophi_goodftk);
  ofile->Add(histopt_goodftk);
  ofile->Add(histontracks_goodftkU);
  ofile->Add(histod0_goodftkU);
  ofile->Add(histoz0_goodftkU);
  ofile->Add(histocurv_goodftkU);
  ofile->Add(histoeta_goodftkU);
  ofile->Add(histophi_goodftkU);
  ofile->Add(histopt_goodftkU);
  ofile->Add(histontracks_truth);
  ofile->Add(histod0_truth);
  ofile->Add(histoz0_truth);
  ofile->Add(histocurv_truth);
  ofile->Add(histoeta_truth);
  ofile->Add(histophi_truth);
  ofile->Add(histopt_truth);
  ofile->Add(histontracks_truthM);
  ofile->Add(histod0_truthM);
  ofile->Add(histoz0_truthM);
  ofile->Add(histocurv_truthM);
  ofile->Add(histoeta_truthM);
  ofile->Add(histophi_truthM);
  ofile->Add(histopt_truthM);
  ofile->Add(histod0res);
  ofile->Add(histoz0res);
  ofile->Add(histocurvres);
  ofile->Add(histoetares);
  ofile->Add(histophires);
  ofile->Add(historelptres);
  ofile->Add(historelptrespt);
  ofile->Add(histontracks_truth_muon);
  ofile->Add(histod0_truth_muon);
  ofile->Add(histoz0_truth_muon);
  ofile->Add(histocurv_truth_muon);
  ofile->Add(histoeta_truth_muon);
  ofile->Add(histophi_truth_muon);
  ofile->Add(histopt_truth_muon);
  ofile->Add(histontracks_truthM_muon);
  ofile->Add(histod0_truthM_muon);
  ofile->Add(histoz0_truthM_muon);
  ofile->Add(histocurv_truthM_muon);
  ofile->Add(histoeta_truthM_muon);
  ofile->Add(histophi_truthM_muon);
  ofile->Add(histopt_truthM_muon);
  ofile->ls();
  ofile->Write();
  ofile->Close();
}


int main(int argc, char **argv) {
  int events;
  std::string output;
  std::vector<std::string> files;

  try {
    std::string appName = boost::filesystem::basename(argv[0]);
    // Define and parse the program options
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print this help message")
      ("output,o", po::value<std::string>(&output)->default_value("FTKCheckEff.root"), "The name out of the output file")
      ("events,e", po::value<int>(&events)->default_value(-1), "The number of events to run over. Set to -1 to use all events")
      ("tower,t", po::value<int>(&towerNumber)->default_value(-1), "the number of the tower in ftkdata output data. Use -1 if you have fully merged output")
      ("use-first-stage", po::bool_switch(&Use1stStage)->default_value(false), "Use events 1st stage tracks instead of 2nd stage tracks")
      ("files", po::value<std::vector<std::string> >(&files), "FTK NTUP files");
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
