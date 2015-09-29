/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecValidationNtuples/MuonInsideOutAnalysisPlots.h"
#include "MuonRecValidationNtuples/MuonBetaCalculationUtils.h"

#include "TH1.h"
#include "TH2.h"
#include "TDirectory.h"
#include <string>
#include "MuonIdHelpers/MuonStationIndex.h"

namespace Muon {

  float mstau = 431000.;

  void ResPlots::book( TString prefix ) {
    res = new TH1F(prefix+"res","res",100,-100.,100.);
    pull = new TH1F(prefix+"pull","pull",100,-10.,10.);
    exerr = new TH1F(prefix+"exerr","exerr",100,0,100.);
  }
  void ResPlots::fill( float res_, float pull_, float exerr_ ) {
    res->Fill(res_);
    pull->Fill(pull_);
    exerr->Fill(exerr_);
  }
    
  void ResPlotsPt::book( TString prefix ) {
    res_p = new TH2F(prefix+"res_p","res_p",100,-100.,100.,100,0.,300.);
    pull_p = new TH2F(prefix+"pull_p","pull_p",100,-10.,10.,100,0.,300.);
    exerr_p  = new TH2F(prefix+"exerr_p","exerr_p",100,0.,100.,100,0.,300.);
  }
  void ResPlotsPt::fill( float res_, float pull_, float exerr_, float p_ ) {
    res_p->Fill(res_,p_);
    pull_p->Fill(pull_,p_);
    exerr_p->Fill(exerr_,p_);
  }
  
  void ChamberResPlots::book( TDirectory* dir, TString prefix ) {
    TDirectory* ndir = dir->mkdir(prefix+"Chamber");
    ndir->cd();
    plots.resize(MuonStationIndex::ChIndexMax);
    plots_p.resize(MuonStationIndex::ChIndexMax);
    for( unsigned int i=0;i<plots.size();++i ){
      TString name = prefix + "chIndex_" + std::to_string(i).c_str() + "_";
      plots[i].book(name);
      plots_p[i].book(name);
    }
    dir->cd();
  }

  void ChamberResPlots::fill( int chIndex_, float res_, float pull_, float exerr_, float p_ ) {
    if( chIndex_ < 0 || chIndex_ >= MuonStationIndex::ChIndexMax ) return;

    plots[chIndex_].fill(res_,pull_,exerr_);
    plots_p[chIndex_].fill(res_,pull_,exerr_,p_);

  }


  void HitPlots::book( TDirectory* dir, TString prefix ) {
    all.book(prefix);
    all_p.book(prefix);
    chamber.book(dir,prefix);
  }

  void HitPlots::fill( const MuonValidationHitBlock& hits, int index, float p_ ) {
    fill( (*hits.id.chIndex)[index], (*hits.residuals.residual)[index], 
          (*hits.residuals.pull)[index], (*hits.residuals.expos_err)[index], p_);
  }


  void HitPlots::fill( int chIndex_, float res_, float pull_, float exerr_, float p_ ) {
    all.fill(res_,pull_,exerr_);
    all_p.fill(res_,pull_,exerr_,p_);
    chamber.fill(chIndex_,res_,pull_,exerr_,p_);
  }

  void TimePlots::book( TDirectory*, TString prefix ) {
    time = new TH1F(prefix+"time","time",100,-20.,100.);
    error = new TH1F(prefix+"error","error",100,-20.,100.);
    timeRes = new TH1F(prefix+"timeRes","timeRes",200,-15,15);
    timePull = new TH1F(prefix+"pullRes","pullRes",200,-7,7);
    beta = new TH1F(prefix+"beta","beta",100,.0,2.);
    betaRes = new TH1F(prefix+"betaRes","betaRes",200,-1.2,1.2);
  }

