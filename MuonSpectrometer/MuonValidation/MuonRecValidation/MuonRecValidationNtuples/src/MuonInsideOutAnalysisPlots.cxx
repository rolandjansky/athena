/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecValidationNtuples/MuonInsideOutAnalysisPlots.h"
#include "MuonRecValidationNtuples/MuonBetaCalculationUtils.h"

#include "TH1.h"
#include "TH2.h"
#include "TDirectory.h"
#include <string>
#include "MuonStationIndex/MuonStationIndex.h"

namespace Muon {

  float mstau = 431000.;

  void ResPlots::book( const TString& prefix ) {
    res = new TH1F(prefix+"res","res",100,-100.,100.);
    pull = new TH1F(prefix+"pull","pull",100,-10.,10.);
    exerr = new TH1F(prefix+"exerr","exerr",100,0,100.);
  }
  void ResPlots::fill( float res_, float pull_, float exerr_ ) {
    res->Fill(res_);
    pull->Fill(pull_);
    exerr->Fill(exerr_);
  }
    
  void ResPlotsPt::book( const TString& prefix ) {
    res_p = new TH2F(prefix+"res_p","res_p",100,-100.,100.,100,0.,300.);
    pull_p = new TH2F(prefix+"pull_p","pull_p",100,-10.,10.,100,0.,300.);
    exerr_p  = new TH2F(prefix+"exerr_p","exerr_p",100,0.,100.,100,0.,300.);
  }
  void ResPlotsPt::fill( float res_, float pull_, float exerr_, float p_ ) {
    res_p->Fill(res_,p_);
    pull_p->Fill(pull_,p_);
    exerr_p->Fill(exerr_,p_);
  }
  
