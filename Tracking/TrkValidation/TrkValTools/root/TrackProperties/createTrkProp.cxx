// $:$Id: createTrkProp.cxx, v 1.0  04/17/2006 12:07:07 
// Author: Miguel Angel Olivo Gomez   04/17/06

/*************************************************************************
 *                                                                       *
 * Copyright (C) 2006                                    .               *
 * All rights reserved.                                                  *
 *                                                                       *
 *************************************************************************/


#include <iostream>
#include <math.h>
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TH1TrkProp.h"
#include "TH2TrkProp.h"


void createTrkProp() {


  // which plots do you want?
  bool doNumberOfTrk              =         true;
  bool doDceroPerigee             =         true;
  bool doZceroPerigee             =         true;
  bool doPhiPerigee               =         true;
  bool doThetaPerigee             =         true;
  bool doEtaPerigee               =         true;
  bool doQOverPPerigee            =         true;
  bool doTrkChiSquarePerigee      =         true;
  bool doNumberOfHits             =         true;
  bool doXVertex                  =         false;
  bool doYVertex                  =         false;
  bool doZVertex                  =         false;


  const char* format              =         "gif";  
  const char* rootfile            =         "trkValNtupSingleMuons.root";
  const char* dir                 =         "TrkValNewTracking";
  const char* ntupleTrkVal        =         "TrkValNewTrkProp";
  const char* ntupleFitter        =         "FitterValNewTrkProp";
  const char* noUnit              =         "";
  const char* mm                  =         "[mm]";
  const char* rad                 =         "[rad]";
  const char* mev                 =         "[MeV]";
  const char* gev                 =         "[GeV]";
  const char* reciMev             =         "[1/MeV]";


  //const char* cut                 =         "";
  //const char* cutName             =         "noCut";
  const char* cut                 =         "trk_Rec_nTracks==1";
  const char* cutName             =         "trkEq1";
  //const char* cut                 =         "abs(1/trk_Rec_qOverP)>10000.0";
  //const char* cutName             =         "momGt2GeV";



  //_________________________________________________________________________________
  

  //--- number of tracks per event ---

  const char* noTrkEvt            =         "trk_Rec_nTracks";
  const int noTrkNbin             =          40;  //<binwidth=1
  const double noTrkLow           =         -0.75;
  const double noTrkUp            =          19.25;


  //--- d0 (perigee) ---

  const char* d0Rec               =         "trk_Rec_d0";
  const int d0Nbin                =          100; //<binwidth=
  const double d0Low              =         -0.1;
  const double d0Up               =          0.1;
  const char* d0TruthAssoc        =         "trk_Mc_d0";
  const int d0TruthAssocNbin      =          100; //<binwidth=
  const double d0TruthAssocLow    =         -0.1;
  const double d0TruthAssocUp     =          0.1;
  const char* d0Error             =         "trk_Rec_error_d0";
  const int d0ErrNbin             =          100; //<binwidth=
  const double d0ErrLow           =          0.005;
  const double d0ErrUp            =          0.016;
  const char* d0Resol             =         "trk_Rec_d0-trk_Mc_d0";
  const int d0ResolNbin           =          100; //<binwidth=
  const double d0ResolLow         =         -0.04;
  const double d0ResolUp          =          0.04;
  const char* d0Pull              =         "(trk_Rec_d0-trk_Mc_d0)/trk_Rec_error_d0";
  const int d0PullNbin            =          100; //<binwidth=
  const double d0PullLow          =         -5.0;
  const double d0PullUp           =          5.0;


  //--- z0 (perigee) ---

  const char* z0Rec               =         "trk_Rec_z0";
  const int z0Nbin                =          100; //<binwidth=
  const double z0Low              =         -200.0;
  const double z0Up               =          200.0;
  const char* z0TruthAssoc        =         "trk_Mc_z0";
  const int z0TruthAssocNbin      =          100; //<binwidth=
  const double z0TruthAssocLow    =         -200.0;
  const double z0TruthAssocUp     =          200.0;
  const char* z0Error             =         "trk_Rec_error_z0";
  const int z0ErrNbin             =          60; //<binwidth=
  const double z0ErrLow           =          0.03;
  const double z0ErrUp            =          0.23;
  const char* z0Resol             =         "trk_Rec_z0-trk_Mc_z0";
  const int z0ResolNbin           =          100; //<binwidth=
  const double z0ResolLow         =         -0.7;
  const double z0ResolUp          =          0.7;
  const char* z0Pull              =         "(trk_Rec_z0-trk_Mc_z0)/trk_Rec_error_z0";
  const int z0PullNbin            =          100; //<binwidth=
  const double z0PullLow          =         -5.0;
  const double z0PullUp           =          5.0;


  //--- phi (perigee) ---

  const char* phiRec              =         "trk_Rec_phi0";
  const int phiNbin               =          72;  //<binwidth=5grad
  const double phiLow             =         -(TMath::Pi());        // -M_PI(cxx)
  const double phiUp              =          TMath::Pi();          //  M_PI
  const char* phiTruthAssoc       =         "trk_Mc_phi0";
  const int phiTruthAssocNbin     =          72;  //<binwidth=
  const double phiTruthAssocLow   =         -(TMath::Pi());        // -M_PI(cxx)
  const double phiTruthAssocUp    =          TMath::Pi();          //  M_PI
  const char* phiError            =         "trk_Rec_error_phi0";
  const int phiErrNbin            =          75; //<binwidth=
  const double phiErrLow          =          0.00007;
  const double phiErrUp           =          0.00022;
  const char* phiResol            =         "trk_Rec_phi0-trk_Mc_phi0";
  const int phiResolNbin          =          100; //<binwidth=
  const double phiResolLow        =         -0.0006;
  const double phiResolUp         =          0.0006;
  const char* phiPull             =         "(trk_Rec_phi0-trk_Mc_phi0)/trk_Rec_error_phi0";
  const int phiPullNbin           =          100; //<binwidth=
  const double phiPullLow         =         -5.0;
  const double phiPullUp          =          5.0;


  //--- theta (perigee) ---

  const char* thetaRec            =         "trk_Rec_theta";
  const int thetaNbin             =          90;  //<binwidth=2grad
  const double thetaLow           =          0.0;
  const double thetaUp            =          TMath::Pi(); //M_PI
  const char* thetaTruthAssoc     =         "trk_Mc_theta";
  const int thetaTruthAssocNbin   =          90;  //<binwidth=
  const double thetaTruthAssocLow =          0.0;
  const double thetaTruthAssocUp  =          TMath::Pi(); //M_PI
  const char* thetaError          =         "trk_Rec_error_theta";
  const int thetaErrNbin          =          100; //<binwidth=
  const double thetaErrLow        =          0.00005;
  const double thetaErrUp         =          0.0008;
  const char* thetaResol          =         "trk_Rec_theta-trk_Mc_theta";
  const int thetaResolNbin        =          100; //<binwidth=
  const double thetaResolLow      =         -0.0015;
  const double thetaResolUp       =          0.0015;
  const char* thetaPull           =         "(trk_Rec_theta-trk_Mc_theta)/trk_Rec_error_theta";
  const int thetaPullNbin         =          100; //<binwidth=
  const double thetaPullLow       =         -5.0;
  const double thetaPullUp        =          5.0;


  //--- eta (perigee) ---

  const char* etaRec              =         "trk_Rec_eta";
  const int etaNbin               =          60;  //<binwidth=0.1
  const double etaLow             =         -3.0;
  const double etaUp              =          3.0;
  const char* etaTruthAssoc       =         "trk_Mc_eta";
  const int etaTruthAssocNbin     =          60;  //<binwidth=
  const double etaTruthAssocLow   =         -3.0;
  const double etaTruthAssocUp    =          3.0;
  const char* etaError            =         "trk_Rec_error_theta/sin(trk_Rec_theta)";
  const int etaErrNbin            =          100; //<binwidth=
  const double etaErrLow          =          0.0001;
  const double etaErrUp           =          0.0011;
  const char* etaResol            =         "trk_Rec_eta-trk_Mc_eta";
  const int etaResolNbin          =          100; //<binwidth=
  const double etaResolLow        =         -0.003;
  const double etaResolUp         =          0.003;
  const char* etaPull             =         "(trk_Rec_eta-trk_Mc_eta)/(trk_Rec_error_theta/sin(trk_Rec_theta))";
  const int etaPullNbin           =          100; //<binwidth=
  const double etaPullLow         =         -5.0;
  const double etaPullUp          =          5.0;


  //--- q over p (perigee) ---

  const char* momRec              =         "abs(1/trk_Rec_qOverP)/1000.0";  // c++ fabs(double)
  const int momNbin               =          100; //<binwidth=
  const double momLow             =          48.0;
  const double momUp              =          110.0;
  const char* momTruthAssoc       =         "abs(1/trk_Mc_qOverP)/1000.0";
  const int momTruthAssocNbin     =          100; //<binwidth=
  const double momTruthAssocLow   =          48.0;
  const double momTruthAssocUp    =          110.0;
  const char* momError            =         "(trk_Rec_error_qOverP/pow(trk_Rec_qOverP,2.0))/1000.0";
  const int momErrNbin            =          100; //<binwidth=
  const double momErrLow          =          0.8;
  const double momErrUp           =          5.0;
  const char* momResol            =         "(abs(1/trk_Rec_qOverP)-abs(1/trk_Mc_qOverP))/1000.0";
  const int momResolNbin          =          100; //<binwidth=
  const double momResolLow        =         -15.0;
  const double momResolUp         =          15.0;
  const char* momPull             =         "(abs(1/trk_Rec_qOverP)-abs(1/trk_Mc_qOverP))/(trk_Rec_error_qOverP/pow(trk_Rec_qOverP,2.0))"; 
  const int momPullNbin           =          100; //<binwidth=
  const double momPullLow         =         -5.0;
  const double momPullUp          =          5.0;


  const char* pzRec               =         "(cos(trk_Rec_theta)/abs(trk_Rec_qOverP))/1000.0";
  const int pzNbin                =          50; //<binwidth=
  const double pzLow              =         -300.0;
  const double pzUp               =          300.0;
  const char* pzTruthAssoc        =         "(cos(trk_Mc_theta)/abs(trk_Mc_qOverP))/1000.0";
  const int pzTruthAssocNbin      =          50; //<binwidth=
  const double pzTruthAssocLow    =         -300.0;
  const double pzTruthAssocUp     =          300.0;
  const char* pzError             =         "(abs(cos(trk_Rec_theta)/trk_Rec_qOverP)*sqrt(pow(trk_Rec_error_qOverP/trk_Rec_qOverP,2.0)+pow(tan(trk_Rec_theta)*trk_Rec_error_theta,2.0)))/1000.0";
  const int pzErrNbin             =          50; //<binwidth=
  const double pzErrLow           =          0.0;
  const double pzErrUp            =          5.0;
  const char* pzResol             =         "((cos(trk_Rec_theta)/abs(trk_Rec_qOverP))-(cos(trk_Mc_theta)/abs(trk_Mc_qOverP)))/1000.0";
  const int pzResolNbin           =          100; //<binwidth=
  const double pzResolLow         =         -5.0;
  const double pzResolUp          =          5.0;
  const char* pzPull              =         "((cos(trk_Rec_theta)/abs(trk_Rec_qOverP))-(cos(trk_Mc_theta)/abs(trk_Mc_qOverP)))/(abs(cos(trk_Rec_theta)/trk_Rec_qOverP)*sqrt(pow(trk_Rec_error_qOverP/trk_Rec_qOverP,2.0)+pow(tan(trk_Rec_theta)*trk_Rec_error_theta,2.0)))";
  const int pzPullNbin            =          100; //<binwidth=
  const double pzPullLow          =         -5.0;
  const double pzPullUp           =          5.0;


  const char* ptRec               =         "abs(1/trk_Rec_qOverPt)/1000.0";
  const int ptNbin                =          100; //<binwidth=
  const double ptLow              =          40.0;
  const double ptUp               =          60.0;
  const char* ptTruthAssoc        =         "abs(1/trk_Mc_qOverPt)/1000.0";
  const int ptTruthAssocNbin      =          40; //<binwidth=
  const double ptTruthAssocLow    =          48.0;
  const double ptTruthAssocUp     =          52.0;
  const char* ptError             =         "(abs(1/trk_Rec_qOverPt)*sqrt(pow(trk_Rec_error_qOverP/trk_Rec_qOverP,2.0)+pow(trk_Rec_error_theta/tan(trk_Rec_theta),2.0)))/1000.0";
  const int ptErrNbin             =          100; //<binwidth=
  const double ptErrLow           =          0.8;
  const double ptErrUp            =          3.0;
  const char* ptResol             =         "(abs(1/trk_Rec_qOverPt)-abs(1/trk_Mc_qOverPt))/1000.0";
  const int ptResolNbin           =          100; //<binwidth=
  const double ptResolLow         =         -10.0;
  const double ptResolUp          =          10.0;
  const char* ptPull              =         "(abs(1/trk_Rec_qOverPt)-abs(1/trk_Mc_qOverPt))/(abs(1/trk_Rec_qOverPt)*sqrt(pow(trk_Rec_error_qOverP/trk_Rec_qOverP,2.0)+pow(trk_Rec_error_theta/tan(trk_Rec_theta),2.0)))";
  const int ptPullNbin            =          100; //<binwidth=
  const double ptPullLow          =         -5.0;
  const double ptPullUp           =          5.0;
  const char* ptErrorOverPt       =         "(abs(1/trk_Rec_qOverPt)*sqrt(pow(trk_Rec_error_qOverP/trk_Rec_qOverP,2.0)+pow(trk_Rec_error_theta/tan(trk_Rec_theta),2.0)))*abs(trk_Rec_qOverPt)";
  const int ptErrOverPtNbin       =          100; //<binwidth=
  const double ptErrOverPtLow     =          0.015;
  const double ptErrOverPtUp      =          0.05;


  //--- track chi2 (perigee) ---

  const char* trkChi2             =         "trk_Rec_chi2";
  const int trkChi2Nbin           =          100; //<binwidth=
  const double trkChi2Low         =          0.0;
  const double trkChi2Up          =          100.0;
  const char* trkNdf              =         "trk_Rec_nDoF";
  const int trkNdfNbin            =          140; //<binwidth=
  const double trkNdfLow          =         -0.75;
  const double trkNdfUp           =          69.25;
  const char* trkChi2DoF          =         "trk_Rec_chi2PerDoF";
  const int trkChi2DoFNbin        =          72; //<binwidth=
  const double trkChi2DoFLow      =          0.2;
  const double trkChi2DoFUp       =          2.0;
  const char* trkChi2Prob         =          "1-trk_Rec_chi2Prob"; //"TMath::Prob(trk_Rec_chi2,trk_Rec_nDoF))"
  const int trkChi2ProbNbin       =          80; //<binwidth=
  const double trkChi2ProbLow     =          0.0;
  const double trkChi2ProbUp      =          1.0;


  //--- Number of hits ---

  const char* pixelHit            =         "trk_Rec_nPixelHits";
  const int pixelHitNbin          =          20; //<binwidth=1
  const double pixelHitLow        =         -0.75;
  const double pixelHitUp         =          9.25;
  const char* sctHit              =         "trk_Rec_nSctHits";
  const int sctHitNbin            =          36; //<binwidth=1
  const double sctHitLow          =         -0.75;
  const double sctHitUp           =          17.25;
  const char* trtHit              =         "trk_Rec_nTrtHits";
  const int trtHitNbin            =          100; //<binwidth=1
  const double trtHitLow          =         -0.75;
  const double trtHitUp           =          49.25;


  //--- x of vertex ---

  const int xVtxNbin              =          60;//<binwidth=0.001
  const int xVtxErrNbin           =          100;//<binwidth=
  const double xVtxLow            =         -0.03;
  const double xVtxUp             =          0.03;


  //--- y of vertex ---

  const int yVtxNbin              =          60;//<binwidth=0.001
  const int yVtxErrNbin           =          100;//<binwidth=
  const double yVtxLow            =         -0.03;
  const double yVtxUp             =          0.03;
  const double yVtxErrLow         =          0.007;
  const double yVtxErrUp          =          0.016;


  //--- z of vertex ---

  const int zVtxNbin              =          60; //<binwidth=5
  const double zVtxLow            =         -150.0;
  const double zVtxUp             =          150.0;



  //___________________________________________________________________________________

  if (doNumberOfTrk) {

    TH1TrkProp(format,rootfile,dir,ntupleFitter,noTrkEvt,"numberTracksEvent",noUnit,cut,cutName,
	       "numberTracksEvent",noTrkNbin,noTrkLow,noTrkUp,true,false);
  }



  if (doDceroPerigee) {

    TH1TrkProp(format,rootfile,dir,ntupleFitter,d0Rec,"d0",mm,cut,cutName,"d0",
	       d0Nbin,d0Low,d0Up,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,d0TruthAssoc,"d0TruthAssoc",mm,cut,cutName,
	       "d0TruthAssoc",d0TruthAssocNbin,d0TruthAssocLow,d0TruthAssocUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,d0Error,"d0Error",mm,cut,cutName,
	       "d0Error",d0ErrNbin,d0ErrLow,d0ErrUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,d0Resol,"d0-d0TruthAssoc",mm,cut,cutName,
	       "d0Resolution",d0ResolNbin,d0ResolLow,d0ResolUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,d0Pull,"(d0-d0TruthAssoc)/d0Error",noUnit,
	       cut,cutName,"d0Pull",d0PullNbin,d0PullLow,d0PullUp,false,true);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,d0TruthAssoc,"d0TruthAssoc",mm,d0Rec,"d0",mm,cut,
	       cutName,"d0:d0TruthAssoc",d0Nbin,d0Low,d0Up,d0TruthAssocNbin,d0TruthAssocLow,
	       d0TruthAssocUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,d0Error,"d0Error",mm,cut,cutName,
	       "d0Error:phi",phiNbin,phiLow,phiUp,d0ErrNbin,d0ErrLow,d0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,d0Error,"d0Error",mm,cut,cutName,
	       "d0Error:eta",etaNbin,etaLow,etaUp,d0ErrNbin,d0ErrLow,d0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,d0Error,"d0Error",mm,cut,cutName,
	       "d0Error:pt",ptNbin,ptLow,ptUp,d0ErrNbin,d0ErrLow,d0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,d0Error,"d0Error",mm,cut,
	       cutName,"d0Error:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,d0ErrNbin,
	       d0ErrLow,d0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,d0Error,"d0Error",mm,cut,
	       cutName,"d0Error:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,d0ErrNbin,d0ErrLow,
	       d0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,d0Error,"d0Error",mm,cut,
	       cutName,"d0Error:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,d0ErrNbin,d0ErrLow,
	       d0ErrUp,false,false);
  }



  if (doZceroPerigee) {

    TH1TrkProp(format,rootfile,dir,ntupleFitter,z0Rec,"z0",mm,cut,cutName,"z0",
	       z0Nbin,z0Low,z0Up,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,z0TruthAssoc,"z0TruthAssoc",mm,cut,cutName,
	       "z0TruthAssoc",z0TruthAssocNbin,z0TruthAssocLow,z0TruthAssocUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,z0Error,"z0Error",mm,cut,cutName,
	       "z0Error",z0ErrNbin,z0ErrLow,z0ErrUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,z0Resol,"z0-z0TruthAssoc",mm,cut,cutName,
	       "z0Resolution",z0ResolNbin,z0ResolLow,z0ResolUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,z0Pull,"(z0-z0TruthAssoc)/z0Error",noUnit,
	       cut,cutName,"z0Pull",z0PullNbin,z0PullLow,z0PullUp,false,true);    
    TH2TrkProp(format,rootfile,dir,ntupleFitter,z0TruthAssoc,"z0TruthAssoc",mm,z0Rec,"z0",mm,cut,
	       cutName,"z0:z0TruthAssoc",z0Nbin,z0Low,z0Up,z0TruthAssocNbin,z0TruthAssocLow,
	       z0TruthAssocUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,z0Error,"z0Error",mm,cut,cutName,
	       "z0Error:phi",phiNbin,phiLow,phiUp,z0ErrNbin,z0ErrLow,z0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,z0Error,"z0Error",mm,cut,cutName,
	       "z0Error:eta",etaNbin,etaLow,etaUp,z0ErrNbin,z0ErrLow,z0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,z0Error,"z0Error",mm,cut,cutName,
	       "z0Error:pt",ptNbin,ptLow,ptUp,z0ErrNbin,z0ErrLow,z0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,z0Error,"z0Error",mm,cut,
	       cutName,"z0Error:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,z0ErrNbin,
	       z0ErrLow,z0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,z0Error,"z0Error",mm,cut,
	       cutName,"z0Error:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,z0ErrNbin,z0ErrLow,
	       z0ErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,z0Error,"z0Error",mm,cut,
	       cutName,"z0Error:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,z0ErrNbin,z0ErrLow,
	       z0ErrUp,false,false);
  }



  if (doPhiPerigee) {

    TH1TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,cut,cutName,"phi",
	       phiNbin,phiLow,phiUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,phiTruthAssoc,"phiTruthAssoc",rad,cut,cutName,
	       "phiTruthAssoc",phiTruthAssocNbin,phiTruthAssocLow,phiTruthAssocUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,phiError,"phiError",rad,cut,cutName,
	       "phiError",phiErrNbin,phiErrLow,phiErrUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,phiResol,"phi-phiTruthAssoc",rad,cut,cutName,
	       "phiResolution",phiResolNbin,phiResolLow,phiResolUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,phiPull,"(phi-phiTruthAssoc)/phiError",noUnit,
	       cut,cutName,"phiPull",phiPullNbin,phiPullLow,phiPullUp,false,true);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiTruthAssoc,"phiTruthAssoc",rad,phiRec,"phi",
	       rad,cut,cutName,"phi:phiTruthAssoc",phiNbin,phiLow,phiUp,phiTruthAssocNbin,
	       phiLow,phiUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,phiError,"phiError",rad,cut,cutName,
               "phiError:phi",phiNbin,phiLow,phiUp,phiErrNbin,phiErrLow,phiErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,phiError,"phiError",rad,cut,
	       cutName,"phiError:eta",etaNbin,etaLow,etaUp,phiErrNbin,phiErrLow,phiErrUp,
	       false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,phiError,"phiError",rad,cut,cutName,
	       "phiError:pt",ptNbin,ptLow,ptUp,phiErrNbin,phiErrLow,phiErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,phiError,"phiError",rad,
	       cut,cutName,"phiError:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,
	       phiErrNbin,phiErrLow,phiErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,phiError,"phiError",rad,cut,
	       cutName,"phiError:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,phiErrNbin,phiErrLow,
	       phiErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,phiError,"phiError",rad,cut,
	       cutName,"phiError:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,phiErrNbin,phiErrLow,
	       phiErrUp,false,false);
  }



  if (doThetaPerigee) {

    TH1TrkProp(format,rootfile,dir,ntupleFitter,thetaRec,"theta",rad,cut,cutName,"theta",thetaNbin,
	       thetaLow,thetaUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,thetaTruthAssoc,"thetaTruthAssoc",rad,cut,cutName,
	       "thetaTruthAssoc",thetaNbin,thetaTruthAssocLow,thetaTruthAssocUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,thetaError,"thetaError",rad,cut,cutName,
	       "thetaError",thetaErrNbin,thetaErrLow,thetaErrUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,thetaResol,"theta-thetaTruthAssoc",rad,cut,cutName,
	       "thetaResolution",thetaResolNbin,thetaResolLow,thetaResolUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,thetaPull,"(theta-thetaTruthAssoc)/thetaError",noUnit,
	       cut,cutName,"thetaPull",thetaPullNbin,thetaPullLow,thetaPullUp,false,true);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,thetaTruthAssoc,"thetaTruthAssoc",rad,thetaRec,
	       "theta",rad,cut,cutName,"theta:thetaTruthAssoc",thetaNbin,thetaLow,thetaUp,
	       thetaTruthAssocNbin,thetaLow,thetaUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,thetaError,"thetaError",rad,cut,
	       cutName,"thetaError:phi",phiNbin,phiLow,phiUp,thetaErrNbin,thetaErrLow,
	       thetaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,thetaError,"thetaError",
	       rad,cut,cutName,"thetaError:eta",etaNbin,etaLow,etaUp,thetaErrNbin,
	       thetaErrLow,thetaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,thetaError,"thetaError",rad,cut,cutName,
               "thetaError:pt",ptNbin,ptLow,ptUp,thetaErrNbin,thetaErrLow,thetaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,thetaError,"thetaError",rad,
               cut,cutName,"thetaError:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,
               thetaErrNbin,thetaErrLow,thetaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,thetaError,"thetaError",
	       rad,cut,cutName,"thetaError:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,
	       thetaErrNbin,thetaErrLow,thetaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,thetaError,"thetaError",
	       rad,cut,cutName,"thetaError:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,
	       thetaErrNbin,thetaErrLow,thetaErrUp,false,false);
  }



  if (doEtaPerigee) { 

    TH1TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,cut,cutName,
	       "eta",etaNbin,etaLow,etaUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,etaTruthAssoc,"etaTruthAssoc",noUnit,cut,cutName,
	       "etaTruthAssoc",etaTruthAssocNbin,etaTruthAssocLow,etaTruthAssocUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,etaError,"etaError",noUnit,cut,cutName,
	       "etaError",etaErrNbin,etaErrLow,etaErrUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,etaResol,"eta-etaTruthAssoc",noUnit,cut,cutName,
	       "etaResolution",etaResolNbin,etaResolLow,etaResolUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,etaPull,"(eta-etaTruthAssoc)/etaError",noUnit,
	       cut,cutName,"etaPull",etaPullNbin,etaPullLow,etaPullUp,false,true);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaTruthAssoc,"etaTruthAssoc",noUnit,etaRec,
	       "eta",noUnit,cut,cutName,"eta:etaTruthAssoc",etaNbin,etaLow,etaUp,
	       etaTruthAssocNbin,etaLow,etaUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,etaError,"etaError",noUnit,cut,
	       cutName,"etaError:phi",phiNbin,phiLow,phiUp,etaErrNbin,etaErrLow,
	       etaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,etaError,"etaError",
	       noUnit,cut,cutName,"etaError:eta",etaNbin,etaLow,etaUp,etaErrNbin,
	       etaErrLow,etaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,etaError,"etaError",noUnit,cut,cutName,
               "etaError:pt",ptNbin,ptLow,ptUp,etaErrNbin,etaErrLow,etaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,etaError,"etaError",noUnit,
               cut,cutName,"etaError:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,
               etaErrNbin,etaErrLow,etaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,etaError,"etaError",
	       noUnit,cut,cutName,"etaError:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,
	       etaErrNbin,etaErrLow,etaErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,etaError,"etaError",
	       noUnit,cut,cutName,"etaError:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,
	       etaErrNbin,etaErrLow,etaErrUp,false,false);
  }




  if (doQOverPPerigee) {
    
    TH1TrkProp(format,rootfile,dir,ntupleFitter,momRec,"mom",gev,cut,cutName,
	       "mom",momNbin,momLow,momUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,momTruthAssoc,"momTruthAssoc",gev,cut,cutName,
	       "momTruthAssoc",momTruthAssocNbin,momTruthAssocLow,momTruthAssocUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,momError,"momError",gev,cut,cutName,
	       "momError",momErrNbin,momErrLow,momErrUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,momResol,"mom-momTruthAssoc",gev,cut,cutName,
	       "momResolution",momResolNbin,momResolLow,momResolUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,momPull,"(mom-momTruthAssoc)/momError",noUnit,
	       cut,cutName,"momPull",momPullNbin,momPullLow,momPullUp,false,true);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,momTruthAssoc,"momTruthAssoc",gev,momRec,
	       "mom",gev,cut,cutName,"mom:momTruthAssoc",momNbin,momLow,momUp,
	       momTruthAssocNbin,momLow,momUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,momError,"momError",gev,cut,
	       cutName,"momError:phi",phiNbin,phiLow,phiUp,momErrNbin,momErrLow,momErrUp,
	       false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,momError,"momError",
	       gev,cut,cutName,"momError:eta",etaNbin,etaLow,etaUp,momErrNbin,
	       momErrLow,momErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,momError,"momError",gev,cut,cutName,
               "momError:pt",ptNbin,ptLow,ptUp,momErrNbin,momErrLow,momErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,momError,"momError",gev,
               cut,cutName,"momError:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,
               momErrNbin,momErrLow,momErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,momError,"momError",
	       gev,cut,cutName,"momError:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,
	       momErrNbin,momErrLow,momErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,momError,"momError",
	       gev,cut,cutName,"momError:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,
	       momErrNbin,momErrLow,momErrUp,false,false);
    

    TH1TrkProp(format,rootfile,dir,ntupleFitter,pzRec,"pz",gev,cut,cutName,
	       "pz",pzNbin,pzLow,pzUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,pzTruthAssoc,"pzTruthAssoc",gev,cut,cutName,
	       "pzTruthAssoc",pzTruthAssocNbin,pzTruthAssocLow,pzTruthAssocUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,pzError,"pzError",gev,cut,cutName,
    	       "pzError",pzErrNbin,pzErrLow,pzErrUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,pzResol,"pz-pzTruthAssoc",gev,cut,cutName,
    	       "pzResolution",pzResolNbin,pzResolLow,pzResolUp,false,false);
    //TH1TrkProp(format,rootfile,dir,ntupleFitter,pzPull,"(pz-pzTruthAssoc)/pzError",noUnit,
    //       cut,cutName,"pzPull",pzPullNbin,pzPullLow,pzPullUp,false,true);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pzTruthAssoc,"pzTruthAssoc",gev,pzRec,"pz",
	       gev,cut,cutName,"pz:pzTruthAssoc",pzNbin,pzLow,pzUp,pzTruthAssocNbin,pzLow,
	       pzUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,pzError,"pzError",gev,cut,
	       cutName,"pzError:phi",phiNbin,phiLow,phiUp,pzErrNbin,pzErrLow,pzErrUp,
	       false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,pzError,"pzError",
	       gev,cut,cutName,"pzError:eta",etaNbin,etaLow,etaUp,pzErrNbin,
	       pzErrLow,pzErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,pzError,"pzError",gev,cut,cutName,
               "pzError:pt",ptNbin,ptLow,ptUp,pzErrNbin,pzErrLow,pzErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,pzError,"pzError",gev,
               cut,cutName,"pzError:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,
               pzErrNbin,pzErrLow,pzErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,pzError,"pzError",
	       gev,cut,cutName,"pzError:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,
	       pzErrNbin,pzErrLow,pzErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,pzError,"pzError",
	       gev,cut,cutName,"pzError:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,
	       pzErrNbin,pzErrLow,pzErrUp,false,false);


    TH1TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,cut,cutName,
	       "pt",ptNbin,ptLow,ptUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,ptTruthAssoc,"ptTruthAssoc",gev,cut,cutName,
	       "ptTruthAssoc",ptTruthAssocNbin,ptTruthAssocLow,ptTruthAssocUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,ptError,"ptError",gev,cut,cutName,
    	       "ptError",ptErrNbin,ptErrLow,ptErrUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,ptResol,"pt-ptTruthAssoc",gev,cut,cutName,
    	       "ptResolution",ptResolNbin,ptResolLow,ptResolUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,ptPull,"(pt-ptTruthAssoc)/ptError",noUnit,
	       cut,cutName,"ptPull",ptPullNbin,ptPullLow,ptPullUp,false,true);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,ptErrorOverPt,"ptErrorOverPt",noUnit,cut,
	       cutName,"ptErrorOverPt",ptErrOverPtNbin,ptErrOverPtLow,ptErrOverPtUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptTruthAssoc,"ptTruthAssoc",gev,ptRec,"pt",
	       gev,cut,cutName,"pt:ptTruthAssoc",ptNbin,ptLow,ptUp,ptTruthAssocNbin,ptLow,
	       ptUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,ptError,"ptError",gev,cut,
	       cutName,"ptError:phi",phiNbin,phiLow,phiUp,ptErrNbin,ptErrLow,ptErrUp,
	       false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,ptError,"ptError",
	       gev,cut,cutName,"ptError:eta",etaNbin,etaLow,etaUp,ptErrNbin,
	       ptErrLow,ptErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,ptError,"ptError",gev,cut,cutName,
               "ptError:pt",ptNbin,ptLow,ptUp,ptErrNbin,ptErrLow,ptErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,ptError,"ptError",gev,
               cut,cutName,"ptError:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,
               ptErrNbin,ptErrLow,ptErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,ptError,"ptError",
	       gev,cut,cutName,"ptError:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,
	       ptErrNbin,ptErrLow,ptErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,ptError,"ptError",
	       gev,cut,cutName,"ptError:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,
	       ptErrNbin,ptErrLow,ptErrUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,ptErrorOverPt,"ptErrorOverPt",noUnit,
	       cut,cutName,"ptErrorOverPt:phi",phiNbin,phiLow,phiUp,ptErrOverPtNbin,ptErrOverPtLow,
	       ptErrOverPtUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,ptErrorOverPt,"ptErrorOverPt",
	       noUnit,cut,cutName,"ptErrorOverPt:eta",etaNbin,etaLow,etaUp,ptErrOverPtNbin,
	       ptErrOverPtLow,ptErrOverPtUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,ptRec,"pt",gev,ptErrorOverPt,"ptErrorOverPt",noUnit,
	       cut,cutName,"ptErrorOverPt:pt",ptNbin,ptLow,ptUp,ptErrOverPtNbin,ptErrOverPtLow,
	       ptErrOverPtUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,ptErrorOverPt,"ptErrorOverPt",
	       noUnit,cut,cutName,"ptErrorOverPt:numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,
               ptErrOverPtNbin,ptErrOverPtLow,ptErrOverPtUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,ptErrorOverPt,"ptErrorOverPt",
	       noUnit,cut,cutName,"ptErrorOverPt:numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,
	       ptErrOverPtNbin,ptErrOverPtLow,ptErrOverPtUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,ptErrorOverPt,"ptErrorOverPt",
	       noUnit,cut,cutName,"ptErrorOverPt:numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,
	       ptErrOverPtNbin,ptErrOverPtLow,ptErrOverPtUp,false,false);
  }
  



  if (doTrkChiSquarePerigee) {

    TH1TrkProp(format,rootfile,dir,ntupleFitter,trkChi2,"chi2",noUnit,cut,cutName,"trkChi2",
	       trkChi2Nbin,trkChi2Low,trkChi2Up,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,trkNdf,"Ndf",noUnit,cut,cutName,"trkNdf",
	       trkNdfNbin,trkNdfLow,trkNdfUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,trkChi2DoF,"TrkChi2/Ndf",noUnit,cut,cutName,
	       "trkChi2overNdf",trkChi2DoFNbin,trkChi2DoFLow,trkChi2DoFUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,trkChi2Prob,"Chi2Prob",noUnit,cut,cutName,
	       "trkChi2Prob",trkChi2ProbNbin,trkChi2ProbLow,trkChi2ProbUp,false,false);
  }




  if (doNumberOfHits) {

    // Pixel
    TH1TrkProp(format,rootfile,dir,ntupleFitter,pixelHit,"No. hits",noUnit,cut,cutName,
	       "numberPixelHits",pixelHitNbin,pixelHitLow,pixelHitUp,true,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,pixelHit,"No. hits",
	       noUnit,cut,cutName,"numberPixelHits:phi",phiNbin,phiLow,phiUp,
	       pixelHitNbin,pixelHitLow,pixelHitUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,pixelHit,"No. hits",
	       noUnit,cut,cutName,"numberPixelHits:eta",etaNbin,etaLow,etaUp,
	       pixelHitNbin,pixelHitLow,pixelHitUp,false,false);

    
    // Sct
    TH1TrkProp(format,rootfile,dir,ntupleFitter,sctHit,"No. hits",noUnit,cut,cutName,
	       "numberSCTHits",sctHitNbin,sctHitLow,sctHitUp,true,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,sctHit,"No. hits",
	       noUnit,cut,cutName,"numberSCTHits:phi",phiNbin,phiLow,phiUp,
	       sctHitNbin,sctHitLow,sctHitUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,sctHit,"No. hits",
	       noUnit,cut,cutName,"numberSCTHits:eta",etaNbin,etaLow,etaUp,
	       sctHitNbin,sctHitLow,sctHitUp,false,false);
  
    // Trt
    TH1TrkProp(format,rootfile,dir,ntupleFitter,trtHit,"No. hits",noUnit,cut,cutName,
	       "numberTRTHits",trtHitNbin,trtHitLow,trtHitUp,true,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,phiRec,"phi",rad,trtHit,"No. hits",
	       noUnit,cut,cutName,"numberTRTHits:phi",phiNbin,phiLow,phiUp,
	       trtHitNbin,trtHitLow,trtHitUp,false,false);
    TH2TrkProp(format,rootfile,dir,ntupleFitter,etaRec,"eta",noUnit,trtHit,"No. hits",
	       noUnit,cut,cutName,"numberTRTHits:eta",etaNbin,etaLow,etaUp,
	       trtHitNbin,trtHitLow,trtHitUp,false,false);
  }



  if (doXVertex) {
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_vtx_x","x",mm,cut,cutName,"xPrmVtx",
	       xVtxNbin,xVtxLow,xVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_vtx_x_truth","x",mm,cut,cutName,"xPrimVtxTruth",
	       xVtxNbin,xVtxLow,xVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"RVGen*cos(PhiVGen)","x",mm,cut,cutName,
	       "xOriginVtxTruth",xVtxNbin,xVtxLow,xVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"RV*cos(PhiV)","x",mm,cut,cutName,"xVtxTruth",
	       xVtxNbin,xVtxLow,xVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_sigvtx_x","xError",mm,cut,cutName,"xPrmVtxError",
	       100,0.007,0.016,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"(vxp_vtx_x-vxp_vtx_x_truth)","x-xTruth",mm,cut,cutName,
	       "xPrmVtxRes",100,-0.1,0.1,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"(vxp_vtx_x-vxp_vtx_x_truth)/vxp_sigvtx_x",
	       "(x-xTruth)/xError",mm,cut,cutName,"xPrmVtxPull",100,-5.0,5.0,
	       false,true);
  }



  if (doYVertex) {
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_vtx_y","y",mm,cut,cutName,"yPrmVtx",
	       yVtxNbin,yVtxLow,yVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_vtx_y_truth","y",mm,cut,cutName,"yPrimVtxTruth",
	       yVtxNbin,yVtxLow,yVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"RVGen*sin(PhiVGen)","y",mm,cut,cutName,
	       "yOriginVtxTruth",yVtxNbin,yVtxLow,yVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"RV*sin(PhiV)","y",mm,cut,cutName,"yVtxTruth",
	       yVtxNbin,yVtxLow,yVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_sigvtx_y","yError",mm,cut,cutName,"yPrmVtxError",
	       100,0.007,0.016,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"(vxp_vtx_y-vxp_vtx_y_truth)","y-yTruth",mm,cut,cutName,
	       "yPrmVtxRes",100,-0.1,0.1,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"(vxp_vtx_y-vxp_vtx_y_truth)/vxp_sigvtx_y",
	       "(y-yTruth)/yError",mm,cut,cutName,"yPrmVtxPull",100,-5.0,5.0,
	       false,true);
  }
  


  if (doZVertex) {
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_vtx_z","z",mm,cut,cutName,"zPrmVtx",
	       zVtxNbin,zVtxLow,zVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_vtx_z_truth","z",mm,cut,cutName,"zPrimVtxTruth",
	       zVtxNbin,zVtxLow,zVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"ZVGen","z",mm,cut,cutName,"zOriginVtxTruth",
	       zVtxNbin,zVtxLow,zVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"ZV","z",mm,cut,cutName,"zVtxTruth",
	       zVtxNbin,zVtxLow,zVtxUp,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"vxp_sigvtx_z","zError",mm,cut,cutName,"zPrmVtxError",
	       100,0.08,0.13,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"(vxp_vtx_z-vxp_vtx_z_truth)","z-zTruth",mm,cut,cutName,
	       "zPrmVtxRes",100,-1.0,1.0,false,false);
    TH1TrkProp(format,rootfile,dir,ntupleFitter,"(vxp_vtx_z-vxp_vtx_z_truth)/vxp_sigvtx_z",
	       "(z-zTruth)/zError",mm,cut,cutName,"zPrmVtxPull",100,-5.0,5.0,
	       false,true);
  }


 
}