  void TimePlots::fill( const MuonValidationTimeBlock& times, int index, float p_, int pdg ) {
    float dist = (*times.d)[index];
    float m2 = std::abs(pdg) == 1000015 ? mstau : 0.;
    MuonBetaCalculationUtils betaUtils(m2);
    float tof       = betaUtils.calculateTof(1.,dist);
    float betaTrack = betaUtils.calculateBetaTrack(1000.*p_);
    float timeTrack = betaUtils.calculateTof(betaTrack,dist);
    float t0_       = (*times.time)[index]+tof;
    float betaSeg   = betaUtils.calculateBeta(t0_,dist);

    time->Fill((*times.time)[index]);
    error->Fill((*times.err)[index]);
    beta->Fill(betaSeg);
    timeRes->Fill(t0_-timeTrack);
    timePull->Fill( (t0_-timeTrack)/(*times.err)[index] );
    betaRes->Fill(betaSeg-betaTrack);
  }

  void HoughPlots::book( TDirectory* dir, TString prefix ) {

    max = new TH1F(prefix+"max","max",100,0.,30.);
    max_pull = new TH2F(prefix+"max_pull","max_pull",100,0.,30.,100,-10.,10.);
    all.book(prefix);
    chamber.book(dir,prefix);
  }

  void HoughPlots::fill( const MuonValidationHoughBlock& hough, int index, float p_ ) {
    fill( (*hough.id.chIndex)[index], (*hough.residuals.residual)[index], 
          (*hough.residuals.pull)[index], (*hough.residuals.expos_err)[index], p_, (*hough.maximum)[index]);
  }

  void HoughPlots::fill( int chIndex_, float res_, float pull_, float exerr_, float p_, float max_ ){
    max->Fill(max_);
    max_pull->Fill(max_,pull_);
    all.fill(res_,pull_,exerr_);
    chamber.fill(chIndex_,res_,pull_,exerr_,p_);
  }

  void SegmentPlots::book( TDirectory* dir, TString prefix ){
    t0 = new TH1F(prefix+"t0","t0",100,-20.,50.);
    t0Trig = new TH1F(prefix+"t0Trig","t0Trig",100,-20.,50.);
    beta = new TH1F(prefix+"beta","beta",100,.0,2.);
    beta2 = new TH2F(prefix+"betaTrack","betaTrack",100,.0,2.,100,.0,2.);
    betaTrig = new TH1F(prefix+"betaTrig","betaTrig",100,.0,2.);
    t0Res = new TH1F(prefix+"t0Res","t0Res",200,-15,15);
    t0ResTrig = new TH1F(prefix+"t0TrigRes","t0TrigRes",200,-15,15);
    betaRes = new TH1F(prefix+"betaRes","betaRes",200,-1.2,1.2);
    betaResTrig = new TH1F(prefix+"betaTrigRes","betaTrigRes",200,-1.2,1.2);
    quality = new TH1F(prefix+"quality","quality",6,-0.5,5.5);
    quality_chIndex = new TH2F(prefix+"quality_chIndex","quality_chIndex",6,-0.5,5.5,MuonStationIndex::ChIndexMax,-0.5,-0.5+MuonStationIndex::ChIndexMax);
    quality_sector = new TH2F(prefix+"quality_sector","quality_sector",6,-0.5,5.5,16,0.5,16.5);

    allx.book(prefix+"x_");
    chamberx.book(dir,prefix+"x_");

    ally.book(prefix+"y_");
    chambery.book(dir,prefix+"y_");

    allxz.book(prefix+"xz_");
    chamberxz.book(dir,prefix+"xz_");

    allyz.book(prefix+"yz_");
    chamberyz.book(dir,prefix+"yz_");

    allcy.book(prefix+"cy_");
    chambercy.book(dir,prefix+"cy_");
  }