  void ChamberResPlots::book( TDirectory* dir, const TString& prefix ) {
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


  void HitPlots::book( TDirectory* dir, const TString& prefix ) {
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

  void TimePlots::book( TDirectory*, const TString& prefix, int type ) {
    type = type % 1000; // truncate anything above 1000
    bool isMDT = (type == 1 || type == 10);
    float tmin = -15;
    float tmax =  15;
    float timeRange = 5;
    if( isMDT ){
      tmin = -30;
      tmax =  30;
      if( type == 10 ) timeRange = 800;
    }
    time = new TH1F(prefix+"time","time",100,-20.,100.);
    error = new TH1F(prefix+"error","error",100,0.,15.);
    timeRes = new TH1F(prefix+"timeRes","timeRes",200,tmin,tmax);
    timePull = new TH1F(prefix+"pullRes","pullRes",200,-7,7);
    beta = new TH1F(prefix+"beta","beta",100,.0,2.);
    betaRes = new TH1F(prefix+"betaRes","betaRes",200,-1.2,1.2);
    betaPull = new TH1F(prefix+"betaPull","betaPull",200,-7,7);
    invBetaRes = new TH1F(prefix+"invBetaRes","invBetaRes",200,-1.2,1.2);
    invBetaPull = new TH1F(prefix+"invBetaPull","invBetaPull",200,-7,7);


    if( type == 10 ){
      timeResScat = new TH2F(prefix+"timeResScat","timeResScat",100,tmin,timeRange,100,tmin,tmax);
      timePullScat = new TH2F(prefix+"pullResScat","pullResScat",100,tmin,timeRange,100,-7.,7.);

      error_time = new TH2F(prefix+"error_time","error_time",100,tmin,timeRange,100,0.,15.);
      errorRT = new TH1F(prefix+"errorRT","errorRT",100,-20.,100.);
      errorTrack = new TH1F(prefix+"errorTrack","errorTrack",100,-20.,100.);
      errorRT_Track = new TH2F(prefix+"errorRT_Track","errorRT_Track",100,-20.,100.,100,-20.,100.);
      timeResCor = new TH1F(prefix+"timeResCor","timeResCor",200,tmin,tmax);
      timePullCor  = new TH1F(prefix+"timePullCor","timePullCor",200,-7,7);
      timeRes0_10 = new TH1F(prefix+"timeRes0_10","timeRes0_10",200,tmin,tmax);
      timeRes10_70 = new TH1F(prefix+"timeRes10_70","timeRes10_70",200,tmin,tmax);
      timeRes70_150 = new TH1F(prefix+"timeRes70_150","timeRes70_150",200,tmin,tmax);
      timeRes150_350 = new TH1F(prefix+"timeRes150_350","timeRes150_350",200,tmin,tmax);
      timeRes350_800 = new TH1F(prefix+"timeRes350_800","timeRes350_800",200,tmin,tmax);

      timePull0_10 = new TH1F(prefix+"timePull0_10","timePull0_10",200,-7,7);
      timePull10_70 = new TH1F(prefix+"timePull10_70","timePull10_70",200,-7,7);
      timePull70_150 = new TH1F(prefix+"timePull70_150","timePull70_150",200,-7,7);
      timePull150_350 = new TH1F(prefix+"timePull150_350","timePull150_350",200,-7,7);
      timePull350_800 = new TH1F(prefix+"timePull350_800","timePull350_800",200,-7,7);
    }else if( type == 1 ){
      timeResCor = new TH1F(prefix+"timeResLargeError","timeResLargeError",200,tmin,tmax);
      timePullCor  = new TH1F(prefix+"timePullLargError","timePullLargError",200,-7,7);
    } 
  }

  void TimePlots::fill( const MuonValidationTimeBlock& times, int index, float betaTrack ) {
    float dist = (*times.d)[index];
    int type = (*times.type)[index];
    float err = (*times.err)[index];
    float t = (*times.time)[index];
    MuonBetaCalculationUtils betaUtils(0.);
    float tof       = betaUtils.calculateTof(1.,dist);
    float timeTrack = betaUtils.calculateTof(betaTrack,dist);
    float t0_       = (*times.time)[index]+tof;
    float betaMeas   = betaUtils.calculateBeta(t0_,dist);
    float betaError  = betaUtils.calculateBetaError(t0_,err,dist);
    float invBetaMeas   = betaUtils.calculateInverseBeta(t0_,dist);
    float invBetaError  = betaUtils.calculateInverseBetaError(err,dist);
    time->Fill(t);
    error->Fill(err);
    beta->Fill(betaMeas);
    float res = t0_-timeTrack;
    float pull = res/err;
    if( type != 1 || err < 7 ){
      timeRes->Fill(res);
      timePull->Fill(pull);
    }
    float betaResidual = betaMeas-betaTrack;
    betaRes->Fill(betaResidual);
    betaPull->Fill(betaResidual/betaError);
    float invBetaResidual = invBetaMeas-1./betaTrack;
    invBetaRes->Fill(invBetaResidual);
    invBetaPull->Fill(invBetaResidual/invBetaError);
    
    float fullTime = (*times.timeProp)[index];
    if( type == 10 ){
      error_time->Fill(fullTime,err);
      timeResScat->Fill(fullTime,res);
      timePullScat->Fill(fullTime,pull);
      float errTrk = (*times.avTimeProp)[index];
      errorTrack->Fill(errTrk);
      float errRT = sqrt(err*err-errTrk*errTrk);
      errorRT->Fill(errRT);
      errorRT_Track->Fill(errRT,errTrk);
      float rescor = res-(*times.timeCor)[index];
      float pullcor = rescor/err;
      timeResCor->Fill(rescor);
      timePullCor->Fill(pullcor);
      if( fullTime < 10 )       timeRes0_10->Fill(res);
      else if( fullTime < 50 )  timeRes10_70->Fill(res);
      else if( fullTime < 150 ) timeRes70_150->Fill(res);
      else if( fullTime < 350 ) timeRes150_350->Fill(res);
      else timeRes350_800->Fill(res);

      if( fullTime < 10 )       timePull0_10->Fill(pull);
      else if( fullTime < 50 )  timePull10_70->Fill(pull);
      else if( fullTime < 150 ) timePull70_150->Fill(pull);
      else if( fullTime < 350 ) timePull150_350->Fill(pull);
      else timePull350_800->Fill(pull);

    }else if( type == 1 && err > 7 ){
      timeResCor->Fill(res);
      timePullCor->Fill(pull);
    }
  }

  void HoughPlots::book( TDirectory* dir, const TString& prefix ) {

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

  void SegmentPlots::book( TDirectory* dir, const TString& prefix ){
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


  void SegmentPlots::fill( const MuonValidationSegmentBlock& segments, int index, float p_, float betaTrack ) {

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
    MuonBetaCalculationUtils betaUtils(0.);
    float tof       = betaUtils.calculateTof(1.,dist);
    float t0_       = (*segments.t0)[index]+tof;
    float t0Trig_   = (*segments.t0Trig)[index]+tof;
    float t0Track   = betaUtils.calculateTof(betaTrack,dist);
    float betaSeg   = betaUtils.calculateBeta(t0_,dist);
    float betaSegTrig = betaUtils.calculateBeta(t0Trig_,dist);
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

  void BetaFitPlots::book( TDirectory* , const TString& prefix ) {
    beta = new TH1F(prefix+"beta","beta",100,0,1.5);
    chi2Ndof = new TH1F(prefix+"chi2Ndof","chi2Ndof",100,0,50.);
    ndof = new TH1F(prefix+"ndof","ndof",51,-0.5,50.5);
    res = new TH1F(prefix+"res","res",500,-1.,1.);
  }

  void BetaFitPlots::fill( float beta_, float betaTruth_, float chi2_, int ndof_  ) {
    beta->Fill(beta_);
    if( ndof_ > 0 ) chi2Ndof->Fill(chi2_/ndof_);
    ndof->Fill(ndof_);
    res->Fill(beta_-betaTruth_);
  }

  void BetaFitRegionPlots::book( TDirectory* dir, const TString& prefix, bool isBarrel ) {
    mdt.book(dir,prefix+"mdt_");
    mdtt.book(dir,prefix+"mdtt_");
    mdtt_good.book(dir,prefix+"mdtt_good_");
    if( isBarrel ){
      rpc.book(dir,prefix+"rpc_");
      rpct.book(dir,prefix+"rpcr_");
      all.book(dir,prefix+"all_");
      allt.book(dir,prefix+"allt_");
      allt_good.book(dir,prefix+"allt_good_");
    }
  }

  /** candidate based plots */
  void CandidatePlots::book( TDirectory* dir, const TString& prefix ) {
    pt = new TH1F(prefix+"pt","pt",100,0,300.);
    eta = new TH1F(prefix+"eta","eta",100,-3,3);
    phi = new TH1F(prefix+"phi","phi",100,-3,3);
    beta = new TH1F(prefix+"beta","beta",100,0,1);
    nseg = new TH1F(prefix+"nseg","nseg",10,-0.5,9.5);

    betaCandidates.book(dir,prefix+"all_");
    betaBestCandidate.book(dir,prefix+"best_");
  }

  void CandidatePlots::fill( const MuonValidationTrackParticleBlock& tracks, int index ) {
    pt->Fill( (*tracks.pt)[index]*0.001 );
    eta->Fill( (*tracks.eta)[index] );
    phi->Fill( (*tracks.phi)[index] );
    beta->Fill( (*tracks.truth.beta)[index] );
  }


  /** candidates based plots */
  void StageSummaryPlots::book( TDirectory* dir, const TString& prefix ) {
    etaMissed = new TH1F(prefix+"etaMissed","etaMissed",100,-3,3);
    etaMissedCombined = new TH1F(prefix+"etaMissedCombined","etaMissedCombined",100,-3,3);
    etaBetaMissed = new TH2F(prefix+"etaBetaMissed","etaBetaMissed",100,-3,3,100,0.,1.);
    betaMissed = new TH1F(prefix+"betaMissed","betaMissed",100,0.,1.);
    betaMissedCombined = new TH1F(prefix+"betaMissedCombined","betaMissedCombined",100,0.,1.);
    ncandidates = new TH1F(prefix+"ncandidates","ncandidates",10,-0.5,9.5);
    ncombinedCandidates = new TH1F(prefix+"ncombinedCandidates","ncombinedCandidates",10,-0.5,9.5);
    allCandidates.book(dir,prefix+"allCandidates_");
    tagCandidates.book(dir,prefix+"tagCandidates_");
    combinedCandidates.book(dir,prefix+"combinedCandidates_");
  }

  void TrackPlots::book( TDirectory* dir, const TString& prefix ) {
    pt = new TH1F(prefix+"pt","pt",100,0,300.);
    eta = new TH1F(prefix+"eta","eta",100,-3,3);
    phi = new TH1F(prefix+"phi","phi",100,-3,3);
    beta = new TH1F(prefix+"beta","beta",100,0,1);
    pdg = new TH1F(prefix+"pdg","pdg",31,-15.5,15.5);
    ntruth = new TH1F(prefix+"ntruth","ntruth",10,-0.5,9.5);
    nseg = new TH1F(prefix+"nseg","nseg",10,-0.5,9.5);
    nseg1 = new TH1F(prefix+"nseg1","nseg",10,-0.5,9.5);
    nseg2 = new TH1F(prefix+"nseg2","nseg",10,-0.5,9.5);
    nseg3 = new TH1F(prefix+"nseg3","nseg",10,-0.5,9.5);
    nhough = new TH1F(prefix+"nhough","nhough",10,-0.5,9.5);
    ntruth_seg = new TH2F(prefix+"ntruth_seg","ntruth_seg",10,-0.5,9.5,10,-0.5,9.5);
    ntruth_seg1 = new TH2F(prefix+"ntruth_seg1","ntruth_seg",10,-0.5,9.5,10,-0.5,9.5);
    ntruth_hough = new TH2F(prefix+"ntruth_hough","ntruth_hough",10,-0.5,9.5,10,-0.5,9.5);

    TDirectory* ldir = dir->mkdir("hits");
    ldir->cd();
    hits.book(ldir,prefix+"h_");
    
    ldir = dir->mkdir("hough");
    ldir->cd();
    hough.book(ldir,prefix+"ho_");

    ldir = dir->mkdir("segments");
    ldir->cd();
    segments.book(ldir,prefix+"s0_");
    segments1.book(ldir,prefix+"s1_");
    segments2.book(ldir,prefix+"s2_");
    segments3.book(ldir,prefix+"s3_");
    
    ldir = dir->mkdir("times");
    ldir->cd();
    std::vector<int> types = { 1,2,10,12,1001,1010};
    std::vector<std::string> typeNames = { "BarrelMDT","BarrelRPC","BarrelMDTT","BarrelRPCT","EndcapMDT","EndcapMDTT"};
    for( unsigned int i=0;i<types.size();++i ){
      auto type = types[i];
      TString name = typeNames[i].c_str();
      name += "_";
      timePlots.push_back(TimePlots());
      timePlots.back().book(ldir,name,type);
      timePlots.back().typeIndex = type;
    }


    ldir = dir->mkdir("betaFits");
    ldir->cd();
    barrel.book(ldir,prefix+"barrel_",true);
    endcap.book(ldir,prefix+"endcap_",false);

    ldir = dir->mkdir("candidates");
    ldir->cd();
    for( unsigned int i=0;i<4;++i ){
      TString name = "stage";
      name += i;
      candidateStages.push_back(StageSummaryPlots());
      candidateStages.back().book(ldir,name);
    }
    dir->cd();
  }

  void TrackPlots::fill( const MuonValidationTrackParticleBlock& tracks, int index ) {
    pt->Fill( (*tracks.pt)[index]*0.001 );
    eta->Fill( (*tracks.eta)[index] );
    phi->Fill( (*tracks.phi)[index] );
    beta->Fill( (*tracks.truth.beta)[index] );
  }

  void TrackPlots::fill( int ntruth_, int nseg_, int nseg1_, int nseg2_, int nseg3_, int nhough_ ) {
    ntruth->Fill(ntruth_);
    nseg->Fill(nseg_);
    nseg1->Fill(nseg1_);
    nseg2->Fill(nseg2_);
    nseg3->Fill(nseg3_);
    nhough->Fill(nhough_);
    ntruth_seg->Fill(ntruth_,nseg_);
    ntruth_seg1->Fill(ntruth_,nseg1_);
    ntruth_hough->Fill(ntruth_,nhough_);
  }

  
  void MuonInsideOutAnalysisPlots::book( TDirectory* dir, const TString& prefix ) {
    TDirectory* ndir = dir->mkdir("Muon");
    ndir->cd();
    muon.book(ndir,prefix);
    ndir = dir->mkdir("Stau");
    ndir->cd();
    stau.book(ndir,prefix);
    ndir = dir->mkdir("Rest");
    ndir->cd();
    rest.book(ndir,prefix);
    dir->cd();
  }
}