  void SegmentPlots::fill( const MuonValidationSegmentBlock& segments, int index, float p_ ) {

    allx.fill( (*segments.xresiduals.residual)[index], 
               (*segments.xresiduals.pull)[index], (*segments.xresiduals.expos_err)[index]);
    chamberx.fill( (*segments.id.chIndex)[index], (*segments.xresiduals.residual)[index], 
                   (*segments.xresiduals.pull)[index], (*segments.xresiduals.expos_err)[index], p_);

    ally.fill( (*segments.yresiduals.residual)[index], 
               (*segments.yresiduals.pull)[index], (*segments.yresiduals.expos_err)[index]);
    chambery.fill( (*segments.id.chIndex)[index], (*segments.yresiduals.residual)[index], 
                   (*segments.yresiduals.pull)[index], (*segments.yresiduals.expos_err)[index], p_);

    allxz.fill( (*segments.angleXZ.residual)[index], 
               (*segments.angleXZ.pull)[index], (*segments.angleXZ.expos_err)[index]);
    chamberxz.fill( (*segments.id.chIndex)[index], (*segments.angleXZ.residual)[index], 
                   (*segments.angleXZ.pull)[index], (*segments.angleXZ.expos_err)[index], p_);

    allyz.fill( (*segments.angleYZ.residual)[index], 
               (*segments.angleYZ.pull)[index], (*segments.angleYZ.expos_err)[index]);
    chamberyz.fill( (*segments.id.chIndex)[index], (*segments.angleYZ.residual)[index], 
                   (*segments.angleYZ.pull)[index], (*segments.angleYZ.expos_err)[index], p_);

    allcy.fill( (*segments.combinedYZ.residual)[index], 
               (*segments.combinedYZ.pull)[index], (*segments.combinedYZ.expos_err)[index]);
    chambercy.fill( (*segments.id.chIndex)[index], (*segments.combinedYZ.residual)[index], 
                   (*segments.combinedYZ.pull)[index], (*segments.combinedYZ.expos_err)[index], p_);
    
    fill( (*segments.id.chIndex)[index], (*segments.id.sector)[index], (*segments.quality)[index] );

    float dist = sqrt( (*segments.r)[index]*(*segments.r)[index] + (*segments.z)[index]*(*segments.z)[index] );
    float m2 = std::abs((*segments.truth.pdg)[index]) == 1000015 ? mstau : 0.;
    MuonBetaCalculationUtils betaUtils(m2);
    float betaTrack = betaUtils.calculateBetaTrack(1000.*p_);
    float tof       = betaUtils.calculateTof(1.,dist);
    float t0_       = (*segments.t0)[index]+tof;
    float t0Trig_   = (*segments.t0Trig)[index]+tof;
    float t0Track   = betaUtils.calculateTof(betaTrack,dist);
    float betaSeg   = betaUtils.calculateBeta(t0_,dist);
    float betaSegTrig = betaUtils.calculateBeta(t0Trig_,dist);
    std::cout << " pdg " << (*segments.truth.pdg)[index] << " index " << index << " beta " << betaTrack << " segBeta " << betaSeg 
              << " t0Seg " << (*segments.t0)[index] << " tof " << tof << " t0 " << t0_ << " track " << t0Track << std::endl;
    t0->Fill((*segments.t0)[index]);
    t0Trig->Fill((*segments.t0Trig)[index]);
    beta->Fill(betaSeg);
    beta2->Fill(betaSeg,betaTrack);
    betaTrig->Fill(betaSegTrig);
    t0Res->Fill(t0_-t0Track);
    t0ResTrig->Fill(t0Trig_-t0Track);
    betaRes->Fill(betaSeg-betaTrack);
    betaResTrig->Fill(betaSegTrig-betaTrack);
  }

  void SegmentPlots::fill( int chIndex_, int sector_, int quality_ ){
    quality->Fill(quality_);
    quality_chIndex->Fill(quality_,chIndex_);
    quality_sector->Fill(quality_,sector_);
  }

  void BetaFitPlots::book( TDirectory* , TString prefix ) {
    beta = new TH1F(prefix+"beta","beta",100,0,1.5);
    chi2Ndof = new TH1F(prefix+"chi2Ndof","chi2Ndof",100,0,50.);
    ndof = new TH1F(prefix+"ndof","ndof",51,-0.5,50.5);
    res = new TH1F(prefix+"res","res",100,-1.,1.);
  }

  void BetaFitPlots::fill( float beta_, float betaTruth_, float chi2_, int ndof_  ) {
    beta->Fill(beta_);
    if( ndof_ > 0 ) chi2Ndof->Fill(chi2_/ndof_);
    ndof->Fill(ndof_);
    res->Fill(beta_-betaTruth_);
  }

  void TrackPlots::book( TDirectory* dir, TString prefix ) {
    pt = new TH1F(prefix+"pt","pt",100,0,300.);
    eta = new TH1F(prefix+"eta","eta",100,-3,3);
    phi = new TH1F(prefix+"phi","phi",100,-3,3);
    beta = new TH1F(prefix+"beta","beta",100,0,1);
    pdg = new TH1F(prefix+"pdg","pdg",31,-15.5,15.5);
    ntruth = new TH1F(prefix+"ntruth","ntruth",10,-0.5,9.5);
    nseg = new TH1F(prefix+"nseg","nseg",10,-0.5,9.5);
    nseg1 = new TH1F(prefix+"nseg1","nseg",10,-0.5,9.5);
    nhough = new TH1F(prefix+"nhough","nhough",10,-0.5,9.5);
    ntruth_seg = new TH2F(prefix+"ntruth_seg","ntruth_seg",10,-0.5,9.5,10,-0.5,9.5);
    ntruth_seg1 = new TH2F(prefix+"ntruth_seg1","ntruth_seg",10,-0.5,9.5,10,-0.5,9.5);
    ntruth_hough = new TH2F(prefix+"ntruth_hough","ntruth_hough",10,-0.5,9.5,10,-0.5,9.5);

    hits.book(dir,prefix+"h_");
    hough.book(dir,prefix+"ho_");
    segments.book(dir,prefix+"s0_");
    segments1.book(dir,prefix+"s1_");
    rpcTimes.book(dir,prefix+"trpc_");
    segmentTimes.book(dir,prefix+"tseg_");
    betaAll.book(dir,prefix+"betaAll_");
    betaType0.book(dir,prefix+"betaType0_");
    betaType1.book(dir,prefix+"betaType1_");

  }

  void TrackPlots::fill( const MuonValidationTrackParticleBlock& tracks, int index ) {
    pt->Fill( (*tracks.pt)[index]*0.001 );
    eta->Fill( (*tracks.eta)[index] );
    phi->Fill( (*tracks.phi)[index] );
    float m2 = std::abs((*tracks.truth.pdg)[index]) == 1000015 ? mstau : 0.;
    MuonBetaCalculationUtils betaUtils(m2);
    float betaTruth_ = betaUtils.calculateBetaTrack((*tracks.p)[index]);
    beta->Fill( betaTruth_ );
  }

  void TrackPlots::fill( int ntruth_, int nseg_, int nseg1_, int nhough_ ) {
    ntruth->Fill(ntruth_);
    nseg->Fill(nseg_);
    nseg1->Fill(nseg1_);
    nhough->Fill(nhough_);
    ntruth_seg->Fill(ntruth_,nseg_);
    ntruth_seg1->Fill(ntruth_,nseg1_);
    ntruth_hough->Fill(ntruth_,nhough_);
  }

  
  void MuonInsideOutAnalysisPlots::book( TDirectory* dir, TString prefix ) {
    TDirectory* ndir = dir->mkdir("Muon");
    ndir->cd();
    muon.book(ndir,prefix);
    ndir = dir->mkdir("Rest");
    ndir->cd();
    rest.book(ndir,prefix);
    dir->cd();
  }
}
