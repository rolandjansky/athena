/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=8 sw=2
/*
  Missing Mass Calculator
*/
//
// to be done : tau 4-vect and type should be data member of MMC.
//
// walk strategy for alg 3. Markov is now the default
#define WALKSTRATEGY WalkStrategy::MARKOVCHAIN // new recommended
//#define WALKSTRATEGY WalkStrategy::RANDOM  // old
//#define WALKSTRATEGY WalkStrategy::GRID  // original


// The directives below allow to alter algorithm.
// By default they should ALL be commented out.
// Change for experts only.
//disable inlining (to get better callgraph from callgrind), but this slow down the code a bit
//#define inline
//
//The following directive allows to enable various debugging.
// To be used with care and only on a few events.
// if dump iteration
//#define DUMPITERATION
//if dump specific iterations
//#define DEBUGTHISITERATION 618
//if make histogram of input parameters
//#define HISTITERATION
//if fill ntuple wit the vector of points
//#define FILLPOINTSOUTPUT
//if fill the vector point only with solutions
//#define FILLPOINTSOUTPUTONLYSOL
//if make histogram of separate probabilities
//#define HISTPROBA
// if keep mass histogram (they are defined in all cases)
//#define HISTMASS
// if debug nupsolutionv3
//#define DEBUG_NUPSOLV3
//
// these directives allow to test algorithms modifications
// In most case either obsolete or untested. Only for experts.
//
// if tau energy scanning
//#define TAUESCAN
//if histogram smoothing
//#define SMOOTH
// if do not use fast sin/cos
//#define NOFASTSINCOS
// if do not reorder input tau
//#define DONOTREORDERTAU
// if do not cut corner for MET (default is disabled)
//#define METNOTCUTCORNER
// if rerun after fixing met to best value from MMC (does not help at all)
//#define RERUNWITHBESTMET
// if use of real solution or approx solution -b/2a (worst result)
//#define USEAPPROXSOLUTION do not use
// use tau visible momentum instead of total tau momentum (not debugged)
//#define USETAUVISFORPROB do not use
// use tau prob from matrix element Weizman/Freiburg (not debugged)
// use maxdelphi (recommended for random, otherwise phi angle scanning too wide)
//#define USEMAXDELPHI
//#define TAUPROBMATRIX do not use
// if use all solution for each point (otherwise pick one randomly)
//#define USEALLSOLUTION
// hand written maximum fitting instead of root fitting. Does not gain
// anything, only useful to remove root Fit overhead when running valgrind
//#define MAXFITTINGBYHAND
// burn in the markov chain
// #define BURNIN

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include "DiTauMassTools/MissingMassCalculator.h" // this is for RootCore package
// #include "MissingMassCalculator.h" // this is for standalone package

#include <TRandom.h>
#include <TObject.h>
//SpeedUp committed from revision 163876
#include <TStopwatch.h>
#include <TFitResult.h>
#include <TF1.h>
#include <TFitResultPtr.h>
#include <TMatrixDSym.h>
#include <TVectorD.h>
//#if !defined(__CINT__) | defined(__MAKECINT__)
//// Define the class for the cint dictionary
//ClassImp(MissingMassCalculator)
//#endif

const double pi=TMath::Pi();
const double twopi=2*pi;

// dTheta probability density function for hadronic taus
double MissingMassCalculator::myDelThetaHadFunc(double *x, double *par)
{
  double fitval=1.0E-10;
  if(x[0]>TMath::Pi() || x[0]<0.0) return fitval;
  const double arg=x[0];
  const double arg_L=arg;
  const double mean=par[1];
  const double sigmaG=par[2];
  const double mpv=par[3];
  const double sigmaL=par[4];

  if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2011){
    double normL=par[5];
    if(normL<0.0) normL=0.0;
    const double g1=normL*TMath::Gaus(arg,mean,sigmaG);
    const double g2=TMath::Landau(arg_L,mpv,sigmaL);
    fitval=par[0]*(g1+g2)/(1.0+normL);
  } else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2012
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C
             || m_mmcCalibrationSet==MMCCalibrationSet::UPGRADE
             || m_mmcCalibrationSet==MMCCalibrationSet::LFVMMC2012){
    const double norm=sqrt(2.0*TMath::Pi());
    const double g1=TMath::Gaus(arg,mean,sigmaG)/norm;
    const double g2=TMath::Landau(arg_L,mpv,sigmaL)/norm;
    fitval=par[0]*g1/sigmaG+par[5]*g2/sigmaL;
  }

  if(fitval<0.0) return 0.0;
  return fitval;
}

// dTheta probability density function for leptonic taus
double MissingMassCalculator::myDelThetaLepFunc(double *x, double *par)
{
  double fitval=1.0E-10;
  if(x[0]>TMath::Pi() || x[0]<0.0) return fitval;
  double arg=x[0]/par[1];

  double normL=par[5];
  if(normL<0.0) normL=0.0;

  if(arg<1) arg=sqrt(std::abs(arg));
  else arg=arg*arg;
  const double arg_L=x[0];
  const double mean=1.0;
  const double sigmaG=par[2];
  const double mpv=par[3];
  const double sigmaL=par[4];
  const double g1=normL*TMath::Gaus(arg,mean,sigmaG);
  const double g2=TMath::Landau(arg_L,mpv,sigmaL);
  fitval=par[0]*(g1+g2)/(1.0+normL);
  if(fitval<0.0) return 0.0;
  return fitval;
}



//ClassImp(MissingMassCalculator)
//______________________________constructor________________________________
MissingMassCalculator::MissingMassCalculator(){


  m_mmcCalibrationSet=MMCCalibrationSet::MAXMMCCALIBRATIONSET;  // initial value invalid, to force setting
  m_apoiIndex=0;
  m_fUseVerbose=0;
  m_fSpeedStudy=false;
  m_AlgorithmVersion=3; // use V9Walk by default
  m_beamEnergy=6500.0; // for now LHC default is sqrt(S)=7 TeV
  m_niter_fit1=20;
  m_niter_fit2=30;
  m_niter_fit3=10;
  m_NsucStop=-1;
  m_NiterRandom=-1; // if the user does not set it to positive value,will be set in SpaceWalkerInit
  m_niterRandomLocal=-1; // niterandom which is really used
  //to be used with RMSSTOP  NiterRandom=10000000; // number of random iterations for lh. Multiplied by 10 for ll, divided by 10 for hh (to be optimised)
  //  RMSStop=200;// Stop criteria depending of rms of histogram
  m_reRunWithBestMET=false;
  m_RMSStop=-1;// disable

  m_RndmSeedAltering=0; // can be changed to re-compute with different random seed
  m_dRmax_tau=0.4; // changed from 0.2
  m_SearchMode=0;
  m_nsigma_METscan=-1; // number of sigmas for MET scan
  m_nsigma_METscan_ll=3.0; // number of sigmas for MET scan
  m_nsigma_METscan_lh=3.0; // number of sigmas for MET scan
  m_nsigma_METscan_hh=4.0; // number of sigmas for MET scan (4 for hh 2013)

  m_meanbinStop=-1; // meanbin stopping criterion (-1 if not used)
  m_proposalTryMEt=-1; // loop on METproposal disable // FIXME should be cleaner
  m_ProposalTryPhi=-1; // loop on Phiproposal disable
  m_ProposalTryMnu=-1; // loop on MNuProposal disable
  m_ProposalTryEtau=-1; // loop on ETauProposal disable

  m_dTheta3d_binMin=0.0025;
  m_dTheta3d_binMax=0.02;
  m_fJERsyst=0; // no JER systematics by default (+/-1: up/down 1 sigma)
  m_METresSyst=0; // no MET resolution systematics by default (+/-1: up/down 1 sigma)
  m_fApplyMassScale=0; // don't apply mass scale correction by default
  m_rawInput.dataType=1; // set to "data" by default
  m_fUseTailCleanup=1; // cleanup by default for lep-had Moriond 2012 analysis
  m_fUseTauProbability=1; // TauProbability is ON by default DRMERGE comment out for now
  m_fUseMnuProbability=0; // MnuProbability is OFF by default
  m_fUseDefaults=0; // use pre-set defaults for various configurations; if set it to 0 if need to study various options
  m_fUseEfficiencyRecovery=0; // no re-fit by default

  m_METScanScheme=1; // MET-scan scheme: 0- use JER; 1- use simple sumEt & missingHt for Njet=0 events in (lep-had winter 2012)
  //  MnuScanRange=1.777; // range of M(nunu) scan
  m_MnuScanRange=1.5; // better value (sacha)
  m_LFVmode=0; // by default consider case of H->mu+tau(->ele)
  ClearInputStuff();

  // added by Tomas Davidek for lep-lep:
  m_fUseDphiLL = false;

  m_randomGen = new TRandom2();

  //walkStrategy=WalkStrategy::RANDOMNONUNIF;  // experimental, do not use

  m_nCallprobCalculatorV9fast=0;
  m_iterTheta3d=0;
  m_debugThisIteration=false;


  m_nsolmax=4;
  m_nsolfinalmax=m_nsolmax*m_nsolmax;

  m_nuvecsol1.resize(m_nsolmax);
  m_nuvecsol2.resize(m_nsolmax);
  m_tauvecsol1.resize(m_nsolmax);
  m_tauvecsol2.resize(m_nsolmax);
  m_tauvecprob1.resize(m_nsolmax);
  m_tauvecprob2.resize(m_nsolmax);

  m_nsol=0;
  m_probFinalSolVec.resize(m_nsolfinalmax);
  m_mtautauFinalSolVec.resize(m_nsolfinalmax);
  m_nu1FinalSolVec.resize(m_nsolfinalmax);
  m_nu2FinalSolVec.resize(m_nsolfinalmax);


  m_nsolOld=0;
  m_probFinalSolOldVec.resize(m_nsolfinalmax);
  m_mtautauFinalSolOldVec.resize(m_nsolfinalmax);
  m_nu1FinalSolOldVec.resize(m_nsolfinalmax);
  m_nu2FinalSolOldVec.resize(m_nsolfinalmax);



  //   float hEmax=1000; // maximum energy (GeV)
  float hEmax=3000.0;
  // number of bins
  //   int hNbins=500; // original 2500 for mass, 10000 for P
  int hNbins=1500; // original 2500 for mass, 10000 for P
  // choice of hNbins also related to size of window for fitting (see maxFromHist)



  //--- define histograms for histogram method
  //--- upper limits need to be revisied in the future!!! It may be not enough for some analyses

  m_fMfit_all = std::make_shared<TH1F>("MMC_h1","M",hNbins,0.0,hEmax); // all solutions
  m_fMfit_all->Sumw2(); // allow proper error bin calculation. Slightly slower but completely negligible

  //histogram without weight. useful for debugging. negligibly slow until now
  m_fMfit_allNoWeight = std::make_shared<TH1F>("MMC_h1NoW","M no weight",hNbins,0.0,hEmax); // all solutions

  m_fPXfit1 = std::make_shared<TH1F>("MMC_h2","Px1",4*hNbins,-hEmax,hEmax); // Px for tau1
  m_fPYfit1 = std::make_shared<TH1F>("MMC_h3","Py1",4*hNbins,-hEmax,hEmax); // Py for tau1
  m_fPZfit1 = std::make_shared<TH1F>("MMC_h4","Pz1",4*hNbins,-hEmax,hEmax); // Pz for tau1
  m_fPXfit2 = std::make_shared<TH1F>("MMC_h5","Px2",4*hNbins,-hEmax,hEmax); // Px for tau2
  m_fPYfit2 = std::make_shared<TH1F>("MMC_h6","Py2",4*hNbins,-hEmax,hEmax); // Py for tau2
  m_fPZfit2 = std::make_shared<TH1F>("MMC_h7","Pz2",4*hNbins,-hEmax,hEmax); // Pz for tau2

  m_fMfit_all->SetDirectory(0);
  m_fMfit_allNoWeight->SetDirectory(0);
  m_fPXfit1->SetDirectory(0);
  m_fPYfit1->SetDirectory(0);
  m_fPZfit1->SetDirectory(0);
  m_fPXfit2->SetDirectory(0);
  m_fPYfit2->SetDirectory(0);
  m_fPZfit2->SetDirectory(0);

  // max hist fitting function
  //   m_fFitting = new TF1("maxFitting",this,&MissingMassCalculator::maxFitting,0.,1000.,3);
  m_fFitting = new TF1("MMC_maxFitting",this,&MissingMassCalculator::maxFitting,0.,hEmax,3);
  // Sets initial parameter names
  m_fFitting->SetParNames("Max","Mean","InvWidth2");



  if(m_fUseVerbose==1)
    {
      gDirectory->pwd();
      gDirectory->ls();
    }


#ifdef HISTITERATION
  //debug histo
  m_fPhi1=new TH1F("MMC_hphi1","phi1",500,-3.2,3.2); // all solutions
  m_fPhi2=new TH1F("MMC_hphi2","phi2",500,-3.2,3.2); // all solutions
  m_fMnu1=new TH1F("MMC_hmnu1","Mnu1",500,-1.,10.); // all solutions
  m_fMnu2=new TH1F("MMC_hmnu2","Mnu2",500,-1.,10.); // all solutions
  m_fMetx=new TH1F("MMC_hmetx","METx",400,-100.,100.); // all solutions
  m_fMety=new TH1F("MMC_hmety","METy",400,-100.,100.); // all solutions
#endif
#ifdef HISTPROBA
  m_fTheta3D=new TH1F("MMC_htheta3d","lprob",500,-10.,1.); // all solutions
  m_fTauProb=new TH1F("MMC_htauprob","lprob",500,-10.,2.); // all solutions
#endif



  if(m_fUseVerbose==1)
    {
      gDirectory->pwd();
      gDirectory->ls();
    }

  /// MMC2011 parameterisation
  // [tau_type][parLG][par]
  // leptonic tau
  //-par[0]
  s_fit_param[0][0][0][0]=-9.82013E-1;
  s_fit_param[0][0][0][1]=9.09874E-1;
  s_fit_param[0][0][0][2]=0.0;
  s_fit_param[0][0][0][3]=0.0;
  //-par[1]
  s_fit_param[0][0][1][0]=9.96303E1;
  s_fit_param[0][0][1][1]=1.68873E1;
  s_fit_param[0][0][1][2]=3.23798E-2;
  s_fit_param[0][0][1][3]=0.0;
  //-par[2]
  s_fit_param[0][0][2][0]=0.0;
  s_fit_param[0][0][2][1]=0.0;
  s_fit_param[0][0][2][2]=0.0;
  s_fit_param[0][0][2][3]=0.3; // fit value is 2.8898E-1, I use 0.3
  //-par[3]
  s_fit_param[0][0][3][0]=9.70055E1;
  s_fit_param[0][0][3][1]=6.46175E1;
  s_fit_param[0][0][3][2]=3.20679E-2;
  s_fit_param[0][0][3][3]=0.0;
  //-par[4]
  s_fit_param[0][0][4][0]=1.56865;
  s_fit_param[0][0][4][1]=2.28336E-1;
  s_fit_param[0][0][4][2]=1.09396E-1;
  s_fit_param[0][0][4][3]=1.99975E-3;
  //-par[5]
  s_fit_param[0][0][5][0]=0.0;
  s_fit_param[0][0][5][1]=0.0;
  s_fit_param[0][0][5][2]=0.0;
  s_fit_param[0][0][5][3]=0.66;
  //-----------------------------------------------------------------
  // 1-prong hadronic tau
  //-par[0]
  s_fit_param[0][1][0][0]=-2.42674;
  s_fit_param[0][1][0][1]=7.69124E-1;
  s_fit_param[0][1][0][2]=0.0;
  s_fit_param[0][1][0][3]=0.0;
  //-par[1]
  s_fit_param[0][1][1][0]=9.52747E1;
  s_fit_param[0][1][1][1]=1.26319E1;
  s_fit_param[0][1][1][2]=3.09643E-2;
  s_fit_param[0][1][1][3]=0.0;
  //-par[2]
  s_fit_param[0][1][2][0]=1.71302E1;
  s_fit_param[0][1][2][1]=3.00455E1;
  s_fit_param[0][1][2][2]=7.49445E-2;
  s_fit_param[0][1][2][3]=0.0;
  //-par[3]
  s_fit_param[0][1][3][0]=1.06137E2;
  s_fit_param[0][1][3][1]=6.01548E1;
  s_fit_param[0][1][3][2]=3.50867E-2;
  s_fit_param[0][1][3][3]=0.0;
  //-par[4]
  s_fit_param[0][1][4][0]=4.26079E-1;
  s_fit_param[0][1][4][1]=1.76978E-1;
  s_fit_param[0][1][4][2]=1.43419;
  s_fit_param[0][1][4][3]=0.0;
  //-par[5]
  s_fit_param[0][1][5][0]=0.0;
  s_fit_param[0][1][5][1]=0.0;
  s_fit_param[0][1][5][2]=0.0;
  s_fit_param[0][1][5][3]=0.4;
  //-----------------------------------------------------------------
  // 3-prong hadronic tau
  //-par[0]
  s_fit_param[0][2][0][0]=-2.43533;
  s_fit_param[0][2][0][1]=6.12947E-1;
  s_fit_param[0][2][0][2]=0.0;
  s_fit_param[0][2][0][3]=0.0;
  //-par[1]
  s_fit_param[0][2][1][0]=9.54202;
  s_fit_param[0][2][1][1]=2.80011E-1;
  s_fit_param[0][2][1][2]=2.49782E-1;
  s_fit_param[0][2][1][3]=0.0;
  //-par[2]
  s_fit_param[0][2][2][0]=1.61325E1;
  s_fit_param[0][2][2][1]=1.74892E1;
  s_fit_param[0][2][2][2]=7.05797E-2;
  s_fit_param[0][2][2][3]=0.0;
  //-par[3]
  s_fit_param[0][2][3][0]=1.17093E2;
  s_fit_param[0][2][3][1]=4.70000E1;
  s_fit_param[0][2][3][2]=3.87085E-2;
  s_fit_param[0][2][3][3]=0.0;
  //-par[4]
  s_fit_param[0][2][4][0]=4.16557E-1;
  s_fit_param[0][2][4][1]=1.58902E-1;
  s_fit_param[0][2][4][2]=1.53516;
  s_fit_param[0][2][4][3]=0.0;
  //-par[5]
  s_fit_param[0][2][5][0]=0.0;
  s_fit_param[0][2][5][1]=0.0;
  s_fit_param[0][2][5][2]=0.0;
  s_fit_param[0][2][5][3]=0.95;


  /// MMC2012 parameterisation
  // [tau_type][parLG][par]
  // leptonic tau
  //-par[0]
  s_fit_param[1][0][0][0]=-9.82013E-1;
  s_fit_param[1][0][0][1]=9.09874E-1;
  s_fit_param[1][0][0][2]=0.0;
  s_fit_param[1][0][0][3]=0.0;
  s_fit_param[1][0][0][4]=0.0;
  //-par[1]
  s_fit_param[1][0][1][0]=9.96303E1;
  s_fit_param[1][0][1][1]=1.68873E1;
  s_fit_param[1][0][1][2]=3.23798E-2;
  s_fit_param[1][0][1][3]=0.0;
  s_fit_param[1][0][1][4]=0.0;
  //-par[2]
  s_fit_param[1][0][2][0]=0.0;
  s_fit_param[1][0][2][1]=0.0;
  s_fit_param[1][0][2][2]=0.0;
  s_fit_param[1][0][2][3]=0.3; // fit value is 2.8898E-1, I use 0.3
  s_fit_param[1][0][2][4]=0.0;
  //-par[3]
  s_fit_param[1][0][3][0]=9.70055E1;
  s_fit_param[1][0][3][1]=6.46175E1;
  s_fit_param[1][0][3][2]=3.20679E-2;
  s_fit_param[1][0][3][3]=0.0;
  s_fit_param[1][0][3][4]=0.0;
  //-par[4]
  s_fit_param[1][0][4][0]=1.56865;
  s_fit_param[1][0][4][1]=2.28336E-1;
  s_fit_param[1][0][4][2]=1.09396E-1;
  s_fit_param[1][0][4][3]=1.99975E-3;
  s_fit_param[1][0][4][4]=0.0;
  //-par[5]
  s_fit_param[1][0][5][0]=0.0;
  s_fit_param[1][0][5][1]=0.0;
  s_fit_param[1][0][5][2]=0.0;
  s_fit_param[1][0][5][3]=0.66;
  s_fit_param[1][0][5][4]=0.0;
  //-----------------------------------------------------------------
  //-----------------------
  // Only hadronic tau's were re-parametrized in MC12. The parameterization now
  // goes to P(tau)=1 TeV.
  //-----------------------------------------------------------------
  // 1-prong hadronic tau
  //---par[0]
  s_fit_param[1][1][0][0]=0.7568;
  s_fit_param[1][1][0][1]=-0.0001469;
  s_fit_param[1][1][0][2]=5.413E-7;
  s_fit_param[1][1][0][3]=-6.754E-10;
  s_fit_param[1][1][0][4]=2.269E-13;
  //---par[1]
  s_fit_param[1][1][1][0]=-0.0288208;
  s_fit_param[1][1][1][1]=0.134174;
  s_fit_param[1][1][1][2]=-142.588;
  s_fit_param[1][1][1][3]=-0.00035606;
  s_fit_param[1][1][1][4]=-6.94567E-20;
  //---par[2]
  s_fit_param[1][1][2][0]=-0.00468927;
  s_fit_param[1][1][2][1]=0.0378737;
  s_fit_param[1][1][2][2]=-260.284;
  s_fit_param[1][1][2][3]=0.00241158;
  s_fit_param[1][1][2][4]=-6.01766E-7;
  //---par[3]
  s_fit_param[1][1][3][0]=-0.170424;
  s_fit_param[1][1][3][1]=0.135764;
  s_fit_param[1][1][3][2]=-50.2361;
  s_fit_param[1][1][3][3]=0.00735544;
  s_fit_param[1][1][3][4]=-7.34073E-6;
  //---par[4]
  s_fit_param[1][1][4][0]=-0.0081364;
  s_fit_param[1][1][4][1]=0.0391428;
  s_fit_param[1][1][4][2]=-141.936;
  s_fit_param[1][1][4][3]=0.0035034;
  s_fit_param[1][1][4][4]=-1.21956E-6;
  //-par[5]
  s_fit_param[1][1][5][0]=0.0;
  s_fit_param[1][1][5][1]=0.0;
  s_fit_param[1][1][5][2]=0.0;
  s_fit_param[1][1][5][3]=0.624*0.00125; // multiplied by a bin size
  s_fit_param[1][1][5][4]=0.0;
  //-----------------------------------------------------------------
  // 3-prong hadronic tau
  //---par[0]
  s_fit_param[1][2][0][0]=0.7562;
  s_fit_param[1][2][0][1]=-1.168E-5;
  s_fit_param[1][2][0][2]=0.0;
  s_fit_param[1][2][0][3]=0.0;
  s_fit_param[1][2][0][4]=0.0;
  //---par[1]
  s_fit_param[1][2][1][0]=-0.0420458;
  s_fit_param[1][2][1][1]=0.15917;
  s_fit_param[1][2][1][2]=-80.3259;
  s_fit_param[1][2][1][3]=0.000125729;
  s_fit_param[1][2][1][4]=-2.43945E-18;
  //---par[2]
  s_fit_param[1][2][2][0]=-0.0216898;
  s_fit_param[1][2][2][1]=0.0243497;
  s_fit_param[1][2][2][2]=-63.8273;
  s_fit_param[1][2][2][3]=0.0148339;
  s_fit_param[1][2][2][4]=-4.45351E-6;
  //---par[3]
  s_fit_param[1][2][3][0]=-0.0879411;
  s_fit_param[1][2][3][1]=0.110092;
  s_fit_param[1][2][3][2]=-75.4901;
  s_fit_param[1][2][3][3]=0.0116915;
  s_fit_param[1][2][3][4]=-1E-5;
  //---par[4]
  s_fit_param[1][2][4][0]=-0.0118324;
  s_fit_param[1][2][4][1]=0.0280538;
  s_fit_param[1][2][4][2]=-85.127;
  s_fit_param[1][2][4][3]=0.00724948;
  s_fit_param[1][2][4][4]=-2.38792E-6;
  //-par[5]
  s_fit_param[1][2][5][0]=0.0;
  s_fit_param[1][2][5][1]=0.0;
  s_fit_param[1][2][5][2]=0.0;
  s_fit_param[1][2][5][3]=0.6167*0.00125; // multiplied by a bin size
  s_fit_param[1][2][5][4]=0.0;

  // TER parameterization, for now based on p1130, to be checked and updated with new tag
  // [tau_type][eta_bin][parameter]
  // for 1-prongs
  s_ter_sigma_par[0][0][0]=0.311717;
  s_ter_sigma_par[0][0][1]=0.0221615;
  s_ter_sigma_par[0][0][2]=0.859698;
  s_ter_sigma_par[0][1][0]=0.290019;
  s_ter_sigma_par[0][1][1]=0.0225794;
  s_ter_sigma_par[0][1][2]=0.883407;
  s_ter_sigma_par[0][2][0]=0.352312;
  s_ter_sigma_par[0][2][1]=0.0196381;
  s_ter_sigma_par[0][2][2]=0.629708;
  s_ter_sigma_par[0][3][0]=0.342059;
  s_ter_sigma_par[0][3][1]=0.0275107;
  s_ter_sigma_par[0][3][2]=0.48065;
  s_ter_sigma_par[0][4][0]=0.481564;
  s_ter_sigma_par[0][4][1]=0.0197219;
  s_ter_sigma_par[0][4][2]=0.0571714;
  s_ter_sigma_par[0][5][0]=0.41264;
  s_ter_sigma_par[0][5][1]=0.0233964;
  s_ter_sigma_par[0][5][2]=0.515674;
  s_ter_sigma_par[0][6][0]=0.20112;
  s_ter_sigma_par[0][6][1]=0.0339914;
  s_ter_sigma_par[0][6][2]=0.944524;
  s_ter_sigma_par[0][7][0]=0.0892094;
  s_ter_sigma_par[0][7][1]=0.0210225;
  s_ter_sigma_par[0][7][2]=1.34014;
  s_ter_sigma_par[0][8][0]=0.175554;
  s_ter_sigma_par[0][8][1]=0.0210968;
  s_ter_sigma_par[0][8][2]=0.813925;
  s_ter_sigma_par[0][9][0]=0.0;
  s_ter_sigma_par[0][9][1]=0.0340279;
  s_ter_sigma_par[0][9][2]=1.30856;
  // for 3-prongs
  s_ter_sigma_par[1][0][0]=0.303356;
  s_ter_sigma_par[1][0][1]=0.0299807;
  s_ter_sigma_par[1][0][2]=1.25388;
  s_ter_sigma_par[1][1][0]=0.358106;
  s_ter_sigma_par[1][1][1]=0.0229604;
  s_ter_sigma_par[1][1][2]=1.02222;
  s_ter_sigma_par[1][2][0]=0.328643;
  s_ter_sigma_par[1][2][1]=0.025684;
  s_ter_sigma_par[1][2][2]=1.02594;
  s_ter_sigma_par[1][3][0]=0.497332;
  s_ter_sigma_par[1][3][1]=0.0215113;
  s_ter_sigma_par[1][3][2]=0.30055;
  s_ter_sigma_par[1][4][0]=0.4493;
  s_ter_sigma_par[1][4][1]=0.0280311;
  s_ter_sigma_par[1][4][2]=0.285793;
  s_ter_sigma_par[1][5][0]=0.427811;
  s_ter_sigma_par[1][5][1]=0.0316536;
  s_ter_sigma_par[1][5][2]=0.457286;
  s_ter_sigma_par[1][6][0]=0.165288;
  s_ter_sigma_par[1][6][1]=0.0376361;
  s_ter_sigma_par[1][6][2]=1.3913;
  s_ter_sigma_par[1][7][0]=0.289798;
  s_ter_sigma_par[1][7][1]=0.0140801;
  s_ter_sigma_par[1][7][2]=0.83603;
  s_ter_sigma_par[1][8][0]=0.186823;
  s_ter_sigma_par[1][8][1]=0.0213053;
  s_ter_sigma_par[1][8][2]=0.968934;
  s_ter_sigma_par[1][9][0]=0.301673;
  s_ter_sigma_par[1][9][1]=0.0145606;
  s_ter_sigma_par[1][9][2]=0.514022;

}

MissingMassCalculator::~MissingMassCalculator(){

  //  if(fUseVerbose==1)
  //  {
  std::cout << " in MMC destructor " << std::endl;
  //      gDirectory->pwd();
  // gDirectory->ls();
  //  }

#ifdef HISTITERATION
  delete m_fPhi1;
  delete m_fPhi2;
  delete m_fMnu1;
  delete m_fMnu2;
  delete m_fMetx;
  delete m_fMety;
#endif
#ifdef HISTPROBA
  delete m_fTheta3D;
  delete m_fTauProb;
#endif

}

//_____________________________________________________________________________
// Main Method to run MissingMassCalculator
int MissingMassCalculator::RunMissingMassCalculator() {
  m_reRunWithBestMET=false;
  ClearOutputStuff();
  //  InputInfoStuff preparedInput;
  if(m_fUseVerbose==1) {
    std::cout<<"------------- Raw Input for MissingMassCalculator --------------"<<std::endl;
    PrintInputInfo(m_rawInput) ;
  }
  FinalizeInputStuff();//rawInput, preparedInput );
  if(m_fUseVerbose==1) {
    std::cout<<"------------- Prepared Input for MissingMassCalculator --------------"<<std::endl;
    PrintInputInfo(m_preparedInput) ;
  }

  ClearInputStuff(); // DR just to be sure InputInfo is never used downstream, only preparedInput

  if(m_SearchMode==0)
    {
      if(m_AlgorithmVersion==3) {
        // remove argument DiTauMassCalculatorV9Walk work directly on preparedInput
        m_OutputInfo.FitStatus=DitauMassCalculatorV9walk();

        // re-running MMC for on failed events
        if(m_fUseEfficiencyRecovery==1 && m_OutputInfo.FitStatus!=1)
          {
            // most events where MMC failed happened to have dPhi>2.9. Run re-fit only on these events
            if(m_preparedInput.m_DelPhiTT>2.9)
              {
                //              preparedInput.MetVec.Set(-(preparedInput.vistau1+preparedInput.vistau2).Px(),-(preparedInput.vistau1+preparedInput.vistau2).Py()); // replace MET by MPT

                TVector2 dummy_met(-(m_preparedInput.vistau1+m_preparedInput.vistau2).Px(),-(m_preparedInput.vistau1+m_preparedInput.vistau2).Py());
                m_preparedInput.METcovphi=dummy_met.Phi();
                double dummy_METres=sqrt(pow(m_preparedInput.METsigmaL,2) + pow(m_preparedInput.METsigmaP,2));
                m_preparedInput.METsigmaL=dummy_METres*std::abs(cos(dummy_met.Phi()-m_preparedInput.MetVec.Phi()));
                m_preparedInput.METsigmaP=dummy_METres*std::abs(sin(dummy_met.Phi()-m_preparedInput.MetVec.Phi()));
                if(m_preparedInput.METsigmaP<5.0) m_preparedInput.METsigmaP=5.0;
                m_nsigma_METscan_lh=6.0; // increase range of MET scan
                m_nsigma_METscan_hh=6.0; // increase range of MET scan

                ClearOutputStuff(); // clear output stuff before re-running
                m_OutputInfo.FitStatus=DitauMassCalculatorV9walk(); // run MMC again
              }
          }

#ifdef RERUNWITHBESTMET
        if (m_OutputInfo.FitStatus==1) {
          std::cout << "ERROR NON FUNCTIONAL ! TO BE REIMPLEMENTED " << std::endl;
          throw;
          // experimental re-iterate after fixing MET to be the neutrino momentum from the previous iteration
          // best met from method 0
          //const TLorentzVector & p4nu0=fDitauStuffFit.nutau1;
          //const TLorentzVector & p4nu1=fDitauStuffFit.nutau2;
          // best met from method 2
          const TLorentzVector & p4nu0=m_fDitauStuffHisto.nutau1;
          const TLorentzVector & p4nu1=m_fDitauStuffHisto.nutau2;
          TVector2 newMetVec(p4nu0.Px()+p4nu1.Px(),p4nu0.Py()+p4nu1.Py());
          //std::cout << "DRDR old MET " << InputInfo.MetVec.Px() << " " << InputInfo.MetVec.Py() << std::endl;
          //std::cout << "DRDR new MET " << newMetVec.Px() << " " << newMetVec.Py() << std::endl;
          // set sigma met to very small number (avoid zero which could cause instabilities)
          const double Nsigma_METscanSave=m_nsigma_METscan;
          m_nsigma_METscan=0.000001;
          m_reRunWithBestMET=true;
          //OutputInfo.FitStatus=DitauMassCalculatorV9walk(preparedInput.vistau1,preparedInput.type_visTau1,
          //preparedInput.vistau2,preparedInput.type_visTau2,
          //                                                             newMetVec);
          m_reRunWithBestMET=false;
          const TLorentzVector& p4renu0 = m_fDitauStuffFit.nutau1;
          const TLorentzVector& p4renu1 = m_fDitauStuffFit.nutau2;

          TVector2 reMetVec(p4renu0.Px()+p4renu1.Px(),p4renu0.Py()+p4renu1.Py());
          if ((newMetVec-reMetVec).Mod()>0.1)
            {
              std::cout << "ERROR new MET after reiteration is not equal to old MET !!! " << std::endl;
            }
          // reset sigma met to original number
          m_nsigma_METscan=Nsigma_METscanSave;
        }
#endif
      }

      // running MMC in LFV mode for reconstructing mass of X->lep+tau
      else if(m_AlgorithmVersion==4) {
        if(m_fUseVerbose==1) { std::cout << "Calling DitauMassCalculatorV9lfv" << std::endl; }
        m_OutputInfo.FitStatus = DitauMassCalculatorV9lfv();
      }
    }
  DoOutputInfo();
  PrintResults();
  ClearInputStuff();
  return 1;
}

// ---- input Met vector
void MissingMassCalculator::SetMetVec(const TVector2 & vec) {
  m_rawInput.MetVec = vec;
  return;
}
// ----- input vis Tau vectors
void MissingMassCalculator::SetVisTauVec(int i, const TLorentzVector & vec) {
  if(m_fUseVerbose==1) { std::cout << "Seting input " << i << " to pT=" << vec.Pt() << std::endl; }
  if(i==0) m_rawInput.vistau1 = vec;
  if(i==1) m_rawInput.vistau2 = vec;
  return;
}
// ----- input vis Tau type
void MissingMassCalculator::SetVisTauType(int i, int tautype) {
  if(i==0) m_rawInput.type_visTau1 = tautype/10;
  if(i==1) m_rawInput.type_visTau2 = tautype/10;
  return;
}
// ----- input vis Tau N-prong
void MissingMassCalculator::SetNprong(int i, int nprong) {
  if(i==0) m_rawInput.Nprong_tau1 = nprong;
  if(i==1) m_rawInput.Nprong_tau2 = nprong;
  return;
}
// ----- input SumEt
void MissingMassCalculator::SetSumEt(double sumEt) {
  m_rawInput.m_SumEt = sumEt;
  return;
}
// ----- input data type
void MissingMassCalculator::SetIsData(int val) {
  if(val==0 || val==1) m_rawInput.dataType = val;
  return;
}
// ---- input number of jets with Et>25 GeV
void MissingMassCalculator::SetNjet25(int val) {
  if(val>-1) m_rawInput.m_Njet25 = val;
  return;
}

// ----- input Met Scan parameters
void MissingMassCalculator::SetMetScanParams(double phi, double sigmaP, double sigmaL) {
  m_rawInput.METcovphi = phi;
  m_rawInput.METsigmaP = sigmaP;
  m_rawInput.METsigmaL = sigmaL;
  return;
}

// ----- input Met covariance parameters
void MissingMassCalculator::SetMetCovariance(double varX, double varY, double varXY) { // MET covariance matrix
  // MET covariance is obtained from summing on MET elements, each one with ET uncertainty sigma_i and direction phi_i
  //varX=Sigma_i  sigma_i^2 cos ^2 phi_i
  //varY=Sigma_i  sigma_i^2 sin ^2 phi_i
  //varXY=Sigma_i  sigma_i^2 cos phi_i cos phi_i

  if (varX<1E-6  || varY<1E-6  )  {
    std::cout << " SetMetCovariance. Negative or null diagonal element. FATAL ERROR pathological error matrix " << std::endl;
    std::cout << " varX=" << varX << " varY=" << varY <<" varXY=" << varXY << std::endl;
    throw; // fatal
  }

  const double det = varX*varY - std::pow(varXY,2); // matrix determinant
  if (det<1E-6)  {
    std::cout << " SetMetCovariance. matrix determinant is negative or null. " << det << " FATAL ERROR pathological error matrix " << std::endl;
    std::cout << " varX=" << varX << " varY=" << varY <<" varXY=" << varXY << std::endl;
    throw; // fatal
  }

  const double trace = varX + varY; // matrix trace
  double discri = std::pow(trace,2) - 4*det;
  double sqdiscri;
  if (discri<-1e-6) {
    std::cout << " SetMetCovariance. Negative discriminant. " << discri << " FATAL ERROR pathological error matrix " << std::endl;
    std::cout << " varX=" << varX << " varY=" << varY <<" varXY=" << varXY << std::endl;
    throw; // fatal
  } else    if (discri<0) { // some margin for rounding errors
    sqdiscri=0.;
  } else {
    sqdiscri=sqrt(discri);
  }

  const double varL = 1/2. *(trace + sqdiscri); //larger eigenvalue
  const double varP = 1/2. *(trace - sqdiscri); //smaller eigenvalue

  const double sigmaL = sqrt(varL); // ellipse major axis
  const double sigmaP = sqrt(varP); // ellipse minor axis
  double phi;
  if (std::abs(varXY) < 1E-6) {
    phi=0.; // isotropic, undefined angle
  } else {
    phi = atan((varL - varX)/varXY); // angle ellipse major axis wrt to x axis
  }

  //  std::cout << "SetMetCovariance phi=" << phi << " sigmaP=" << sigmaP << " sigmaL=" << sigmaL << std::endl;
  SetMetScanParams(phi, sigmaP, sigmaL);

  return;
}


// input is sumEt after electrons and taus have been removed
// data_code=0 for data and =1 for MC
void MissingMassCalculator::SetMetScanParamsUE(double sumEt, double phi_scan, int data_code) {
  m_rawInput.METcovphi = phi_scan;
  if(sumEt>2.0*m_beamEnergy) sumEt = sumEt/1000.0; // it's likely that sumEt was entered in MeV; this fix won't work only for a very small fraction of events
  double sigma = 1.0;
  double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
  double METresScale = 0.7; // using inclusive number for winter 2012
  if(data_code==1) METresScale=0.7; // use the same for data & MC
  METresScale = METresScale*(1.0+m_METresSyst*sigmaSyst);
  // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt = 5.0 for now
  sigma = sumEt>pow(3.0/METresScale,2) ? METresScale*sqrt(sumEt) : 3.0; // assume that MET resolution can't be better than 3 GeV
  m_rawInput.METsigmaP = sigma;
  m_rawInput.METsigmaL = sigma;
  m_rawInput.m_SumEt = sumEt;
  m_rawInput.dataType = data_code; // Sasha added on 09/26/11
  return;
}

// input jet vectors and for MET resolution due to jets
// only consider jets with Et>20 GeV
void MissingMassCalculator::SetMetScanParamsJets(std::vector<TLorentzVector> jets) {
  for(unsigned int i=0; i<jets.size(); i++)
    {
      if(jets[i].Pt()>20.0) m_rawInput.m_jet4vecs.push_back(jets[i]);
    }
  // re-order jets
  if(m_rawInput.m_jet4vecs.size()>1)
    {
      TLorentzVector jet1(0.0,0.0,0.0,0.0);
      for(unsigned int i=1; i<m_rawInput.m_jet4vecs.size(); i++)
        {
          if(m_rawInput.m_jet4vecs[i].Pt()>m_rawInput.m_jet4vecs[i-1].Pt())
            {
              jet1 = m_rawInput.m_jet4vecs[i-1];
              m_rawInput.m_jet4vecs[i-1] = m_rawInput.m_jet4vecs[i];
              m_rawInput.m_jet4vecs[i] = jet1;
            }
        }
    }
  return;
}

//-------- clearing ditau container
void MissingMassCalculator::ClearDitauStuff(DitauStuff &fStuff) {
  fStuff.Mditau_best = 0.0;
  fStuff.Sign_best = 1.0E6;
  fStuff.nutau1.SetPxPyPzE(0.0,0.0,0.0,0.0);
  fStuff.nutau2.SetPxPyPzE(0.0,0.0,0.0,0.0);
  fStuff.vistau1.SetPxPyPzE(0.0,0.0,0.0,0.0);
  fStuff.vistau2.SetPxPyPzE(0.0,0.0,0.0,0.0);
  fStuff.taufit1.SetPxPyPzE(0.0,0.0,0.0,0.0);
  fStuff.taufit2.SetPxPyPzE(0.0,0.0,0.0,0.0);
  fStuff.RMSoverMPV = 0.0;

  return;
}

//------- clearing input stuff
void MissingMassCalculator::ClearInputStuff() {
  m_rawInput.MetVec.Set(0.0,0.0);
  m_rawInput.detMetVec.Set(0.0,0.0);
  m_rawInput.vistau1.SetPxPyPzE(0.0,0.0,0.0,0.0);
  m_rawInput.vistau2.SetPxPyPzE(0.0,0.0,0.0,0.0);
  m_rawInput.type_visTau1 = -1;
  m_rawInput.type_visTau2 = -1;
  m_rawInput.Nprong_tau1 = -1;
  m_rawInput.Nprong_tau2 = -1;
  //   m_rawInput.dataType = -1;
  m_rawInput.METcovphi = 0.0;
  m_rawInput.METsigmaP = 0.0;
  m_rawInput.METsigmaL = 0.0;
  m_rawInput.m_SumEt = 0.0;
  m_rawInput.sigmaEtau1 = -1.;
  m_rawInput.sigmaEtau2 = -1.;
  m_rawInput.m_jet4vecs.clear();
  //DR default at 1
  m_rawInput.m_Njet25 = 1;
  m_rawInput.allowUseHT = true;
  m_rawInput.m_DelPhiTT = 0.0;
  m_rawInput.UseHT = false;
  m_rawInput.m_MHtSigma1 = -1.0;
  m_rawInput.m_MHtSigma2 = -1.0;
  m_rawInput.MHtGaussFr = -1.0;
  m_rawInput.HtOffset = -1.0;

  return;
}

// checks units of input variables, converts into [GeV] if needed, make all possible corrections
// DR new : now a second structure preparedInput is derived from the input one which only has direct user input
void MissingMassCalculator::FinalizeInputStuff()//const InputInfoStuff &rawInput, InputInfoStuff &preparedInput)
{

  // check that the calibration set has been chosen explicitly, otherwise abort
  if (m_mmcCalibrationSet==MMCCalibrationSet::MAXMMCCALIBRATIONSET) {
    std::cout << " ERROR MMCCalibrationSet has not been set !. Please use fMMC.SetCalibrationSet(MMCCalibrationSet::MMC2011) or MMC2012. Abort now. " << std::endl;
    throw; // stop job
  }
  //----------- Re-ordering input info, to make sure there is no dependence of results on input order
  // this might be needed because a random scan is used
  // highest pT tau is always first
  m_InputReorder=0; // set flag to 0 by default, i.e. no re-ordering
#ifndef DONOTREORDERTAU
  if(m_rawInput.type_visTau1>0 && m_rawInput.type_visTau2==0) // if hadron-lepton, reorder to have lepton first
    {
      m_InputReorder=1;  // re-order to be done, this flag is to be checked in DoOutputInfo()
    }
  else if(!(m_rawInput.type_visTau2>0 && m_rawInput.type_visTau1==0)) // if not lep-had nor had lep, reorder if tau1 is after tau2 clockwise
    {
      if (fixPhiRange(m_rawInput.vistau1.Phi()-m_rawInput.vistau2.Phi())>0)    {
        m_InputReorder=1; // re-order to be done, this flag is to be checked in DoOutputInfo()
      }
    }
#endif

  if(m_InputReorder==1) // copy and re-order
    {
      m_preparedInput.vistau1 = m_rawInput.vistau2;
      m_preparedInput.vistau2 = m_rawInput.vistau1;
      m_preparedInput.type_visTau1 = m_rawInput.type_visTau2;
      m_preparedInput.type_visTau2 = m_rawInput.type_visTau1;
      m_preparedInput.Nprong_tau1 = m_rawInput.Nprong_tau2;
      m_preparedInput.Nprong_tau2 = m_rawInput.Nprong_tau1;

    } else // simple copy
    {
      m_preparedInput.vistau1 = m_rawInput.vistau1;
      m_preparedInput.vistau2 = m_rawInput.vistau2;
      m_preparedInput.type_visTau1 = m_rawInput.type_visTau1;
      m_preparedInput.type_visTau2 = m_rawInput.type_visTau2;
      m_preparedInput.Nprong_tau1 = m_rawInput.Nprong_tau1;
      m_preparedInput.Nprong_tau2 = m_rawInput.Nprong_tau2;
    }
  //--------- re-ordering is done ---------------------------------------

  m_preparedInput.MetVec = m_rawInput.MetVec;
  m_preparedInput.detMetVec = m_rawInput.MetVec;

  m_preparedInput.m_DelPhiTT = std::abs(TVector2::Phi_mpi_pi(m_preparedInput.vistau1.Phi()-m_preparedInput.vistau2.Phi()));
  double scale = 1.;


  if(m_preparedInput.vistau1.P()>3000.0 || m_preparedInput.vistau2.P()>3000.0) // if units are MeV
    {
      if(m_fUseVerbose==1) std::cout << "converting to GeV" << std::endl;
      scale=0.001;
      m_preparedInput.MetVec *= scale;
      m_preparedInput.detMetVec *= scale;
      m_preparedInput.vistau1 *= scale;
      m_preparedInput.vistau2 *= scale;
    }

  m_preparedInput.m_SumEt = scale*m_rawInput.m_SumEt;
  m_preparedInput.METcovphi = m_rawInput.METcovphi;
  m_preparedInput.METsigmaP = scale*m_rawInput.METsigmaP;
  m_preparedInput.METsigmaL = scale*m_rawInput.METsigmaL;

  m_preparedInput.sigmaEtau1 = scale*m_rawInput.sigmaEtau1;
  m_preparedInput.sigmaEtau2 = scale*m_rawInput.sigmaEtau2;
  m_preparedInput.m_jet4vecs = m_rawInput.m_jet4vecs;

  for(unsigned int i=0; i<m_preparedInput.m_jet4vecs.size(); i++)
    {
      if (scale!=1.) m_preparedInput.m_jet4vecs[i]*=scale;
      // correcting sumEt, give priority to SetMetScanParamsUE()
      if(m_METScanScheme==0)
        {
          if((m_preparedInput.METsigmaP<0.1 || m_preparedInput.METsigmaL<0.1)
             && m_preparedInput.m_SumEt>m_preparedInput.m_jet4vecs[i].Pt()
             && m_preparedInput.m_jet4vecs[i].Pt()>20.0) {
            if(m_fUseVerbose==1) { std::cout << "correcting sumET" << std::endl; }
            m_preparedInput.m_SumEt -= m_preparedInput.m_jet4vecs[i].Pt();
          }
        }
    }

  // give priority to SetVisTauType, only do this if type_visTau1 and type_visTau2 are not set

  if(m_preparedInput.type_visTau1<0 && m_preparedInput.type_visTau2<0 && m_preparedInput.Nprong_tau1>-1 && m_preparedInput.Nprong_tau2>-1)
    {
      if(m_preparedInput.Nprong_tau1==0 || m_preparedInput.Nprong_tau1==1 || m_preparedInput.Nprong_tau1==3) m_preparedInput.type_visTau1=m_preparedInput.Nprong_tau1;
      if(m_preparedInput.Nprong_tau2==0 || m_preparedInput.Nprong_tau2==1 || m_preparedInput.Nprong_tau2==3) m_preparedInput.type_visTau2=m_preparedInput.Nprong_tau2;
    }

  // checking input mass of hadronic tau-1
  // one prong
  //   // checking input mass of hadronic tau-1
  //DRMERGE LFV addition
  if(m_mmcCalibrationSet==MMCCalibrationSet::LFVMMC2012)
    {
      if(m_preparedInput.type_visTau1==1 && m_preparedInput.vistau1.M()!=1.1)
        {
          double n_pt, n_phi, n_eta, n_m;
          n_pt = m_preparedInput.vistau1.Pt();
          n_phi = m_preparedInput.vistau1.Phi();
          n_eta = m_preparedInput.vistau1.Eta();
          n_m = 1.1;
          m_preparedInput.vistau1.SetPtEtaPhiM(n_pt,n_eta,n_phi,n_m);
        }
      if(m_preparedInput.type_visTau1==3 && m_preparedInput.vistau1.M()!=1.35)
        {
          double n_pt, n_phi, n_eta, n_m;
          n_pt = m_preparedInput.vistau1.Pt();
          n_phi = m_preparedInput.vistau1.Phi();
          n_eta = m_preparedInput.vistau1.Eta();
          n_m = 1.35;
          m_preparedInput.vistau1.SetPtEtaPhiM(n_pt,n_eta,n_phi,n_m);
        }
      // checking input mass of hadronic tau-2
      if(m_preparedInput.type_visTau2==1 && m_preparedInput.vistau2.M()!=1.1)
        {
          double n_pt, n_phi, n_eta, n_m;
          n_pt = m_preparedInput.vistau2.Pt();
          n_phi = m_preparedInput.vistau2.Phi();
          n_eta = m_preparedInput.vistau2.Eta();
          n_m = 1.1;
          m_preparedInput.vistau2.SetPtEtaPhiM(n_pt,n_eta,n_phi,n_m);
        }
      if(m_preparedInput.type_visTau2==3 && m_preparedInput.vistau2.M()!=1.35)
        {
          double n_pt, n_phi, n_eta, n_m;
          n_pt = m_preparedInput.vistau2.Pt();
          n_phi = m_preparedInput.vistau2.Phi();
          n_eta = m_preparedInput.vistau2.Eta();
          n_m = 1.35;
          m_preparedInput.vistau2.SetPtEtaPhiM(n_pt,n_eta,n_phi,n_m);
        }
    }
  else
    {
      //DRMERGE end LFV addition
      if(m_preparedInput.type_visTau1==1 && m_preparedInput.vistau1.M()!=0.8)
        {
          double n_pt, n_phi, n_eta, n_m;
          n_pt = m_preparedInput.vistau1.Pt();
          n_phi = m_preparedInput.vistau1.Phi();
          n_eta = m_preparedInput.vistau1.Eta();
          n_m = 0.8;
          m_preparedInput.vistau1.SetPtEtaPhiM(n_pt,n_eta,n_phi,n_m);
        }


      // 3 prong
      if(m_preparedInput.type_visTau1==3 && m_preparedInput.vistau1.M()!=1.2)
        {
          double n_pt, n_phi, n_eta, n_m;
          n_pt = m_preparedInput.vistau1.Pt();
          n_phi = m_preparedInput.vistau1.Phi();
          n_eta = m_preparedInput.vistau1.Eta();
          n_m = 1.2;
          m_preparedInput.vistau1.SetPtEtaPhiM(n_pt,n_eta,n_phi,n_m);
        }
      // checking input mass of hadronic tau-2
      // one prong
      if(m_preparedInput.type_visTau2==1 &&  m_preparedInput.vistau2.M()!=0.8)
        {
          double n_pt, n_phi, n_eta, n_m;
          n_pt = m_preparedInput.vistau2.Pt();
          n_phi = m_preparedInput.vistau2.Phi();
          n_eta = m_preparedInput.vistau2.Eta();
          n_m = 0.8;
          m_preparedInput.vistau2.SetPtEtaPhiM(n_pt,n_eta,n_phi,n_m);
        }
      // 3 prong
      if(m_preparedInput.type_visTau2==3 && m_preparedInput.vistau2.M()!=1.2)
        {
          double n_pt, n_phi, n_eta, n_m;
          n_pt = m_preparedInput.vistau2.Pt();
          n_phi = m_preparedInput.vistau2.Phi();
          n_eta = m_preparedInput.vistau2.Eta();
          n_m = 1.2;
          m_preparedInput.vistau2.SetPtEtaPhiM(n_pt,n_eta,n_phi,n_m);
        }
    } //DRDRMERGE LFV else closing

  // correcting sumEt for electron pt, give priority to SetMetScanParamsUE()
  // DR20150615 in tag 00-00-11 and before. The following was done before the mass of the hadronic tau was set
  // which mean that sumEt was wrongly corrected for the hadronic tau pt if the hadronic tau mass was set to zero
  // Sasha 08/12/15: don't do electron Pt subtraction for high mass studies; in the future, need to check if lepton Pt needs to be subtracted for both ele and muon
  if(m_preparedInput.METsigmaP<0.1 || m_preparedInput.METsigmaL<0.1)
    {
      // T. Davidek: hack for lep-lep -- subtract lepton pT both for muon and electron
      if (m_mmcCalibrationSet == MMCCalibrationSet::MMC2016MC15C &&
          m_preparedInput.vistau1.M() < 0.12 &&
          m_preparedInput.vistau2.M() < 0.12) { // lep-lep channel
        if (m_preparedInput.m_SumEt>m_preparedInput.vistau1.Pt())
          m_preparedInput.m_SumEt -= m_preparedInput.vistau1.Pt();
        if (m_preparedInput.m_SumEt>m_preparedInput.vistau2.Pt())
          m_preparedInput.m_SumEt -= m_preparedInput.vistau2.Pt();
      } else {
        // continue with the original code
        if(m_preparedInput.m_SumEt>m_preparedInput.vistau1.Pt()
           && m_preparedInput.vistau1.M()<0.05
           && m_mmcCalibrationSet != MMCCalibrationSet::MMC2015HIGHMASS) {
          if(m_fUseVerbose==1) { std::cout  << "Substracting pt1 from sumEt" << std::endl; }
          m_preparedInput.m_SumEt -= m_preparedInput.vistau1.Pt();
        }
        if(m_preparedInput.m_SumEt>m_preparedInput.vistau2.Pt()
           && m_preparedInput.vistau2.M()<0.05
           && m_mmcCalibrationSet != MMCCalibrationSet::MMC2015HIGHMASS) {
          if(m_fUseVerbose==1) { std::cout  << "Substracting pt2 from sumEt" << std::endl; }
          m_preparedInput.m_SumEt -= m_preparedInput.vistau2.Pt();
        }
      }
    }


  // give priority to SetMetScanParamsUE()
  m_preparedInput.UseHT = false;
  m_preparedInput.allowUseHT = m_rawInput.allowUseHT;
  m_preparedInput.m_Njet25 = m_rawInput.m_Njet25;

  // controling TauProbability settings for UPGRADE studies
  if(m_mmcCalibrationSet==MMCCalibrationSet::UPGRADE && m_fUseDefaults==1)
    {
      if((m_preparedInput.vistau1.M() < 0.12 && m_preparedInput.vistau2.M() > 0.12) || (m_preparedInput.vistau2.M() < 0.12 && m_preparedInput.vistau1.M() > 0.12)) {
        m_fUseTauProbability=1; // lep-had case
      }
      if(m_preparedInput.vistau1.M() > 0.12 && m_preparedInput.vistau2.M() > 0.12) {
        m_fUseTauProbability=0; // had-had case
      }
    }



  //  compute tau energy resolution
  // FIXME should be possible to set sigmaEtau1 from outside
  m_preparedInput.sigmaEtau1 = m_rawInput.sigmaEtau1;
  m_preparedInput.sigmaEtau2 = m_rawInput.sigmaEtau2;

  if (m_rawInput.sigmaEtau1<0) m_preparedInput.sigmaEtau1 = tauSigmaE(m_preparedInput.vistau1,m_preparedInput.type_visTau1);
  if (m_rawInput.sigmaEtau2<0) m_preparedInput.sigmaEtau2 = tauSigmaE(m_preparedInput.vistau2,m_preparedInput.type_visTau2);


  m_preparedInput.m_MHtSigma1 = m_rawInput.m_MHtSigma1;
  m_preparedInput.m_MHtSigma2 = m_rawInput.m_MHtSigma2;
  m_preparedInput.MHtGaussFr = m_rawInput.MHtGaussFr;

  // change Beam Energy for different running conditions
  if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2015
     || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS
     || m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C
     || m_mmcCalibrationSet==MMCCalibrationSet::UPGRADE) m_beamEnergy=6500.0; // 13 TeV running

  //--------------------- pre-set defaults for Run-2. To disable pre-set defaults set fUseDefaults=0
  if(m_fUseDefaults==1)
    {
      if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS)
        {
          m_nsigma_METscan_ll = 4.0;
          m_nsigma_METscan_lh = 4.0;
          m_nsigma_METscan_hh = 4.0;
          m_fUseTailCleanup = 0;
          if((m_preparedInput.vistau1.M()<0.12 && m_preparedInput.vistau2.M()>0.12) || (m_preparedInput.vistau2.M()<0.12 && m_preparedInput.vistau1.M()>0.12)) m_fUseTauProbability=0; // lep-had
          if(m_preparedInput.type_visTau1>0 && m_preparedInput.type_visTau2>0) m_fUseTauProbability=1; // had-had
          m_fUseMnuProbability=0;
        }
    }


  // compute MET resolution (eventually use HT)
  if(m_preparedInput.METsigmaP<0.1 || m_preparedInput.METsigmaL<0.1)
    {
      if(m_mmcCalibrationSet==MMCCalibrationSet::LFVMMC2012)
        {
          if(m_fUseVerbose==1) { std::cout << "Attempting to set LFV MMC settings" << std::endl; }
          double mT1 = mT(m_preparedInput.vistau1,m_preparedInput.MetVec);
          double mT2 = mT(m_preparedInput.vistau2,m_preparedInput.MetVec);
          int sr_switch = 0;
          if(m_preparedInput.vistau1.M()<0.12 && m_preparedInput.vistau2.M()<0.12) // lep-lep case
            {
              if(m_LFVmode==0) // e+tau(->mu) case
                {
                  if(m_preparedInput.vistau1.M()<0.05 && m_preparedInput.vistau2.M()>0.05)
                    {
                      if(mT1>mT2) sr_switch = 0; // SR1
                      else sr_switch = 1; // SR2
                    }
                  else
                    {
                      if(mT1>mT2) sr_switch = 1; // SR2
                      else sr_switch = 0; // SR1
                    }
                }
              if(m_LFVmode==1) // mu+tau(->e) case
                {
                  if(m_preparedInput.vistau1.M()>0.05 && m_preparedInput.vistau2.M()<0.05)
                    {
                      if(mT1>mT2) sr_switch = 0; // SR1
                      else sr_switch = 1; // SR2
                    }
                  else
                    {
                      if(mT1>mT2) sr_switch = 1; // SR2
                      else sr_switch = 0; // SR1
                    }
                }
            }
          if((m_preparedInput.vistau1.M()<0.12 && m_preparedInput.vistau2.M()>0.12) || (m_preparedInput.vistau2.M()<0.12 && m_preparedInput.vistau1.M()>0.12)) // lep-had case
            {
              if(m_preparedInput.vistau1.M()<0.12 && m_preparedInput.vistau2.M()>0.12)
                {
                  if(mT1>mT2) sr_switch = 0; // SR1
                  else sr_switch = 1; // SR2
                }
              else
                {
                  if(mT1>mT2) sr_switch = 1; // SR2
                  else sr_switch = 0; // SR1
                }
            }

          m_preparedInput.UseHT = false;
          if(m_preparedInput.m_Njet25==0) // 0-jet
            {
              double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
              double METresScale;
              double METoffset;
              if(sr_switch==0)
                {
                  METresScale = 0.41*(1.0+m_METresSyst*sigmaSyst);
                  METoffset = 7.36*(1.0+m_METresSyst*sigmaSyst);
                }
              else
                {
                  METresScale = 0.34*(1.0+m_METresSyst*sigmaSyst);
                  METoffset = 6.61*(1.0+m_METresSyst*sigmaSyst);
                }
              if(m_fUseVerbose==1) {
                std::cout << "SumEt = " <<  m_preparedInput.m_SumEt << std::endl;
                std::cout << "METoffset = " <<  METoffset << std::endl;
                std::cout << "METresScale = " <<  METresScale << std::endl;
              }

              double sigma = m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
              m_preparedInput.METsigmaP = sigma;
              m_preparedInput.METsigmaL = sigma;
              if(m_fUseVerbose==1) {
                std::cout << "=> METsigmaP = " << m_preparedInput.METsigmaP << std::endl;
                std::cout << "=> METsigmaL = " << m_preparedInput.METsigmaL << std::endl;
              }
            }
          if(m_preparedInput.m_Njet25>0) // Inclusive 1-jet
            {
              double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
              double sigma = 0.;
              double METresScale;
              double METoffset;
              if(sr_switch==0)
                {
                  METresScale = 0.38*(1.0+m_METresSyst*sigmaSyst);
                  METoffset = 7.96*(1.0+m_METresSyst*sigmaSyst);
                }
              else
                {
                  METresScale = 0.39*(1.0+m_METresSyst*sigmaSyst);
                  METoffset = 6.61*(1.0+m_METresSyst*sigmaSyst);
                }

              // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt = 5.0 for now
              sigma = m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
              m_preparedInput.METsigmaP = sigma;
              m_preparedInput.METsigmaL = sigma;
            } // Njet25>0
        }
      else //LFV
        {
          //DRDRMERGE end addition

          if(m_METScanScheme==1) // default for Winter 2012 and further
            {
	      //LEP-HAD
              if (m_preparedInput.type_visTau1+m_preparedInput.type_visTau2>0 && m_preparedInput.type_visTau1*m_preparedInput.type_visTau2==0) // lephad case
                {
		  //0-jet
                  if(m_preparedInput.m_Njet25==0)//0-jet
                    {
                      if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2011){
                        if (m_preparedInput.allowUseHT) {
                          m_preparedInput.UseHT = true;
                          if(m_preparedInput.detMetVec.Mod()<20.0) // 0-jet low MET case
                            {
                              // giving priority to external settings
                              if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1 = 5.89;
                              if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2 = 15.47;
                              if(m_preparedInput.MHtGaussFr<0.0) m_preparedInput.MHtGaussFr = 0.48;
                            } else
                            {
                              // giving priority to external settings
                              if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1 = 6.47;
                              if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2 = 16.82;
                              if(m_preparedInput.MHtGaussFr<0.0) m_preparedInput.MHtGaussFr = 0.4767;
                            }
                        } else // if disallow use of HT, then fall back to same tuning as 1 jet
                          {
                            double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                            double METresScale = 0.56*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                            double METoffset = 3.73*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                            // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt = 5.0 for now
                            double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                            m_preparedInput.METsigmaP = sigma;
                            m_preparedInput.METsigmaL = sigma;
                            std::cout << "DRDR lh nj0 2011 sigma = "<< sigma << std::endl;
                          }
                      }
                      else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2012){
                        if(m_preparedInput.detMetVec.Mod()<20.0) // 0-jet low MET case
                          {
                            //--------- tag-00-00-10 modifications, for HCP-2012 analysis on 8 TeV data
                            if(std::abs(m_preparedInput.m_DelPhiTT)>2.95 && m_preparedInput.allowUseHT) // use mHt only if dPhi(lep-tau)>2.95
                              {
                                m_preparedInput.UseHT = true;
                                // giving priority to external settings
                                if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1 = 4.822;
                                if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2 = 10.31;
                                if(m_preparedInput.MHtGaussFr<0.0) m_preparedInput.MHtGaussFr = 6.34E-5;
                              }
                            else
                              {
                                m_preparedInput.UseHT = false;
                                double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                                double METresScale = 0.32*(1.0+m_METresSyst*sigmaSyst);
                                double METoffset = 5.38*(1.0+m_METresSyst*sigmaSyst);
                                double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                                m_preparedInput.METsigmaP = sigma;
                                m_preparedInput.METsigmaL = sigma;
                              }
                          }
                        else // 0-jet high MET case
                          {
                            //--------- tag-00-00-10 modifications, for HCP-2012 analysis on 8 TeV data
                            if(std::abs(m_preparedInput.m_DelPhiTT)>2.8 && m_preparedInput.allowUseHT) // use mHt only if dPhi(lep-tau)>2.8
                              {
                                m_preparedInput.UseHT = true;
                                // giving priority to external settings
                                if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1 = 7.5;
                                if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2 = 13.51;
                                if(m_preparedInput.MHtGaussFr<0.0) m_preparedInput.MHtGaussFr = 6.81E-4;
                                m_preparedInput.METsigmaP = m_preparedInput.m_MHtSigma2; // sigma of 2nd Gaussian for missing Ht resolution
                                m_preparedInput.METsigmaL = m_preparedInput.m_MHtSigma2;
                              }
                            else
                              {
                                m_preparedInput.UseHT = false;
                                double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                                double METresScale = 0.87*(1.0+m_METresSyst*sigmaSyst);
                                double METoffset = 4.16*(1.0+m_METresSyst*sigmaSyst);
                                double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                                m_preparedInput.METsigmaP = sigma;
                                m_preparedInput.METsigmaL = sigma;
                              }
                          } // high MET
                      } // MMC2012
                      // placeholder for 2015 tune; for now 2015 settings are the same as 2012, to be changed int he future;
                      else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2015){
                        if(m_preparedInput.detMetVec.Mod()<20.0) // 0-jet low MET case
                          {
                            //--------- tag-00-00-10 modifications, for HCP-2012 analysis on 8 TeV data
                            if(std::abs(m_preparedInput.m_DelPhiTT)>2.95 && m_preparedInput.allowUseHT) // use mHt only if dPhi(lep-tau)>2.95
                              {
                                m_preparedInput.UseHT = true;
                                // giving priority to external settings
                                if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1 = 4.822;
                                if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2 = 10.31;
                                if(m_preparedInput.MHtGaussFr<0.0) m_preparedInput.MHtGaussFr = 6.34E-5;
                              }
                            else
                              {
                                m_preparedInput.UseHT = false;
                                double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                                double METresScale = 0.32*(1.0+m_METresSyst*sigmaSyst);
                                double METoffset = 5.38*(1.0+m_METresSyst*sigmaSyst);
                                double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                                m_preparedInput.METsigmaP = sigma;
                                m_preparedInput.METsigmaL = sigma;
                              }
                          }
                        else // 0-jet high MET case
                          {
                            //--------- tag-00-00-10 modifications, for HCP-2012 analysis on 8 TeV data
                            if(std::abs(m_preparedInput.m_DelPhiTT)>2.8 && m_preparedInput.allowUseHT) // use mHt only if dPhi(lep-tau)>2.8
                              {
                                m_preparedInput.UseHT = true;
                                // giving priority to external settings
                                if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1 = 7.5;
                                if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2 = 13.51;
                                if(m_preparedInput.MHtGaussFr<0.0) m_preparedInput.MHtGaussFr = 6.81E-4;
                                m_preparedInput.METsigmaP = m_preparedInput.m_MHtSigma2; // sigma of 2nd Gaussian for missing Ht resolution
                                m_preparedInput.METsigmaL = m_preparedInput.m_MHtSigma2;
                              }
                            else
                              {
                                m_preparedInput.UseHT = false;
                                double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                                double METresScale = 0.87*(1.0+m_METresSyst*sigmaSyst);
                                double METoffset = 4.16*(1.0+m_METresSyst*sigmaSyst);
                                double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                                m_preparedInput.METsigmaP = sigma;
                                m_preparedInput.METsigmaL = sigma;
                              }
                          } // high MET
                      } // MMC2015
                      // placeholder for 2016MC15c tune
                      else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C){
                        if(m_preparedInput.detMetVec.Mod()<20.0) // 0-jet low MET case
                          {
                            if(std::abs(m_preparedInput.m_DelPhiTT)>2.95 && m_preparedInput.allowUseHT) // use mHt only if dPhi(lep-tau)>2.95
                              {
                                m_preparedInput.UseHT = true;
                                // giving priority to external settings
                                if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1 = 4.822;
                                if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2 = 10.31;
                                if(m_preparedInput.MHtGaussFr<0.0) m_preparedInput.MHtGaussFr = 6.34E-5;
                              }
                            else
                              {
                                m_preparedInput.UseHT = false;
                                double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                                double METresScale = 0.32*(1.0+m_METresSyst*sigmaSyst);
                                double METoffset = 5.38*(1.0+m_METresSyst*sigmaSyst);
                                double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                                m_preparedInput.METsigmaP = sigma;
                                m_preparedInput.METsigmaL = sigma;
                              }
                          }
                        else // 0-jet high MET case
                          {
                            if(std::abs(m_preparedInput.m_DelPhiTT)>2.8 && m_preparedInput.allowUseHT) // use mHt only if dPhi(lep-tau)>2.8
                              {
                                m_preparedInput.UseHT = true;
                                // giving priority to external settings
                                if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1 = 7.5;
                                if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2 = 13.51;
                                if(m_preparedInput.MHtGaussFr<0.0) m_preparedInput.MHtGaussFr = 6.81E-4;
                                m_preparedInput.METsigmaP = m_preparedInput.m_MHtSigma2; // sigma of 2nd Gaussian for missing Ht resolution
                                m_preparedInput.METsigmaL = m_preparedInput.m_MHtSigma2;
                              }
                            else
                              {
                                m_preparedInput.UseHT = false;
                                double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                                double METresScale = 0.87*(1.0+m_METresSyst*sigmaSyst);
                                double METoffset = 4.16*(1.0+m_METresSyst*sigmaSyst);
                                double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                                m_preparedInput.METsigmaP = sigma;
                                m_preparedInput.METsigmaL = sigma;
                              }
                          } // high MET
                      } // MMC2016MC15C
                      // 2015 high-mass tune; avergare MET resolution for Mh=600,1000 mass points
                      else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS)
                        {
                          m_preparedInput.UseHT = false;
                          double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                          double METresScale = 0.65*(1.0+m_METresSyst*sigmaSyst);
                          double METoffset = 5.0*(1.0+m_METresSyst*sigmaSyst);
                          double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                          m_preparedInput.METsigmaP = sigma;
                          m_preparedInput.METsigmaL = sigma;
                        } // MMC2015HIGHMASS
                    } // 0 jet
		  //1-jet
                  else if(m_preparedInput.m_Njet25>0) // Inclusive 1-jet and VBF lep-had categories for Winter 2012
                    {
                      double sigmaSyst=0.10; // 10% systematics for now (be conservative)
                      double sigma=0.;
                      if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2011)
                        {
                          double METresScale=0.56*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                          double METoffset=3.73*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                          // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt=5.0 for now
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                        }
                      else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2012)
                        {
                          double METresScale=0.85*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012 => updated for HCP 2012
                          double METoffset=5.94*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012 => updated for HCP 2012
                          // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt=5.0 for now
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                        }
                      // placeholder for 2015 tune; for now 2015 settings are the same as 2012, to be changed int he future;
                      else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2015)
                        {
                          double METresScale=0.85*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012 => updated for HCP 2012
                          double METoffset=5.94*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012 => updated for HCP 2012
                          // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt=5.0 for now
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                        }
                      // 2015 high-mass tune; average MET resolution for Mh=400,600 mass points (they look consistent);
                      else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS)
                        {
                          double METresScale=0.86*(1.0+m_METresSyst*sigmaSyst);
                          double METoffset=3.0*(1.0+m_METresSyst*sigmaSyst);
                          // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt=5.0 for now
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                        }
                      //2016 mc15c
		      else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C)
                        {
			  double x = m_preparedInput.m_DelPhiTT;
			  double dphi_scale = x > 0.3 ? 0.9429 - 0.059*x + 0.054*x*x : 0.728;
			  double METoffset    = 1.875*(1.0+m_METresSyst*sigmaSyst);
			  double METresScale1 = 8.914*(1.0+m_METresSyst*sigmaSyst);
			  double METresScale2 = -8.53*(1.0+m_METresSyst*sigmaSyst);

			  sigma = m_preparedInput.m_SumEt > 80.0 ? METoffset + METresScale1*TMath::Log(sqrt(m_preparedInput.m_SumEt)+METresScale2) : 5.0;
			  sigma = sigma * dphi_scale;
                        }
		      //

                      m_preparedInput.METsigmaP=sigma;
                      m_preparedInput.METsigmaL=sigma;
                    } // Njet25>0
		  
                } // lep-had

	      //HAD-HAD
              else if(m_preparedInput.type_visTau1>0 && m_preparedInput.type_visTau2>0) // had-had events
                {
                  if(m_preparedInput.m_Njet25==0 && m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS) //0-jet high mass hadhad
                    {
                      // 2015 high-mass tune; average of all mass points
                      //                      double METresScale=-1.;
                      //                      double METoffset=-1.;
                      double sigmaSyst=0.10; // 10% systematics for now (be conservative)

                      double METresScale=0.9*(1.0+m_METresSyst*sigmaSyst);
                      double METoffset=-1.8*(1.0+m_METresSyst*sigmaSyst);
                      double sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : std::abs(METoffset);
                      m_preparedInput.METsigmaP=sigma;
                      m_preparedInput.METsigmaL=sigma;

                    }
                  else if(m_preparedInput.m_Njet25==0 && m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C)
                    {
                      double sigmaSyst=0.10; // 10% systematics for now (be conservative)
                      double x = m_preparedInput.m_DelPhiTT;
                      double dphi_scale = x > 2.5 ? 11.0796 - 4.61236*x + 0.423617*x*x : 2.;
                      double METoffset = -8.51013*(1.0+m_METresSyst*sigmaSyst);
                      double METresScale1 = 8.54378*(1.0+m_METresSyst*sigmaSyst);
                      double METresScale2 = -3.97146*(1.0+m_METresSyst*sigmaSyst);
                      double sigma= m_preparedInput.m_SumEt>80.0 ? METoffset+METresScale1*TMath::Log(sqrt(m_preparedInput.m_SumEt)+METresScale2) : 5.;
                      sigma = sigma*dphi_scale;

                      m_preparedInput.METsigmaP=sigma;
                      m_preparedInput.METsigmaL=sigma;

                    }
                  else if(m_preparedInput.m_Njet25==0 && m_preparedInput.allowUseHT) // 0-jet high MET had-had category for Winter 2012
                    {

                      m_preparedInput.UseHT=true; // uncomment this line to enable HT also for HH (crucial)
                      // updated for final cuts, may 21 2012
                      if(m_preparedInput.m_MHtSigma1<0.0) m_preparedInput.m_MHtSigma1=5.972;
                      if(m_preparedInput.m_MHtSigma2<0.0) m_preparedInput.m_MHtSigma2=13.85;
                      //                  if(MHtGaussFr<0.0) MHtGaussFr=0.4767; // don't directly use 2nd fraction
                    }
		  //1-jet
                  else  // Inclusive 1-jet and VBF categories
                    {
                      double METresScale=-1.;
                      double METoffset=-1.;
                      double sigmaSyst=0.10; // 10% systematics for now (be conservative)

                      // previous value in trunk
                      // double METresScale=0.56*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                      //double METoffset=3.73*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                      if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2011){
                        METresScale = 0.56*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                        METoffset = 3.73*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                      } else if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2012) {
                        METresScale = 0.5*(1.0+m_METresSyst*sigmaSyst); // for hh 2013
                        METoffset = 6.14*(1.0+m_METresSyst*sigmaSyst);  // for hh 2013
                      } else if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2015) {
                        // placeholder for 2015 tune; same as 2012 tune for now
                        METresScale = 0.5*(1.0+m_METresSyst*sigmaSyst);
                        METoffset = 6.14*(1.0+m_METresSyst*sigmaSyst);
                      } else if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS) {
                        // 2015 high-mass tune; average of all mass points
                        METresScale = 1.1*(1.0+m_METresSyst*sigmaSyst);
                        METoffset = -5.0*(1.0+m_METresSyst*sigmaSyst);
                      }
                      // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt=5.0 for now
                      double sigma =  m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : std::abs(METoffset);

                      if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C) {
                        double x = m_preparedInput.m_DelPhiTT;
                        double dphi_scale = x > 0.6 ? 1.42047 - 0.666644*x + 0.199986*x*x : 1.02;
                        METoffset = 1.19769*(1.0+m_METresSyst*sigmaSyst);
                        double METresScale1 = 5.61687*(1.0+m_METresSyst*sigmaSyst);
                        double METresScale2 = -4.2076*(1.0+m_METresSyst*sigmaSyst);
                        sigma= m_preparedInput.m_SumEt>115.0 ? METoffset+METresScale1*TMath::Log(sqrt(m_preparedInput.m_SumEt)+METresScale2) : 12.1;
                        sigma = sigma*dphi_scale;
                      } //for hh 2016 mc15c

                      m_preparedInput.METsigmaP = sigma;
                      m_preparedInput.METsigmaL = sigma;

                    }// 1 jet
                } // had-had
	      //LEP-LEP
              else if(m_preparedInput.type_visTau1==0 && m_preparedInput.type_visTau2==0) // setup for LEP-LEP channel
                {
                  if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2011) // no tune for 7 TeV
                    {
                      double sigmaSyst=0.10; // 10% systematics for now (be conservative)
                      double METresScale=0.56*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                      double METoffset=3.73*(1.0+m_METresSyst*sigmaSyst); // for events with jets & analysis cuts for winter 2012
                      // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt=5.0 for now
                      double sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                      m_preparedInput.METsigmaP=sigma;
                      m_preparedInput.METsigmaL=sigma;
                    }
		  
                  if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2012) // tuned version of MET+STVF resolution for lep-lep events. (tune done by Tomas)
                    {
                      m_preparedInput.UseHT=false;
                      double sigmaSyst=0.10; // 10% systematics for now (be conservative)
                      double METresScale=-1.0;
                      double METoffset=-1.0;
                      double sigma=5.0;
                      // tune is based on cuts for Run-1 paper analysis
                      if(m_preparedInput.m_Njet25==0)
                        {
                          // use tune for emebedding
                          METresScale=-0.4307*(1.0+m_METresSyst*sigmaSyst);
                          METoffset=7.06*(1.0+m_METresSyst*sigmaSyst);
                          double METresScale2=0.07693*(1.0+m_METresSyst*sigmaSyst); // quadratic term
                          // this is a tune for Higgs125
                          //                      METresScale=-0.5355*(1.0+m_METresSyst*sigmaSyst);
                          //                      METoffset=11.5*(1.0+m_METresSyst*sigmaSyst);
                          //                      double METresScale2=0.07196*(1.0+m_METresSyst*sigmaSyst); // quadratic term
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt)+METresScale2*m_preparedInput.m_SumEt : METoffset;
                        }
                      if(m_preparedInput.m_Njet25>0)
                        {
                          // use tune for embedding
                          METresScale=0.8149*(1.0+m_METresSyst*sigmaSyst);
                          METoffset=5.343*(1.0+m_METresSyst*sigmaSyst);
                          // this is a tune for Higgs125
                          //                      METresScale=0.599*(1.0+m_METresSyst*sigmaSyst);
                          //                      METoffset=8.223*(1.0+m_METresSyst*sigmaSyst);
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                        }
                      m_preparedInput.METsigmaP=sigma;
                      m_preparedInput.METsigmaL=sigma;
                    }

                  if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2015) // placeholder for 2015 tune; for now it is the same as 2012
                    {
                      m_preparedInput.UseHT=false;
                      double sigmaSyst=0.10; // 10% systematics for now (be conservative)
                      double METresScale=-1.0;
                      double METoffset=-1.0;
                      double sigma=5.0;
                      // tune is based on cuts for Run-1 paper analysis
                      if(m_preparedInput.m_Njet25==0)
                        {
                          // use tune for emebedding
                          METresScale=-0.4307*(1.0+m_METresSyst*sigmaSyst);
                          METoffset=7.06*(1.0+m_METresSyst*sigmaSyst);
                          double METresScale2=0.07693*(1.0+m_METresSyst*sigmaSyst); // quadratic term
                          // this is a tune for Higgs125
                          //                      METresScale=-0.5355*(1.0+m_METresSyst*sigmaSyst);
                          //                      METoffset=11.5*(1.0+m_METresSyst*sigmaSyst);
                          //                      double METresScale2=0.07196*(1.0+m_METresSyst*sigmaSyst); // quadratic term
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt)+METresScale2*m_preparedInput.m_SumEt : METoffset;
                        }
                      if(m_preparedInput.m_Njet25>0)
                        {
                          // use tune for embedding
                          METresScale=0.8149*(1.0+m_METresSyst*sigmaSyst);
                          METoffset=5.343*(1.0+m_METresSyst*sigmaSyst);
                          // this is a tune for Higgs125
                          //                      METresScale=0.599*(1.0+m_METresSyst*sigmaSyst);
                          //                      METoffset=8.223*(1.0+m_METresSyst*sigmaSyst);
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                        }
                      m_preparedInput.METsigmaP=sigma;
                      m_preparedInput.METsigmaL=sigma;
                    } // end of MMC2015

                  if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS) // placeholder for 2015 high-mass tune; for now it is the same as 2012
                    {
                      m_preparedInput.UseHT = false;
                      double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
                      double METresScale = -1.0;
                      double METoffset = -1.0;
                      double sigma = 5.0;
                      // tune is based on cuts for Run-1 paper analysis
                      if(m_preparedInput.m_Njet25==0)
                        {
                          // use tune for emebedding
                          METresScale=-0.4307*(1.0+m_METresSyst*sigmaSyst);
                          METoffset=7.06*(1.0+m_METresSyst*sigmaSyst);
                          double METresScale2=0.07693*(1.0+m_METresSyst*sigmaSyst); // quadratic term
                          // this is a tune for Higgs125
                          //                      METresScale=-0.5355*(1.0+m_METresSyst*sigmaSyst);
                          //                      METoffset=11.5*(1.0+m_METresSyst*sigmaSyst);
                          //                      double METresScale2=0.07196*(1.0+m_METresSyst*sigmaSyst); // quadratic term
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt)+METresScale2*m_preparedInput.m_SumEt : METoffset;
                        }
                      if(m_preparedInput.m_Njet25>0)
                        {
                          // use tune for embedding
                          METresScale=0.8149*(1.0+m_METresSyst*sigmaSyst);
                          METoffset=5.343*(1.0+m_METresSyst*sigmaSyst);
                          // this is a tune for Higgs125
                          //                      METresScale=0.599*(1.0+m_METresSyst*sigmaSyst);
                          //                      METoffset=8.223*(1.0+m_METresSyst*sigmaSyst);
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt) : METoffset;
                        }
                      m_preparedInput.METsigmaP = sigma;
                      m_preparedInput.METsigmaL = sigma;
                    } // end of MMC2015HIGHMASS

                  if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C) // 2016 MC15c leplep
                    {
		      m_preparedInput.UseHT=false;
                      double sigmaSyst=0.10; // 10% systematics for now (be conservative)
                      double METresScale=-1.0;
                      double METoffset=-1.0;
                      double sigma=5.0;
                      double min_sigma = 2.0;
                      // tune is based on cuts for Run-1 paper analysis
                      if(m_preparedInput.m_Njet25==0)
                        {
                          // Madgraph Ztautau MET param
                          METoffset = 4.22581*(1.0+m_METresSyst*sigmaSyst);
                          METresScale = 0.03818*(1.0+m_METresSyst*sigmaSyst);
                          double METresScale2= 0.12623;
                          sigma= m_preparedInput.m_SumEt>0.0 ? METoffset+METresScale*sqrt(m_preparedInput.m_SumEt)+METresScale2*m_preparedInput.m_SumEt : min_sigma;
                          if (m_fUseDphiLL) {
                            double p0 = 2.60131;
                            double p1const = 1.22427;
                            double p2quad = -1.71261;
                            double DphiLL = std::abs(TVector2::Phi_mpi_pi(m_preparedInput.vistau1.Phi()-m_preparedInput.vistau2.Phi()));
                            sigma *= (DphiLL < p0) ? p1const : p1const+
                              p2quad*p0*p0 - 2*p2quad*p0*DphiLL+p2quad*DphiLL*DphiLL;
                          }
                          if (sigma < min_sigma) sigma = min_sigma;
                        }
		      if(m_preparedInput.m_Njet25>0)
                        {
                          // Madgraph Ztautau MET param
                          METoffset = 5.42506*(1.0+m_METresSyst*sigmaSyst);
                          METresScale = 5.36760*(1.0+m_METresSyst*sigmaSyst);
                          double METoffset2 = -4.86808*(1.0+m_METresSyst*sigmaSyst);
                          if (m_preparedInput.m_SumEt > 0.0) {
                            double x = sqrt(m_preparedInput.m_SumEt);
                            sigma = (x+METoffset2 > 1) ? METoffset+METresScale*log(x+METoffset2) : METoffset;
                          } else {
                            sigma = METoffset;
                          }
                          if (m_fUseDphiLL) {
                            double p0 = 2.24786;
                            double p1const = 0.908597;
                            double p2quad = 0.544577;
                            double DphiLL = std::abs(TVector2::Phi_mpi_pi(m_preparedInput.vistau1.Phi()-m_preparedInput.vistau2.Phi()));
                            sigma *= (DphiLL < p0) ? p1const : p1const+
                              p2quad*p0*p0 - 2*p2quad*p0*DphiLL+p2quad*DphiLL*DphiLL;
                          }
                          if (sigma < min_sigma) sigma = min_sigma;
                        }
                      m_preparedInput.METsigmaP=sigma;
                      m_preparedInput.METsigmaL=sigma;
                    }//2016 mc15c

                } // lep-lep

            } //METScanScheme

          if(m_METScanScheme==0) // old scheme with JER
            {
              if(m_preparedInput.dataType==0 || m_preparedInput.dataType==1) SetMetScanParamsUE(m_preparedInput.m_SumEt,m_preparedInput.METcovphi,m_preparedInput.dataType);
              else SetMetScanParamsUE(m_preparedInput.m_SumEt,m_preparedInput.METcovphi,0);
            }
        }
    } // end else LFV


  // compute HTOffset if relevant
  m_preparedInput.HtOffset=m_rawInput.HtOffset;
  if(m_preparedInput.UseHT )  // use missing Ht for Njet25=0 events
    {
      // dPhi(l-t) dependence of misHt-trueMET
      double HtOffset=0.;
      // proper for hh
      if (m_preparedInput.type_visTau1!=0 && m_preparedInput.type_visTau2!=0) {
        // hh
        double x=m_preparedInput.m_DelPhiTT;
        HtOffset=87.5-27.0*x;
      } else {

        // FIXME the condition is really on MET non on HT ?
        if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2011){
          if(m_preparedInput.detMetVec.Mod()<20.0) HtOffset=m_preparedInput.m_DelPhiTT>1.8 ? -161.9+235.5*m_preparedInput.m_DelPhiTT-101.6*pow(m_preparedInput.m_DelPhiTT,2)+13.57*pow(m_preparedInput.m_DelPhiTT,3) : 12.0;
          else HtOffset=115.5-78.1*m_preparedInput.m_DelPhiTT+12.83*pow(m_preparedInput.m_DelPhiTT,2);

        } else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2012){
          if(m_preparedInput.detMetVec.Mod()<20.0) HtOffset=132.1-79.26*m_preparedInput.m_DelPhiTT+11.77*pow(m_preparedInput.m_DelPhiTT,2); // updated for HCP-2012, 8 TeV
          else HtOffset=51.28-23.56*m_preparedInput.m_DelPhiTT+2.637*pow(m_preparedInput.m_DelPhiTT,2); // updated for HCP-2012, 8 TeV
        } else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2015){
          if(m_preparedInput.detMetVec.Mod()<20.0) HtOffset=132.1-79.26*m_preparedInput.m_DelPhiTT+11.77*pow(m_preparedInput.m_DelPhiTT,2); // updated for HCP-2012, 8 TeV
          else HtOffset=51.28-23.56*m_preparedInput.m_DelPhiTT+2.637*pow(m_preparedInput.m_DelPhiTT,2); // updated for HCP-2012, 8 TeV
        }

      }
      m_preparedInput.HtOffset=HtOffset;
    }

  // if use HT, replace MET with HT
  if (m_preparedInput.UseHT)
    {
      m_preparedInput.METsigmaP=m_preparedInput.m_MHtSigma2; // sigma of 2nd Gaussian for missing Ht resolution
      m_preparedInput.METsigmaL=m_preparedInput.m_MHtSigma2;

      TLorentzVector tauSum=m_preparedInput.vistau1+m_preparedInput.vistau2;
      m_preparedInput.MetVec.Set(-tauSum.Px(),-tauSum.Py()); // WARNING this replace metvec by -mht
    }

  DoMetResolution(m_preparedInput);

  return;
}


//------- clearing output stuff
void MissingMassCalculator::ClearOutputStuff() {

  if(m_fUseVerbose == 1){ std::cout << "MissingMassCalculator::ClearOutputStuff()" << std::endl; }
  m_OutputInfo.FitStatus=0;

  for (int imeth=0; imeth<MMCFitMethod::MAX; ++imeth)
    {
      if(m_fUseVerbose == 1){ std::cout << "MissingMassCalculator::ClearOutputStuff(): clearing for method " << imeth << std::endl; }
      m_OutputInfo.FitSignificance[imeth] = -1.0;
      m_OutputInfo.FittedMass[imeth] = 0.0;
      m_OutputInfo.nuvec1[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_OutputInfo.objvec1[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_OutputInfo.nuvec2[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_OutputInfo.objvec2[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_OutputInfo.totalvec[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_OutputInfo.FittedMetVec[imeth].Set(0.0,0.0);
    }

  m_OutputInfo.RMS2MPV = 0.0;

  m_aPOI.prob.clear();
  m_aPOI.mtautau.clear();
  m_aPOI.Phi1.clear();
  m_aPOI.Phi2.clear();
  m_aPOI.MEtP.clear();
  m_aPOI.MEtL.clear();
  m_aPOI.MEtX.clear();
  m_aPOI.MEtY.clear();
  m_aPOI.Mnu1.clear();
  m_aPOI.Mnu2.clear();
  m_aPOI.eTau1.clear();
  m_aPOI.eTau2.clear();
  m_aPOI.pCode.clear();
  m_aPOI.index.clear();
  return;
}

//---- finalizes MET resolution parameters
// OBSOLETE
void MissingMassCalculator::DoMetResolution(InputInfoStuff &fStuff) {
  if(fStuff.m_jet4vecs.size()>0 && m_METScanScheme==0)
    {
      std::cout << "ERROR MissingMassCalculator::DoMetResolution is obsolete " << std::endl;
    }
  return;
}


//---------------------------- Accessors to output parameters ------------------------
//
// return fit status
int MissingMassCalculator::GetFitStatus() {
  return m_OutputInfo.FitStatus;
}

// returns fit significance
double MissingMassCalculator::GetFitSignificance(int fitcode) {
  double signif = -1.0;
  if (fitcode<0 || fitcode >= MMCFitMethod::MAX) {
    std::cout << "MissingMassCalculator::GetFitSignificance ERROR ! fitcode=" << fitcode
              << "Should be between 0 and " << MMCFitMethod::MAX-1 << std::endl;
  } else {
    signif = m_OutputInfo.FitSignificance[fitcode];
  }

  return signif;
}

// returns RMS/MPV according to histogram method
double MissingMassCalculator::GetRms2Mpv() {
  return m_OutputInfo.RMS2MPV;
}

// returns fitted Mass
double MissingMassCalculator::GetFittedMass(int fitcode) {
  // 0 best parameter space
  // 1 best mass
  // best nu from hist
  double mass = 0.0;
  if (fitcode<0 || fitcode >= MMCFitMethod::MAX) {
    std::cout << "MissingMassCalculator::GetFittedMass ERROR ! fitcode=" << fitcode
              << "Should be between 0 and " << MMCFitMethod::MAX-1 << std::endl;
  } else {
    mass = m_OutputInfo.FittedMass[fitcode];
  }

  return mass;
}


int MissingMassCalculator::GetNTrials()
{
  return m_OutputInfo.NTrials;
}


int MissingMassCalculator::GetNSuccesses()
{
  return m_OutputInfo.NSuccesses;
}


int MissingMassCalculator::GetNSolutions()
{
  return m_OutputInfo.NSolutions;
}

// sum of weights of all solutions
double MissingMassCalculator::GetSumW()
{
  return m_OutputInfo.SumW;
}


// average RMS of solutions in one event
double MissingMassCalculator::GetAveSolRMS()
{
  return m_OutputInfo.AveSolRMS;
}


std::shared_ptr<TH1F> MissingMassCalculator::GetMassHistogram()
{
  return m_OutputInfo.hMfit_all;
}


std::shared_ptr<TH1F> MissingMassCalculator::GetMassHistogramNoWeight()
{
  return m_OutputInfo.hMfit_allNoWeight;
}

// returns neutrino 4-vec
TLorentzVector MissingMassCalculator::GetNeutrino4vec(int fitcode, int ind)
{
  TLorentzVector vec(0.0,0.0,0.0,0.0);
  if (fitcode!=MMCFitMethod::MAXW && fitcode!=MMCFitMethod::MLNU3P )
    {
      std::cout << "MissingMassCalculator::GetNeutrino4Vec ERROR ! fitcode=" << fitcode
                << "Should be either " << MMCFitMethod::MAXW << " or " << MMCFitMethod::MLNU3P  << std::endl;
    }
  else if (m_OutputInfo.FitStatus>0)
    {
      if(ind==0) vec = m_OutputInfo.nuvec1[fitcode];
      else if(ind==1) vec = m_OutputInfo.nuvec2[fitcode];
    }
  return vec;
}

// returns neutrino 4-vec
TLorentzVector MissingMassCalculator::GetTau4vec(int fitcode, int ind)
{
  TLorentzVector vec(0.0,0.0,0.0,0.0);
  if (fitcode!=MMCFitMethod::MAXW && fitcode!=MMCFitMethod::MLNU3P )
    {
      std::cout << "MissingMassCalculator::GetTau4vec ERROR ! fitcode=" << fitcode
                << "Should be either " << MMCFitMethod::MAXW << " or " << MMCFitMethod::MLNU3P  << std::endl;
    }
  else if (m_OutputInfo.FitStatus>0)
    {
      if(ind==0) vec = m_OutputInfo.objvec1[fitcode];
      else if(ind==1) vec = m_OutputInfo.objvec2[fitcode];
    }
  return vec;
}


// returns 4-vec for resonance
TLorentzVector MissingMassCalculator::GetResonanceVec(int fitcode) {
  TLorentzVector vec(0.0,0.0,0.0,0.0);
  if (fitcode!=MMCFitMethod::MAXW && fitcode!=MMCFitMethod::MLNU3P )
    {
      std::cout << "MissingMassCalculator::GetResonanceVec ERROR ! fitcode=" << fitcode
                << "Should be either " << MMCFitMethod::MAXW << " or " << MMCFitMethod::MLNU3P  << std::endl;
    }
  else if (m_OutputInfo.FitStatus>0)
    {
      vec = m_OutputInfo.objvec1[fitcode]+m_OutputInfo.objvec2[fitcode];
    }

  return vec;
}

// returns 2-vec for fitted MET
TVector2 MissingMassCalculator::GetFittedMetVec(int fitcode) {
  TVector2 vec(0.0,0.0);
  if (fitcode!=MMCFitMethod::MAXW && fitcode!=MMCFitMethod::MLNU3P )
    {
      std::cout << "MissingMassCalculator::GetFittedMetVec ERROR ! fitcode=" << fitcode
                << "Should be either " << MMCFitMethod::MAXW << " or " << MMCFitMethod::MLNU3P  << std::endl;
    }
  else if (m_OutputInfo.FitStatus>0)
    {
      vec=m_OutputInfo.FittedMetVec[fitcode];
    }

  return vec;
}

// finalizes output information
void MissingMassCalculator::DoOutputInfo()
{
  if(m_SearchMode==0) // di-tau mode
    {
      if(m_OutputInfo.FitStatus>0)
        {
          if(m_fUseVerbose == 1) { std::cout << "Retrieving output from fDitauStuffFit" << std::endl; }
          // MAXW method : get from fDittauStuffFit
          m_OutputInfo.FitSignificance[MMCFitMethod::MAXW] = m_fDitauStuffFit.Sign_best;
          m_OutputInfo.FittedMass[MMCFitMethod::MAXW] = m_fDitauStuffFit.Mditau_best;
          m_OutputInfo.nuvec1[MMCFitMethod::MAXW] = m_fDitauStuffFit.nutau1;
          m_OutputInfo.objvec1[MMCFitMethod::MAXW] = m_fDitauStuffFit.taufit1;
          m_OutputInfo.nuvec2[MMCFitMethod::MAXW] = m_fDitauStuffFit.nutau2;
          m_OutputInfo.objvec2[MMCFitMethod::MAXW] = m_fDitauStuffFit.taufit2;
          m_OutputInfo.totalvec[MMCFitMethod::MAXW] = m_OutputInfo.objvec1[MMCFitMethod::MAXW]+m_OutputInfo.objvec2[MMCFitMethod::MAXW];
          TVector2 metmaxw(m_OutputInfo.nuvec1[MMCFitMethod::MAXW].Px()+m_OutputInfo.nuvec2[MMCFitMethod::MAXW].Px(),m_OutputInfo.nuvec1[MMCFitMethod::MAXW].Py()+m_OutputInfo.nuvec2[MMCFitMethod::MAXW].Py());
          m_OutputInfo.FittedMetVec[MMCFitMethod::MAXW] = metmaxw;

          // MLM method : can only get MMC, rest is dummy
          double scale=MassScale(MMCFitMethod::MAXW,m_fDitauStuffHisto.Mditau_best,m_preparedInput.type_visTau1,m_preparedInput.type_visTau2); // only for histo method for now. In practice disabled by default

          m_OutputInfo.FittedMass[MMCFitMethod::MLM] = scale*m_fDitauStuffHisto.Mditau_best;

          TLorentzVector tlvdummy(0.,0.,0.,0.);
          TVector2 metdummy(0.,0.);
          m_OutputInfo.FitSignificance[MMCFitMethod::MLM] = -1.;
          m_OutputInfo.nuvec1[MMCFitMethod::MLM] = tlvdummy;
          m_OutputInfo.objvec1[MMCFitMethod::MLM] = tlvdummy;
          m_OutputInfo.nuvec2[MMCFitMethod::MLM] = tlvdummy;
          m_OutputInfo.objvec2[MMCFitMethod::MLM] = tlvdummy;
          m_OutputInfo.totalvec[MMCFitMethod::MLM] = tlvdummy;
          m_OutputInfo.FittedMetVec[MMCFitMethod::MLM] = metdummy;

          // MLNU3P method : get from fDittauStuffHisto 4 momentum
          m_OutputInfo.nuvec1[MMCFitMethod::MLNU3P] = m_fDitauStuffHisto.nutau1;
          m_OutputInfo.objvec1[MMCFitMethod::MLNU3P] = m_fDitauStuffHisto.taufit1;
          m_OutputInfo.nuvec2[MMCFitMethod::MLNU3P] = m_fDitauStuffHisto.nutau2;
          m_OutputInfo.objvec2[MMCFitMethod::MLNU3P] = m_fDitauStuffHisto.taufit2;
          m_OutputInfo.totalvec[MMCFitMethod::MLNU3P] = m_OutputInfo.objvec1[MMCFitMethod::MLNU3P] + m_OutputInfo.objvec2[MMCFitMethod::MLNU3P];
          m_OutputInfo.FittedMass[MMCFitMethod::MLNU3P] = m_OutputInfo.totalvec[MMCFitMethod::MLNU3P].M();

          TVector2 metmlnu3p(m_OutputInfo.nuvec1[MMCFitMethod::MLNU3P].Px()+m_OutputInfo.nuvec2[MMCFitMethod::MLNU3P].Px(),m_OutputInfo.nuvec1[MMCFitMethod::MLNU3P].Py()+m_OutputInfo.nuvec2[MMCFitMethod::MLNU3P].Py());
          m_OutputInfo.FittedMetVec[MMCFitMethod::MLNU3P] = metmlnu3p;

          m_OutputInfo.RMS2MPV = m_fDitauStuffHisto.RMSoverMPV;

        }
      // DR add a margin, given that due to binning effect, fitted mass can be slightly below the visible mass
      if(m_OutputInfo.FittedMass[MMCFitMethod::MLM]<=(m_preparedInput.vistau1+m_preparedInput.vistau2).M()-10.){
        ClearOutputStuff(); // to avoid cases when FitStatus=1 but mass is not reconstructed
      }

    }

  m_OutputInfo.hMfit_all = m_fMfit_all;
  m_OutputInfo.hMfit_allNoWeight = m_fMfit_allNoWeight;
  m_OutputInfo.NSolutions = m_fMfit_all->GetEntries();
  m_OutputInfo.SumW = m_fMfit_all->GetSumOfWeights();
  m_OutputInfo.UseHT = m_preparedInput.UseHT;

  //----------------- Check if input was re-ordered in FinalizeInputStuff() and restore the original order if needed
  if(m_InputReorder==1)
    {
      TLorentzVector dummy_vec1(0.0,0.0,0.0,0.0);
      TLorentzVector dummy_vec2(0.0,0.0,0.0,0.0);
      for(int i=0; i<3; i++)
        {
          // re-ordering neutrinos
          dummy_vec1=m_OutputInfo.nuvec1[i];
          dummy_vec2=m_OutputInfo.nuvec2[i];
          m_OutputInfo.nuvec1[i]=dummy_vec2;
          m_OutputInfo.nuvec2[i]=dummy_vec1;
          // re-ordering tau's
          dummy_vec1=m_OutputInfo.objvec1[i];
          dummy_vec2=m_OutputInfo.objvec2[i];
          m_OutputInfo.objvec1[i]=dummy_vec2;
          m_OutputInfo.objvec2[i]=dummy_vec1;
        }
    }

  return;
}


// Printout of final results
void MissingMassCalculator::PrintInputInfo(const InputInfoStuff & fStuff) {
  if(m_fUseVerbose!=1) return;

  std::cout << std::setprecision(15);
  std::cout<<" met_x="<<fStuff.MetVec.Px()<<" met_y="<<fStuff.MetVec.Py()<<" MET="<<fStuff.MetVec.Mod()<<" met_phi="<<fStuff.MetVec.Phi()<<std::endl;
  std::cout<<" detmet_x="<<fStuff.detMetVec.Px()<<" detmet_y="<<fStuff.detMetVec.Py()<<" detMET="<<fStuff.detMetVec.Mod()<<" detmet_phi="<<fStuff.detMetVec.Phi()<<std::endl;
  std::cout<<" sumEt="<<fStuff.m_SumEt<<" METsigmaP="<<fStuff.METsigmaP<<" METsigmaL="<<fStuff.METsigmaL<<" METcovphi="<<fStuff.METcovphi<<std::endl;
#ifdef  TAUESCAN
  std::cout<<" sigmaEtau1="<<fStuff.sigmaEtau1<<" sigmaEtau2="<< fStuff.sigmaEtau2 << std::endl ;
#endif
  std::cout << " Njet25=" << fStuff.m_Njet25 << " allowUseHT="<<fStuff.allowUseHT << " useHT="<<fStuff.UseHT << std::endl;

  std::cout << " MHtSigma1=" << fStuff.m_MHtSigma1 << " MHtSigma2="<< fStuff.m_MHtSigma2
            << " MHtGaussFr=" << fStuff.MHtGaussFr
            << " HtOffset=" << fStuff.HtOffset << std::endl;

  const TLorentzVector * origVisTau1=0;
  int origTauType1;

  const TLorentzVector * origVisTau2=0;
  int origTauType2;

  origVisTau1=&fStuff.vistau1;
  origVisTau2=&fStuff.vistau2;
  origTauType1=fStuff.type_visTau1;
  origTauType2=fStuff.type_visTau2;

  std::cout<<" 1st visible tau: type="<< origTauType1 << " Nprong="<< fStuff.Nprong_tau1 << "  P="<<origVisTau1->P()<<" Pt="<<origVisTau1->Pt()
           << " Eta="<<origVisTau1->Eta()<<" Phi="<<origVisTau1->Phi() << " M="<<origVisTau1->M()
           << std::endl;
  std::cout<<" 2nd visible tau: type="<< origTauType2 << " Nprong="<< fStuff.Nprong_tau2 << "  P="<<origVisTau2->P()<<" Pt="<<origVisTau2->Pt()
           << " Eta="<<origVisTau2->Eta()<<" Phi="<<origVisTau2->Phi() << " M="<<origVisTau2->M()
           << std::endl;

  if(fStuff.m_jet4vecs.size()>0)
    {
      for(unsigned int i=0; i<fStuff.m_jet4vecs.size(); i++)
        {
          std::cout<<" Printing jets: jet "<<i<<" E="<<fStuff.m_jet4vecs[i].E()<<" Pt="
                   <<fStuff.m_jet4vecs[i].Pt()<<" Phi="<<fStuff.m_jet4vecs[i].Phi()<<" Eta="<<fStuff.m_jet4vecs[i].Eta()<<std::endl;
        }
    }
  std::cout << std::setprecision(6);
  return;
}



// Printout of final results
void MissingMassCalculator::PrintOtherInput() {
  if(m_fUseVerbose!=1) return;


  std::cout << std::setprecision(15);
  std::cout<<".........................Other input....................................."<<std::endl;
  std::cout<<"   SearchMode="<<m_SearchMode<<"  -- 0: ditau, 1: WW, 2: W->taunu"<<std::endl;
  std::cout<<" Beam energy ="<<m_beamEnergy<<"  sqrt(S) for collisions ="<<2.0*m_beamEnergy<<std::endl;
  std::cout << " CalibrationSet " << MMCCalibrationSet::name[m_mmcCalibrationSet] << std::endl ;
  std::cout << " AlgorithmVersion " << m_AlgorithmVersion << " WalkStrategy=" << WALKSTRATEGY << " seed="<< m_seed << std::endl;
  std::cout << " usetauProbability=" << m_fUseTauProbability << " useTailCleanup=" << m_fUseTailCleanup << std::endl;

  if (m_InputReorder!=0)
    {
      std::cout << " tau1 and tau2 were internally swapped (visible on prepared input printout)" << std::endl;
    } else
    {
      std::cout << " tau1 and tau2 were NOT internally swapped" << std::endl;
    }

  std::cout << " MEtLMin="<<m_MEtLMin<< " MEtLMax="<<m_MEtLMax << std::endl;
  std::cout << " MEtPMin="<<m_MEtPMin<< " MEtPMax="<<m_MEtPMax << std::endl;
  std::cout << " Phi1Min="<<m_Phi1Min<< " Phi1Max="<<m_Phi1Max << std::endl;
  std::cout << " Phi2Min="<<m_Phi2Min<< " Phi2Max="<<m_Phi2Max << std::endl;
  std::cout << " Mnu1Min="<<m_Mnu1Min<< " Mnu1Max="<<m_Mnu1Max << std::endl;
  std::cout << " Mnu2Min="<<m_Mnu2Min<< " Mnu2Max="<<m_Mnu2Max << std::endl;
#ifdef TAUESCAN
  std::cout << " eTau1Min="<<m_eTau1Min<< " eTau1Max="<<m_eTau1Max <<  " range " << m_eTau1Range << " prop " << m_eTau1Proposal << std::endl;
  std::cout << " eTau2Min="<<m_eTau2Min<< " eTau2Max="<<m_eTau2Max <<  " range " << m_eTau2Range << " prop " << m_eTau2Proposal << std::endl;
#endif
}


// Printout of final results
void MissingMassCalculator::PrintResults() {

  if(m_fUseVerbose!=1) return;

  const TLorentzVector *origVisTau1=0;
  const TLorentzVector *origVisTau2=0;

  if(m_InputReorder==0)
    {
      origVisTau1=&m_preparedInput.vistau1;
      origVisTau2=&m_preparedInput.vistau2;
    }
  else // input order was flipped
    {
      origVisTau1=&m_preparedInput.vistau2;
      origVisTau2=&m_preparedInput.vistau1;
    }

  PrintOtherInput() ;

  std::cout << std::setprecision(6);
  std::cout<<"------------- Printing Final Results for MissingMassCalculator --------------"<<std::endl;
  std::cout<<"................................................................................."<<std::endl;
  std::cout<<"  Fit status="<<m_OutputInfo.FitStatus<<std::endl;


  for (int imeth=0; imeth<MMCFitMethod::MAX; ++imeth)
    {
      std::cout << "___  Results for " << MMCFitMethod::name[imeth] << "Method ___" << std::endl;
      std::cout << " signif=" << m_OutputInfo.FitSignificance[imeth] << std::endl;
      std::cout << " mass=" << m_OutputInfo.FittedMass[imeth] << std::endl;
      std::cout << " rms/mpv=" << m_OutputInfo.RMS2MPV << std::endl;

      if (imeth==MMCFitMethod::MLM)
        {
          std::cout << " no 4-momentum or MET from this method " << std::endl;
          continue;
        }

      if(m_OutputInfo.FitStatus<=0)
        {
          std::cout << " fit failed " << std::endl;
        }

      const TLorentzVector & tlvnu1 = m_OutputInfo.nuvec1[imeth];
      const TLorentzVector & tlvnu2 = m_OutputInfo.nuvec2[imeth];
      const TLorentzVector & tlvo1 = m_OutputInfo.objvec1[imeth];
      const TLorentzVector & tlvo2 = m_OutputInfo.objvec2[imeth];
      const TVector2 & tvmet = m_OutputInfo.FittedMetVec[imeth];


      std::cout << " Neutrino-1: P=" << tlvnu1.P() << "  Pt=" << tlvnu1.Pt() << "  Eta=" << tlvnu1.Eta()  << "  Phi=" << tlvnu1.Phi() << "  M=" << tlvnu1.M()
                <<  " Px=" << tlvnu1.Px() << " Py=" << tlvnu1.Py() << " Pz=" << tlvnu1.Pz() << std::endl;
      std::cout << " Neutrino-2: P=" << tlvnu2.P() << "  Pt=" << tlvnu2.Pt() << "  Eta=" << tlvnu2.Eta()  << "  Phi=" << tlvnu2.Phi() << "  M=" << tlvnu2.M()
                <<  " Px=" << tlvnu2.Px() << " Py=" << tlvnu2.Py() << " Pz=" << tlvnu2.Pz() << std::endl;
      std::cout << " Tau-1: P=" << tlvo1.P() << "  Pt=" << tlvo1.Pt() << "  Eta=" << tlvo1.Eta()  << "  Phi=" << tlvo1.Phi() << "  M=" << tlvo1.M()
                <<  " Px=" << tlvo1.Px() << " Py=" << tlvo1.Py() << " Pz=" << tlvo1.Pz() << std::endl;
      std::cout << " Tau-2: P=" << tlvo2.P() << "  Pt=" << tlvo2.Pt() << "  Eta=" << tlvo2.Eta()  << "  Phi=" << tlvo2.Phi() << "  M=" << tlvo2.M()
                <<  " Px=" << tlvo2.Px() << " Py=" << tlvo2.Py() << " Pz=" << tlvo2.Pz() << std::endl;

      if(m_SearchMode==0)
        {
          std::cout << " dR(nu1-visTau1)=" << tlvnu1.DeltaR(*origVisTau1) << std::endl;
          std::cout << " dR(nu2-visTau2)=" << tlvnu2.DeltaR(*origVisTau2) << std::endl;
        }

      std::cout << " Fitted MET =" <<  tvmet.Mod() << "  Phi=" << tlvnu1.Phi()
                <<  " Px=" << tvmet.Px() << " Py=" << tvmet.Py() << std::endl;

      std::cout << " Resonance: P=" << m_OutputInfo.totalvec[imeth].P() << "  Pt=" << m_OutputInfo.totalvec[imeth].Pt()
                << " Eta=" << m_OutputInfo.totalvec[imeth].Eta()
                << " Phi=" << m_OutputInfo.totalvec[imeth].Phi()
                << " M=" << m_OutputInfo.totalvec[imeth].M()
                << " Px=" << m_OutputInfo.totalvec[imeth].Px()
                << " Py=" << m_OutputInfo.totalvec[imeth].Py()
                << " Pz=" << m_OutputInfo.totalvec[imeth].Pz() << std::endl;

    }


  return;
}

double MissingMassCalculator::mT(const TLorentzVector & vec,const TVector2 & met_vec) {
  double mt=0.0;
  double dphi=std::abs(TVector2::Phi_mpi_pi(vec.Phi()-met_vec.Phi()));
  double cphi=1.0-cos(dphi);
  if(cphi>0.0) mt=sqrt(2.0*vec.Pt()*met_vec.Mod()*cphi);
  return mt;
}

// returns P(nu1) & P(nu2)
int MissingMassCalculator::NuPsolution(TVector2 met_vec, double theta1, double phi1,
                                       double theta2, double phi2, double &P1, double &P2) {
  int solution_code=0; // 0== no solution, 1==with solution
  P1=0.0;
  P2=0.0;
  double D=sin(theta1)*sin(theta2)*sin(phi2-phi1);
  if(std::abs(D)>0.0) // matrix deteriminant is non-zero
    {
      P1=(met_vec.Px()*sin(phi2)-met_vec.Py()*cos(phi2))*sin(theta2)/D;
      P2=(met_vec.Py()*cos(phi1)-met_vec.Px()*sin(phi1))*sin(theta1)/D;
      if(P1>0.0 && P2>0.0) solution_code=1;
    }
  return solution_code;
}



// returns P1, P2, and theta1 & theta2 solutions
// This compute the nu1 nu2 solution in the most efficient way. Wrt to NuPsolutionV2, the output nu1 nu2 4-vector have non zero mass (if relevant).
// It is not optimised for grid running so much less caching is done (which makes it more readable). Only quantities fixed within an event are cached.
// relies on a number of these variables to be initialised before the loop.


int MissingMassCalculator::NuPsolutionV3(const double & mNu1, const double & mNu2, const double & phi1, const double & phi2, int & nsol1, int & nsol2) {



  // Pv1, Pv2 : visible tau decay product momentum
  // Pn1 Pn2 : neutrino momentum
  // phi1, phi2 : neutrino azymutal angles
  // PTmiss2=PTmissy Cos[phi2] - PTmissx Sin[phi2]
  // PTmiss2cscdphi=PTmiss2/Sin[phi1-phi2]
  // Pv1proj=Pv1x Cos[phi1] + Pv1y Sin[phi1]
  // M2noma1=Mtau^2-Mv1^2-Mn1^2
  // ETv1^2=Ev1^2-Pv1z^2

  //discriminant : 16 Ev1^2 (M2noma1^2 + 4 M2noma1 PTmiss2cscdphi Pv1proj -  4 (ETv1^2 (Mn1^2 + PTmiss2cscdphi^2) - PTmiss2cscdphi^2 Pv1proj^2))
  // two solutions for epsilon = +/-1
  //Pn1z=(1/(2 ETv1^2))(epsilon Ev1 Sqrt[ M2noma1^2 + 4 M2noma1 PTmiss2cscdphi Pv1proj - 4 (ETv1^2 (Mn1^2 + qPTmiss2cscdphi^2) - PTmiss2cscdphi^2 Pv1proj^2)] + M2noma1 Pv1z + 2 PTmiss2cscdphi Pv1proj Pv1z)
  // with conditions:  M2noma1 + 2 PTmiss2cscdphi Pv1proj + 2 Pn1z Pv1z > 0
  // PTn1 -> PTmiss2 Csc[phi1 - phi2]

  // if initialisation precompute some quantities
  int solution_code=0; // 0 with no solution, 1 with solution
  nsol1=0;
  nsol2=0;

  // Variables used to test PTn1 and PTn2 > 0

  const double & pTmissx=m_MEtX;
  const double & pTmissy=m_MEtY;

  fastSinCos(phi2,m_sinPhi2,m_cosPhi2);
  double pTmiss2=pTmissy*m_cosPhi2 - pTmissx*m_sinPhi2;

  int dPhiSign=0;
  dPhiSign= fixPhiRange(phi1-phi2)>0 ? +1 : -1;

  // Test if PTn1 and PTn2 > 0. Then MET vector is between the two neutrino vector


  if(pTmiss2*dPhiSign < 0)
    {
      ++m_testptn1;
      return solution_code;
    }

  fastSinCos(phi1,m_sinPhi1,m_cosPhi1);
  double pTmiss1=pTmissy*m_cosPhi1 - pTmissx*m_sinPhi1;

  if(pTmiss1*(-dPhiSign) < 0)
    {
      ++m_testptn2;
      return solution_code;
    }


  // Variables used to calculate discri1

  double m2Vis1=m_tauVec1M*m_tauVec1M;
  m_ET2v1=std::pow(m_tauVec1E,2)-std::pow(m_tauVec1Pz,2);
  m_m2Nu1=mNu1*mNu1;
  double m2noma1=m_mTau2-m_m2Nu1-m2Vis1;
  double m4noma1=m2noma1*m2noma1;
  double pv1proj=m_tauVec1Px *m_cosPhi1 + m_tauVec1Py *m_sinPhi1;
  double p2v1proj=std::pow(pv1proj,2);
  double sinDPhi2=m_cosPhi2*m_sinPhi1-m_sinPhi2*m_cosPhi1; // sin(Phi1-Phi2)
  double pTmiss2CscDPhi=pTmiss2/sinDPhi2;
  double& pTn1=pTmiss2CscDPhi;
  double pT2miss2CscDPhi=pTmiss2CscDPhi*pTmiss2CscDPhi;
#ifdef DEBUG_NUPSOLV3
  double sin2DPhi2=sinDPhi2*sinDPhi2;
#endif

  // Test on discri1
  const double discri1= m4noma1 + 4*m2noma1*pTmiss2CscDPhi*pv1proj - 4*( m_ET2v1*(m_m2Nu1 + pT2miss2CscDPhi) - (pT2miss2CscDPhi*p2v1proj) );

  if (discri1<0) // discriminant negative -> no solution
    {
      ++m_testdiscri1;
      return solution_code;
    }

  //Variables used to calculate discri2
  double m2Vis2=m_tauVec2M*m_tauVec2M;
  m_ET2v2=std::pow(m_tauVec2E,2)-std::pow(m_tauVec2Pz,2);
  m_m2Nu2=mNu2*mNu2;
  double m2noma2=m_mTau2-m_m2Nu2-m2Vis2;
  double m4noma2=m2noma2*m2noma2;
  double pv2proj=m_tauVec2Px *m_cosPhi2 + m_tauVec2Py *m_sinPhi2;
  double p2v2proj=std::pow(pv2proj,2);
  double sinDPhi1=-sinDPhi2;
  double pTmiss1CscDPhi=pTmiss1/sinDPhi1;
  double& pTn2=pTmiss1CscDPhi;
  double pT2miss1CscDPhi=pTmiss1CscDPhi*pTmiss1CscDPhi;
#ifdef DEBUG_NUPSOLV3
  double sin2DPhi1=sinDPhi1*sinDPhi1;
#endif

  const double discri2= m4noma2 + 4* m2noma2* pTmiss1CscDPhi* pv2proj - 4* (m_ET2v2 * (m_m2Nu2 + pT2miss1CscDPhi) - (pT2miss1CscDPhi * p2v2proj) );

  if (discri2<0) // discriminant negative -> no solution
    {
      ++m_testdiscri2;
      return solution_code;
    }

  // this should be done only once we know there are solutions for nu2
  m_E2v1 = m_tauVec1Pz * m_tauVec1Pz + m_ET2v1;
  m_Ev1 = sqrt(m_E2v1);
#ifndef USEAPPROXSOLUTION
  double sqdiscri1=sqrt(discri1);
  double first1=(m2noma1*m_tauVec1Pz + 2*pTmiss2CscDPhi*pv1proj*m_tauVec1Pz) / (2*m_ET2v1);
  double second1=sqdiscri1*m_Ev1/( 2*m_ET2v1 );


  // first solution
  double pn1Z=first1+second1;

  if(m2noma1 + 2 * pTmiss2CscDPhi * pv1proj + 2 * pn1Z * m_tauVec1Pz >0) // Condition for solution to exist
    {
      m_nuvecsol1[nsol1].SetPxPyPzE(pTn1 * m_cosPhi1, pTn1 * m_sinPhi1, pn1Z, sqrt(std::pow(pTn1,2) + std::pow(pn1Z,2) + m_m2Nu1));

#ifdef DEBUG_NUPSOLV3

      std::cout << "" << std::endl;
      std::cout << "/*---------- Solution Neutrino 1 ---------*/" << std::endl;
      std::cout << "First solution" << std::endl;
      double mtau1plus=( m_nuvecsol1[nsol1]+m_tauVec1 ).M();
      std::cout << "Mtau1+ = " << mtau1plus << std::endl;
      double verif=-m4noma1-4*m2noma1*pTmiss2CscDPhi*pv1proj+4*(m_E2v1*(m_m2Nu1+std::pow(pTmiss2CscDPhi,2))-std::pow(pTmiss2CscDPhi,2)*p2v1proj)-4*pn1Z*(m2noma1+2*pTmiss2CscDPhi*pv1proj)*m_tauVec1Pz+4*std::pow(pn1Z,2)*(std::pow(m_Ev1,2)-std::pow(m_tauVec1Pz,2));
      std::cout << "Test if null : " << verif << std::endl;
      double verif2=4*m_E2v1*(m_m2Nu1+std::pow(pn1Z,2)+std::pow(pTmiss2,2)/sin2DPhi2) - std::pow(m_mTau2-m_m2Nu1-m2Vis1+2*pn1Z*m_tauVec1Pz+2*pTmiss2*pv1proj/sinDPhi2,2);
      std::cout << "Test if null earlier : " << verif2 << std::endl;
      double verif3=m_m2Nu1+m2Vis1+2*m_Ev1*sqrt(m_m2Nu1+pn1Z*pn1Z+pTmiss2*pTmiss2/sin2DPhi2) - (m_mTau2+2*pn1Z*m_tauVec1Pz+2*pTmiss2*pv1proj/sinDPhi2);
      std::cout << "Test if null at the begining : " << verif3 << std::endl;
      double m2taueq1c = -m4noma1 - 4*m2noma1*pTmiss2CscDPhi*pv1proj + 4*(std::pow(m_Ev1,2)*(m_m2Nu1 + std::pow(pTmiss2CscDPhi,2)) - std::pow(pTmiss2CscDPhi,2)*p2v1proj) - 4*pn1Z*(m2noma1 + 2*pTmiss2CscDPhi*pv1proj)*m_tauVec1Pz + 4*std::pow(pn1Z,2)*(std::pow(m_Ev1,2) - std::pow(m_tauVec1Pz,2));
      std::cout << "Test m2taueq1c = " << m2taueq1c << std::endl;
      std::cout << "" << std::endl;
      std::cout << "-------------------------" << std::endl;
      std::cout << "Valeurs intermediaires" << std::endl;
      std::cout << "" << std::endl;
      std::cout << "Discri = " << discri1 << std::endl;
      std::cout << "sqDiscri = " << sqdiscri1 << std::endl;
      std::cout << "Pv1x = " << m_tauVec1Px << std::endl;
      std::cout << "Pv1y = " << m_tauVec1Py << std::endl;
      std::cout << "Pv1z = " << m_tauVec1Pz << std::endl;
      std::cout << "Mv1 = " << sqrt(m2Vis1) << std::endl;
      std::cout << "M2v1 = " << m2Vis1 << std::endl;
      std::cout << "PTmissx = " << pTmissx << std::endl;
      std::cout << "PTmissy = " << pTmissy << std::endl;
      std::cout << "m_MNu1 = " << mNu1 << std::endl;
      std::cout << "m_M2Nu1 = " << m_m2Nu1 << std::endl;
      std::cout << "Ev1 = " << m_Ev1 << std::endl;
      std::cout << "ETv1 = " << sqrt(m_ET2v1) << std::endl;
      std::cout << "Pv1proj = " << pv1proj << std::endl;
      std::cout << "M2noma1 = " << m2noma1 << std::endl;
      std::cout << "PTn1 = " << pTn1 << std::endl;

#endif
      ++nsol1;

    }

  pn1Z=first1-second1;

  if (m2noma1 + 2 * pTmiss2CscDPhi * pv1proj + 2 * pn1Z * m_tauVec1Pz >0) // Condition for solution to exist
    {

      m_nuvecsol1[nsol1].SetPxPyPzE(pTn1 * m_cosPhi1, pTn1 * m_sinPhi1, pn1Z,sqrt( std::pow(pTn1,2) + std::pow(pn1Z,2) + m_m2Nu1 )) ;

#ifdef DEBUG_NUPSOLV3
      double mtau1moins=( m_nuvecsol1[nsol1]+m_tauVec1 ).M();
      std::cout << "M2tau1- = " << mtau1moins << std::endl;
      double verif=-m4noma1 - 4*m2noma1*pTn1*pv1proj + 4*(m_Ev1*m_Ev1*(m_m2Nu1+std::pow(pTn1,2))-std::pow(pTn1,2)*p2v1proj)-4*pn1Z*(m2noma1+2*pTn1*pv1proj)*m_tauVec1Pz+4*pn1Z*pn1Z*(m_Ev1*m_Ev1-m_tauVec1Pz*m_tauVec1Pz);
      std::cout << "Test if null : " << verif << std::endl;
      double verif2=4*m_Ev1*m_Ev1*(m_m2Nu1+pn1Z*pn1Z+(pTmiss2*pTmiss2/sin2DPhi2)) - std::pow(m_mTau2-m_m2Nu1-m2Vis1+2*pn1Z*m_tauVec1Pz+2*pTmiss2*pv1proj/sinDPhi2,2);
      std::cout << "Test if null earlier : " << verif2 << std::endl;
      double verif3=m_m2Nu1+m2Vis1+2*m_Ev1*sqrt(m_m2Nu1+pn1Z*pn1Z+(pTmiss2*pTmiss2/sin2DPhi2)) - (m_mTau2+2*pn1Z*m_tauVec1Pz+(2*pTmiss2*pv1proj/sinDPhi2));
      std::cout << "Test if null at the begining : " << verif3 << std::endl;
      double m2taueq1c = -m4noma1 - 4*m2noma1*pTmiss2CscDPhi*pv1proj + 4*(std::pow(m_Ev1,2)*(m_m2Nu1 + std::pow(pTmiss2CscDPhi,2)) - std::pow(pTmiss2CscDPhi,2)*p2v1proj) - 4*pn1Z*(m2noma1 + 2*pTmiss2CscDPhi*pv1proj)*m_tauVec1Pz + 4*std::pow(pn1Z,2)*(std::pow(m_Ev1,2) - std::pow(m_tauVec1Pz,2));

      std::cout << "Test m2taueq1c = " << m2taueq1c << std::endl;
      std::cout << "Phi1 = " << phi1 << std::endl;
      std::cout << "Phi2 = " << phi2 << std::endl;
      std::cout << "Pn1Z = " << pn1Z << std::endl;
#endif
      ++nsol1;

    }

  if(nsol1==0)
    {
      return solution_code;
    }

#else // else of USEAPPROXSOLUTION

  // If discri1/2a <<<< 1, we can admit that solution is Pn1Z=first

  double first1=(m2noma1*m_tauVec1Pz + 2*pTmiss2CscDPhi*pv1proj*m_tauVec1Pz) / (2*m_ET2v1);
  double pn1Z=first1;
  if (m2noma1 + 2 * pTmiss2CscDPhi * pv1proj + 2 * pn1Z * m_tauVec1Pz >0) // Condition for solution to exist
    {
      nuvecsol1[nsol1].SetPxPyPzE(pTn1 * m_cosPhi1, pTn1 * m_sinPhi1, pn1Z, sqrt(std::pow(pTn1,2) + std::pow(pn1Z,2) + m_m2Nu1));
      ++nsol1;
    }

  if (nsol1==0) return solution_code;

#endif // endif of USEAPPROXSOLUTION

  m_E2v2 = m_tauVec2Pz * m_tauVec2Pz + m_ET2v2;
  m_Ev2 = sqrt(m_E2v2);
#ifndef USEAPPROXSOLUTION
  double sqdiscri2=sqrt(discri2);
  double first2=(m2noma2*m_tauVec2Pz + 2*pTmiss1CscDPhi*pv2proj*m_tauVec2Pz) / (2*m_ET2v2);
  double second2=sqdiscri2*m_Ev2/(2*m_ET2v2);

  // second solution
  double pn2Z=first2+second2;

  if (m2noma2 + 2 * pTmiss1CscDPhi * pv2proj + 2 * pn2Z * m_tauVec2Pz >0) // Condition for solution to exist
    {
      m_nuvecsol2[nsol2].SetPxPyPzE(pTn2 * m_cosPhi2, pTn2 * m_sinPhi2, pn2Z, sqrt( std::pow(pTn2,2) + std::pow(pn2Z,2) + m_m2Nu2 ));

#ifdef DEBUG_NUPSOLV3
      std::cout << "" << std::endl;
      std::cout << "/*---------- Solution Neutrino 2 ---------*/" << std::endl;
      std::cout << "First Solution" << std::endl;
      double mtau2plus=( m_nuvecsol2[nsol2]+tau_vec2 ).M();
      std::cout << "Mtau2+ = " << mtau2plus << std::endl;
      std::cout << "m_MNu2 = " << mNu2 << std::endl;
      double verif=-m4noma2-4*m2noma2*pTn2*pv2proj+4*(m_Ev2*m_Ev2*(m_m2Nu2+std::pow(pTn2,2))-std::pow(pTn2,2)*p2v2proj)-4*pn2Z*(m2noma2+2*pTn2*pv2proj)*m_tauVec2Pz+4*pn2Z*pn2Z*(m_Ev2*m_Ev2-m_tauVec2Pz*m_tauVec2Pz);
      std::cout << "Test if null : " << verif << std::endl;
      double verif2=4*m_Ev2*m_Ev2*(m_m2Nu2+pn2Z*pn2Z+pTmiss1*pTmiss1/sin2DPhi1) - std::pow(m_mTau2-m_m2Nu2-m2Vis2+2*pn2Z*m_tauVec2Pz+2*pTmiss1*pv2proj/sinDPhi1,2);
      std::cout << "Test if null earlier : " << verif2 << std::endl;
      double verif3=m_m2Nu2+m2Vis2+2*m_Ev2*sqrt(m_m2Nu2+pn2Z*pn2Z+pTmiss1*pTmiss1/sin2DPhi1) - (m_mTau2+2*pn2Z*m_tauVec2Pz+2*pTmiss1*pv2proj/sinDPhi1);
      std::cout << "Test if null at the begining : " << verif3 << std::endl;
      double m2taueq2c = -m4noma2 - 4*m2noma2*pTmiss1CscDPhi*pv2proj + 4*(std::pow(m_Ev2,2)*(m_m2Nu2 + std::pow(pTmiss1CscDPhi,2)) - std::pow(pTmiss1CscDPhi,2)*p2v2proj) - 4*pn2Z*(m2noma2 + 2*pTmiss1CscDPhi*pv2proj)*m_tauVec2Pz + 4*std::pow(pn2Z,2)*(std::pow(m_Ev2,2) - std::pow(m_tauVec2Pz,2));
      std::cout << "Test m2taueq2c = " << m2taueq2c << std::endl;
      std::cout << "Phi1 = " << phi1 << std::endl;
      std::cout << "Phi2 = " << phi2 << std::endl;
      std::cout << "-------------------------" << std::endl;

#endif
      ++nsol2;

    }

  pn2Z=first2-second2;;

  if (m2noma2 + 2*pTmiss1CscDPhi*pv2proj + 2*pn2Z*m_tauVec2Pz >0) // Condition for solution to exist
    {
      m_nuvecsol2[nsol2].SetPxPyPzE(pTn2 * m_cosPhi2, pTn2 * m_sinPhi2, pn2Z,sqrt( std::pow(pTn2,2) + std::pow(pn2Z,2) + m_m2Nu2 ));

#ifdef DEBUG_NUPSOLV3
      std::cout << "Second Solution" << std::endl;
      double mtau2moins=( m_nuvecsol2[nsol2]+tau_vec2 ).M();
      std::cout << "M2tau2- = " << mtau2moins << std::endl;
      std::cout << "m_MNu2 = " << mNu2 << std::endl;
      double verif=-m4noma2-4*m2noma2*pTn2*pv2proj+4*(m_Ev2*m_Ev2*(m_m2Nu2+std::pow(pTn2,2))-std::pow(pTn2,2)*p2v2proj)-4*pn2Z*(m2noma2+2*pTn2*pv2proj)*m_tauVec2Pz+4*pn2Z*pn2Z*(m_Ev2*m_Ev2-m_tauVec2Pz*m_tauVec2Pz);
      std::cout << "Test if null : " << verif << std::endl;
      double verif2=4*m_Ev2*m_Ev2*(m_m2Nu2+pn2Z*pn2Z+pTmiss1*pTmiss1/sin2DPhi1) - std::pow(m_mTau2-m_m2Nu2-m2Vis2+2*pn2Z*m_tauVec2Pz+2*pTmiss1*pv2proj/sinDPhi1,2);
      std::cout << "Test if null earlier : " << verif2 << std::endl;
      double verif3=m_m2Nu2+m2Vis2+2*m_Ev2*sqrt(m_m2Nu2+pn2Z*pn2Z+pTmiss1*pTmiss1/sin2DPhi1) - (m_mTau2+2*pn2Z*m_tauVec2Pz+2*pTmiss1*pv2proj/sinDPhi1);
      std::cout << "Test if null at the begining : " << verif3 << std::endl;
      double m2taueq2c = -m4noma2 - 4*m2noma2*pTmiss1CscDPhi*pv2proj + 4*(std::pow(m_Ev2,2)*(m_m2Nu2 + std::pow(pTmiss1CscDPhi,2)) - std::pow(pTmiss1CscDPhi,2)*p2v2proj) - 4*pn2Z*(m2noma2 + 2*pTmiss1CscDPhi*pv2proj)*m_tauVec2Pz + 4*std::pow(pn2Z,2)*(std::pow(m_Ev2,2) - std::pow(m_tauVec2Pz,2));
      std::cout << "Test m2taueq2c = " << m2taueq2c << std::endl;
      std::cout << "Phi1 = " << phi1 << std::endl;
      std::cout << "Phi2 = " << phi2 << std::endl;
      std::cout << "-------------------------" << std::endl;

#endif
      ++nsol2;

    }

  if(nsol2==0)
    {
      return solution_code;
    }

#else // else of ifndef USEAPPROXSOLUTION

  // If discri2 <<<< 1, we can admit that solution is Pn2Z=first
  double first2=(m2noma2*m_tauVec2Pz + 2*pTmiss1CscDPhi*pv2proj*m_tauVec2Pz) / (2*m_ET2v2);
  double pn2Z=first2;
  if(m2noma2 + 2 * pTmiss1CscDPhi * pv2proj + 2 * pn2Z * m_tauVec2Pz >0) // Condition for solution to exist
    {
      m_nuvecsol2[nsol2].SetPxPyPzE(pTn2 * m_cosPhi2, pTn2 * m_sinPhi2, pn2Z, sqrt( std::pow(pTn2,2) + std::pow(pn2Z,2) + m_m2Nu2 ));
      ++nsol2;
    }

  if(nsol2==0) return solution_code;

#endif // end else ifndef USEAPPROXSOLUTION

  //Verification if solution exist


  solution_code=1;
  ++m_iterNuPV3;

  // double check solutions from time to time
  if (m_iterNuPV3 % 1000 ==1)
    {
      double pnux = m_nuvecsol1[0].Px() + m_nuvecsol2[0].Px();
      double pnuy = m_nuvecsol1[0].Py() + m_nuvecsol2[0].Py();
      double mtau1plus=( m_nuvecsol1[0]+m_tauVec1 ).M();
      double mtau1moins=( m_nuvecsol1[1]+m_tauVec1 ).M();
      double mtau2plus=( m_nuvecsol2[0]+m_tauVec2 ).M();
      double mtau2moins=( m_nuvecsol2[1]+m_tauVec2 ).M();
#ifdef DEBUG_NUPSOLV3
      std::cout << "/*---------- Solution Neutrino 1 ---------*/" << std::endl;

      std::cout.precision(16);
      std::cout << "PTn1 * cos(Phi) = " << pTn1*m_cosPhi1 << std::endl;
      std::cout << "PTn1 * sin(Phi) = " << pTn1*m_sinPhi1 << std::endl;
      std::cout << "cos(Phi1) = " << m_cosPhi1 << std::endl;
      std::cout << "sin(Phi1) = " << m_sinPhi1 << std::endl;
      std::cout << "Phi1 = " << phi1 << std::endl;
      std::cout << "Phi2 = " << phi2 << std::endl;
      std::cout << "PTmiss2 = " << pTmiss2 << std::endl;
      std::cout << "PTmiss2CscDPhi = " << pTmiss2CscDPhi << std::endl;
      std::cout << "FIRST SOLUTION" << std::endl;
      std::cout << " Pn1Z_1 = " << first1+second1 << std::endl;
      std::cout << "SECOND SOLUTION" << std::endl;
      std::cout << " Pn1Z_2 = " << first1-second1 << std::endl;
      std::cout << "" << std::endl;
      std::cout << "Approx sol -b/2a : " << first1 << std::endl;
      std::cout << "" << std::endl;

      std::cout << "/*---------- Solution Neutrino 2 ---------*/" << std::endl;

      std::cout << "PTn2 * cos(Phi) = " << pTn2*m_cosPhi2 << std::endl;
      std::cout << "PTn2 * sin(Phi) = " << pTn2*m_sinPhi2 << std::endl;
      std::cout << "cos(Phi2) = " << m_cosPhi2 << std::endl;
      std::cout << "sin(Phi2) = " << m_sinPhi2 << std::endl;
      std::cout << "Phi1 = " << phi1 << std::endl;
      std::cout << "Phi2 = " << phi2 << std::endl;
      std::cout << "PTmiss1 = " << pTmiss1 << std::endl;
      std::cout << "PTmiss1CscDPhi = " << pTmiss1CscDPhi << std::endl;
      std::cout << "FIRST SOLUTION" << std::endl;
      std::cout << " Pn2Z_1 = " << first2+second2 << std::endl;
      std::cout << "SECOND SOLUTION" << std::endl;
      std::cout << " Pn2Z_2 = " << first2-second2 << std::endl;
      std::cout << "" << std::endl;
      std::cout << "Approx sol -b/2a : " << first2 << std::endl;
      std::cout << "" << std::endl;

      std::cout << "TESTS IF SOL OK" << std::endl;
      std::cout << "Pnux = " << pnux << std::endl;
      std::cout << "Pnuy = " << pnuy << std::endl;
      std::cout << "PTmissx = " << pTmissx << std::endl;
      std::cout << "PTmissy = " << pTmissy << std::endl;
      std::cout << "Pnux - Met.X = " << pnux - met_vec.Px() << std::endl;
      std::cout << "Pnuy - Met.Y = " << pnuy - met_vec.Py() << std::endl;
      std::cout << "Mtau1+ = " << mtau1plus << std::endl;
      std::cout << "Mtau1- = " << mtau1moins << std::endl;
      std::cout << "Mtau2+ = " << mtau2plus << std::endl;
      std::cout << "Mtau2- = " << mtau2moins << std::endl;
      std::cout << "" << std::endl;
      std::cout << m_iterNuPV3 << std::endl;
      std::cout << "" << std::endl;
#endif
      if (std::abs(pnux - pTmissx) > 0.001 || std::abs(pnuy - pTmissy) > 0.001)
        {
          std::cout << "NuPsolutionV3 ERROR Pnux-Met.X or Pnuy-Met.Y > 0.001 :" << " "  << pnux - pTmissx << " and " << pnuy - pTmissx << " " << "Invalid solutions" << std::endl;
          std::cout << "" << std::endl;
        }
      if (std::abs(mtau1plus - m_mTau) > 0.001 || std::abs(mtau1moins - m_mTau) > 0.001 || std::abs(mtau2plus - m_mTau) > 0.001 || std::abs(mtau2moins - m_mTau) > 0.001)
        {
          std::cout << "NuPsolutionV3 ERROR tau mass not recovered : " << " " << mtau1plus << " " << mtau1moins << " " << mtau2plus << " " << mtau2moins << std::endl;
          std::cout << "" << std::endl;
        }

    }


  return solution_code;
}


double MissingMassCalculator::tauSigmaE(const TLorentzVector & tauVec, const int tauType ) {
#ifdef TAUESCAN
  // do nothing for lepton (note that one could easily scan electron energy)
  if (tauType==0) return -1;
  // FIXME trivial function to get started. Need proper parameterisation
  //   return 0.3*sqrt(tauVec.E());
  // TERSimpleSigma returns the relative sigma on pT. While we need the absolute error on energy
  // from pT to p=> sin(theta) disappear in the ratio
  // from p to E : here we neglect the mass, effect should be very small
  return TERSimpleSigma(tauType,tauVec)*tauVec.E();
#else
  double dummy=0.*tauVec.E()*tauType; // avoid warning
  return -1+dummy;
#endif
  return -1;
}


// returns solution for Lepton Flavor Violating X->lep+tau studies
int MissingMassCalculator::NuPsolutionLFV(const TVector2 & met_vec, const TLorentzVector & tau,
                                          const double & l_nu, std::vector<TLorentzVector> &nu_vec) {
  int solution_code=0; // 0 with no solution, 1 with solution

  nu_vec.clear();
  TLorentzVector nu(0.0,0.0,0.0,0.0);
  TLorentzVector nu2(0.0,0.0,0.0,0.0);
  nu.SetXYZM(met_vec.Px(),met_vec.Py(),0.0,l_nu);
  nu2.SetXYZM(met_vec.Px(),met_vec.Py(),0.0,l_nu);

  const double Mtau = 1.777;
  //   double msq = (Mtau*Mtau-tau.M()*tau.M())/2;
  double msq = (Mtau*Mtau-tau.M()*tau.M()-l_nu*l_nu)/2; // to take into account the fact that 2-nu systema has mass
  double gamma = nu.Px()*nu.Px()+nu.Py()*nu.Py();
  double beta = tau.Px()*nu.Px() + tau.Py()*nu.Py() + msq;
  double a = tau.E()*tau.E() - tau.Pz()*tau.Pz();
  double b = -2*tau.Pz()*beta;
  double c = tau.E()*tau.E()*gamma-beta*beta;
  if((b*b-4*a*c) < 0) return solution_code; // no solution found
  else solution_code=2;
  double pvz1 = (-b+sqrt(b*b-4*a*c))/(2*a);
  double pvz2 = (-b-sqrt(b*b-4*a*c))/(2*a);
  nu.SetXYZM(met_vec.Px(),met_vec.Py(),pvz1,l_nu);
  nu2.SetXYZM(met_vec.Px(),met_vec.Py(),pvz2,l_nu);
  nu_vec.push_back(nu);
  nu_vec.push_back(nu2);
  return solution_code;
}


// returns parameters for dTheta3D pdf
double MissingMassCalculator::dTheta3Dparam(const int & parInd, const int & tau_type, const double & P_tau, const double *par) {
  //tau_type 0: l, 1:1-prong, 3:3-prong
  if(P_tau<0.0) return 0.0;


  if(parInd==0) {
    if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2011){
      return (par[0]+par[1]*P_tau)*0.00125;
    }
    else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2012
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C
             || m_mmcCalibrationSet==MMCCalibrationSet::UPGRADE
             || m_mmcCalibrationSet==MMCCalibrationSet::LFVMMC2012){
      return (par[0]+par[1]*P_tau+par[2]*pow(P_tau,2)+par[3]*pow(P_tau,3)+par[4]*pow(P_tau,4))*0.00125;
    }
  }
  else { // parInd==0
    if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2011){
      return par[0]*(exp(-par[1]*P_tau)+par[2]/P_tau)+par[3];
    }
    else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2012
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS
             || m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C
             || m_mmcCalibrationSet==MMCCalibrationSet::UPGRADE
             || m_mmcCalibrationSet==MMCCalibrationSet::LFVMMC2012){
      if(tau_type==0) return par[0]*(exp(-par[1]*P_tau)+par[2]/P_tau)+par[3]+par[4]*P_tau;
      else return par[0]*(exp(-par[1]*sqrt(P_tau))+par[2]/P_tau)+par[3]+par[4]*P_tau;
    }
  }
  return 0.;
}


//SpeedUp static instantation
// first index is the calibration set : 0: MMC2011, 1:MMC2012
// second index is the decay 0 : lepton, 1 : 1 prong, 2 3 prong
double MissingMassCalculator::s_fit_param[2][3][6][5];
// first parameter: 0- for 1-prong; 1- for 3-prong
double MissingMassCalculator::s_ter_sigma_par[2][10][3];

// returns dTheta3D probability based on ATLAS parameterization
double MissingMassCalculator::dTheta3d_probabilityFast(const int & tau_type,const double & dTheta3d,const  double & P_tau) {
  double prob=1.0E-10;
  int tau_code;  // 0: l, 1:1-prong, 2:3-prong
  if(tau_type<3) {
    tau_code=tau_type;
  }
  else if(tau_type==3) {
    tau_code=2;
  }
  else
    {
      std::cout<<"---- WARNING in MissingMassCalculator::dTheta3d_probabilityFast() ----"<<std::endl;
      std::cout<<"..... wrong tau_type="<<tau_type<<std::endl;
      std::cout<<"..... returning prob="<<prob<<std::endl;
      std::cout<<"____________________________________________________________"<<std::endl;
      return prob;
    }


  double myDelThetaParam[6];

  for (int i=0;i<6;++i)
    {
      if(m_mmcCalibrationSet==MMCCalibrationSet::MMC2011
         || m_mmcCalibrationSet==MMCCalibrationSet::MMC2012) myDelThetaParam[i]=dTheta3Dparam(i,tau_code,P_tau,s_fit_param[m_mmcCalibrationSet][tau_code][i]);
      if(m_mmcCalibrationSet==MMCCalibrationSet::UPGRADE
         || m_mmcCalibrationSet==MMCCalibrationSet::LFVMMC2012
         || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015
         || m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C
         || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS)
        myDelThetaParam[i]=dTheta3Dparam(i,tau_code,P_tau,s_fit_param[MMCCalibrationSet::MMC2012][tau_code][i]);
    }
  double dTheta3dVal=dTheta3d;

  if (tau_type==0) prob=myDelThetaLepFunc(&dTheta3dVal,  myDelThetaParam);
  else prob=myDelThetaHadFunc(&dTheta3dVal,  myDelThetaParam);

  if (false)
    {

      if( m_fUseVerbose==1 && (prob>1.0 || prob<0.0))
        {
          std::cout<<"---- WARNING in MissingMassCalculator::dTheta3d_probabilityFast() ----"<<std::endl;
          std::cout<<"..... wrong probability="<<prob<<std::endl        ;
          std::cout<<"..... debugging: tau_type="<<tau_type<<"dTheta3d="<<dTheta3d<<"  P_tau="<<P_tau<<std::endl;
          std::cout<<"____________________________________________________________"<<std::endl;
          prob=1.0E-10;
        }
    }


  return prob;
}




double MissingMassCalculator::MetProbability(const double & met1,const  double & met2,const  double & MetSigma1,const  double & MetSigma2) {


  double metprob;
  if(MetSigma1>1.0 && MetSigma2>1.0) // it doesn't make sense if MET resolution sigma is <1 GeV
    {
      //SpeedUp
      metprob=exp(-0.5*(met1*met1/(MetSigma1*MetSigma1)+met2*met2/(MetSigma2*MetSigma2)))/(MetSigma1*MetSigma2*2*TMath::Pi());
    }
  else
    {
      if(m_fUseVerbose==1) std::cout<<"Warning!!! MissingMassCalculator::MetProbability: either MetSigma1 or MetSigma2 are <1 GeV--- too low, returning prob=1"<<std::endl;
      metprob=1.;
    }


  return metprob;


}

double MissingMassCalculator::MHtProbability(const double & d_mhtX, const double & d_mhtY, const double & mht,
                                             const double & trueMetGuess, const double & mht_offset) {
  // all param except trueMetguess unchanged in one event. So can cache agaisnt this one.
  //disable cache if (trueMetGuess==trueMetGuesscache) return mhtprobcache;
  double mhtprob;
  //  if(MHtSigma1>0.0 && MHtSigma2>0.0 && MHtGaussFr>0.0)

#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {
    std::cout << " debugThisIteration MHtProbability inputs "
              << " d_mhtx:" << d_mhtX
              << " d_mhty:" << d_mhtY
              << " mht:" << mht
              << " truemetguess:" << trueMetGuess
              << " offset:" << mht_offset
              <<      std::endl;
  }
#endif

  // if RANDOMNONUNIF MET already follow the double gaussian parameterisation. So weight should not include it to avoid double counting
  // formula to be checked IMHO the two gaussian should be correlated
  if ( WALKSTRATEGY==WalkStrategy::RANDOMNONUNIF ){
    mhtprob=1.;
  }
  else{
    mhtprob=exp(-0.5*pow(d_mhtX/m_preparedInput.m_MHtSigma1,2))+m_preparedInput.MHtGaussFr*exp(-0.5*pow(d_mhtX/m_preparedInput.m_MHtSigma2,2));
    mhtprob*=(exp(-0.5*pow(d_mhtY/m_preparedInput.m_MHtSigma1,2))+m_preparedInput.MHtGaussFr*exp(-0.5*pow(d_mhtY/m_preparedInput.m_MHtSigma2,2)));
  }

  const double n_arg=(mht-trueMetGuess-mht_offset)/m_preparedInput.m_MHtSigma1;
  mhtprob*=exp(-0.25*pow(n_arg,2)); // assuming sqrt(2)*sigma
  return mhtprob;
}

double MissingMassCalculator::MHtProbabilityHH(const double & d_mhtX, const double & d_mhtY, const double & mht,
                                               const double & trueMetGuess, const double & mht_offset) {
  double prob=1.0;

  // updated for final cuts, May 21 2012
  // should be merged
  prob=prob*(0.0256*TMath::Gaus(d_mhtX,0.0,m_preparedInput.m_MHtSigma1)+0.01754*TMath::Gaus(d_mhtX,0.0,m_preparedInput.m_MHtSigma2));
  prob=prob*(0.0256*TMath::Gaus(d_mhtY,0.0,m_preparedInput.m_MHtSigma1)+0.01754*TMath::Gaus(d_mhtY,0.0,m_preparedInput.m_MHtSigma2));
  const double n_arg=(mht-trueMetGuess-mht_offset)/5.7; // this sigma is different from MHtSigma1; actually it depends on dPhi
  prob=prob*exp(-0.5*pow(n_arg,2))/(5.7*sqrt(2.0*TMath::Pi())); // assuming sigma from above line

#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {
    std::cout << "mhtprobabilityhh d_mhtx=" << d_mhtX << " dMmhty=" << d_mhtY << " mht="<< mht << "truemetguess=" << trueMetGuess << "mht_offset=" << mht_offset << std::endl;
    std::cout << "mhtprobabilityhh mhtsigma1=" << m_preparedInput.m_MHtSigma1 <<  "mhtsigma2=" <<  m_preparedInput.m_MHtSigma2 << std::endl;
    std::cout << "mhtprobabilityhh prob=" << prob << std::endl;
  }
#endif


  return prob;
}

//-------------- simple Gaussian resolution, assuming perfect TES
// input tau_type=1 or 3;
// vec-- 4-vec of input detector tau
// Pt_scan value of tau Pt from TER scan
inline double MissingMassCalculator::TERProbabilitySimple(int tau_type, const TLorentzVector & vec,
                                                          const double & Pt_scan) {
  std::cout << " WARNING should not be used now that TERSimpleSigma returns the absolute sigma " << std::endl;
  double prob=1.0;
  const double sq2pi=sqrt(2*pi);
  if(tau_type==0)
    {
      std::cout<<" WARNING in TERProbabilitySimple: TER probability is requested for a lepton, returning prob=1.0"<<std::endl;
      return prob;
    }
  const double ter_sigma=TERSimpleSigma(tau_type,vec);
  const double x=Pt_scan/vec.Pt()-1.0;
  prob=0.001*exp(-0.5*std::pow(x/ter_sigma,2))/(ter_sigma*sq2pi);
  // I added an arbitrary normalization factor 0.001 just to make this
  // differential probability smaller than 1.0
  return prob;
}
//-------------- Returns sigma for simple Gaussian resolution, assuming perfect TES
// input tau_type=1 or 3;
// vec-- 4-vec of input detector tau
// Pt_scan value of tau Pt from TER scan
inline double MissingMassCalculator::TERSimpleSigma(int tau_type, const TLorentzVector & vec) {
  int eta_bin=(int)(std::abs(vec.Eta())/0.25);
  if(eta_bin>9) eta_bin=9;
  if(tau_type==0)
    {
      std::cout<<" WARNING in TERSimpleSigma: TER probability is requested for a lepton, returning sigma=0.00001"<<std::endl;
      return 0.00001;
    }
  const int type_ind=tau_type/3;

  const double ter_sigma=s_ter_sigma_par[type_ind][eta_bin][0]/sqrt(vec.Pt())+
    s_ter_sigma_par[type_ind][eta_bin][1]+
    s_ter_sigma_par[type_ind][eta_bin][2]/vec.Pt();

  return ter_sigma;
}


inline double MissingMassCalculator::mEtAndTauProbability()
{
  const int  tau_type1 = m_preparedInput.type_visTau1;
  const int  tau_type2 = m_preparedInput.type_visTau2;
  double proba=1.;
  double metprob;

  // deltaMEt is the difference between the neutrino sum and the MEt (or -HT if useHT),
  //corrected possibly from tau E scanning

  double deltaMetX=m_metVec.X()-m_inputMEtX;
  double deltaMetY=m_metVec.Y()-m_inputMEtY;

  // possibly correct for subtract tau scanning here
#ifdef TAUESCAN
  if (m_scanEtau1) {
    deltaMetX+=m_tauVec1.X()-m_preparedInput.vistau1.X();
    deltaMetY+=m_tauVec1.Y()-m_preparedInput.vistau1.Y();
  }
  if (m_scanEtau2) {
    deltaMetX+=m_tauVec2.X()-m_preparedInput.vistau2.X();
    deltaMetY+=m_tauVec2.Y()-m_preparedInput.vistau2.Y();
  }
#endif
  //  const double met_smearL=(deltaMetVec.X()*m_metCovPhiCos+deltaMetVec.Y()*m_metCovPhiSin;
  //  const double met_smearP=-deltaMetVec.X()*m_metCovPhiSin+deltaMetVec.Y()*m_metCovPhiCos;

  // rotate into error ellipse axis
  const double met_smearL=deltaMetX*m_metCovPhiCos+deltaMetY*m_metCovPhiSin;
  const double met_smearP=-deltaMetX*m_metCovPhiSin+deltaMetY*m_metCovPhiCos;


  if (m_preparedInput.UseHT)
    {
      if (tau_type1!=0 && tau_type2!=0 ) {

        metprob=MHtProbabilityHH(met_smearL,met_smearP,m_inputMEtT,m_MEtT,m_HtOffset); // for had-had
      }
      else {
        metprob=MHtProbability(met_smearL,met_smearP,m_inputMEtT,m_MEtT,m_HtOffset); // for lep-had Winter 2012 analysis
      }
    }
  else {
    metprob=MetProbability(met_smearL,met_smearP,m_preparedInput.METsigmaL,m_preparedInput.METsigmaP);
  }

  proba=metprob;

#ifdef TAUESCAN
  const double sq2pi=sqrt(2*pi);
  // tau proba. To be optimised for speed

  if (m_scanEtau1) {
    double tau1prob=exp(-std::pow((m_tauVec1E-m_preparedInput.vistau1.E())/m_preparedInput.sigmaEtau1,2)/2. )/sq2pi/m_preparedInput.sigmaEtau1;
    //double tau1prob=TERProbabilitySimple(tau_type1,preparedInput.vistau1,m_tauVec1.Pt());
    proba*=tau1prob;
  }
  if (m_scanEtau2) {
    double tau2prob=exp(-std::pow((m_tauVec2E-m_preparedInput.vistau2.E())/m_preparedInput.sigmaEtau2,2)/2. )/sq2pi/m_preparedInput.sigmaEtau2;
    //double tau2prob=TERProbabilitySimple(tau_type2,preparedInput.vistau2,m_tauVec2.Pt());
    proba*=tau2prob;
  }
#endif

  return proba;
}



// standard collinear approximation
// it returns code=0 if collinear approximation can't be applied
// and code=1 and Mrec if collinear approximation was applied
int MissingMassCalculator::StandardCollApprox(const TLorentzVector & tau_vec1, const TLorentzVector & tau_vec2, const TVector2 & met_vec, double &Mrec) {
  int code=0;
  Mrec=0.0;
  double P_nu1=0.0;
  double P_nu2=0.0;
  int coll_code=NuPsolution(met_vec,tau_vec1.Theta(),tau_vec1.Phi(),tau_vec2.Theta(),tau_vec2.Phi(),P_nu1,P_nu2);
  if(coll_code==1)
    {
      code=1;
      TLorentzVector nu1(P_nu1*sin(tau_vec1.Theta())*cos(tau_vec1.Phi()),P_nu1*sin(tau_vec1.Theta())*sin(tau_vec1.Phi()),P_nu1*cos(tau_vec1.Theta()),P_nu1);
      TLorentzVector nu2(P_nu2*sin(tau_vec2.Theta())*cos(tau_vec2.Phi()),P_nu2*sin(tau_vec2.Theta())*sin(tau_vec2.Phi()),P_nu2*cos(tau_vec2.Theta()),P_nu2);
      Mrec=(nu1+nu2+tau_vec1+tau_vec2).M();
    }
  return code;
}

// Sasha: keep this for now, may need in the future
// returns analytical P(theta)-probability for given tau-topology
// decayType==1 for leptonic decays and 0 for hadronic decays
// uses product of probabilities
double MissingMassCalculator::AngularProbability(TLorentzVector nu_vec, TLorentzVector vis_vec, int decayType) {
  double prob=0.0;
  double M=1.777;
  double angl=0.0;
  double P=(vis_vec+nu_vec).P();
  double V=P/sqrt(P*P+M*M); // tau speed
  double dA=m_dRmax_tau/(2.0*m_niter_fit1);

  if(decayType==1) // leptonic tau for now
    {
      // exact formular for energy probability is sqrt(1-V^2)/(2*V*p_0)*dE
      double n_1=nu_vec.M();
      double n_2=vis_vec.M();
      double E_nu=(M*M+n_1*n_1-n_2*n_2)/(2.0*M);
      if(E_nu<=n_1) return 0.0;
      double P_nu=sqrt(pow(E_nu,2)-pow(n_1,2));
      double prob1=0.5*sqrt(1-V*V)/(P_nu*V); // energy probability

      angl=Angle(vis_vec,vis_vec+nu_vec); // using lepton direction
      double det1=1.0-V*cos(angl+dA);
      double det2= (angl-dA)>0.0 ? 1.0-V*cos(angl-dA) : 1.0-V;
      double prob2=std::abs(1.0/det1-1.0/det2)*(1.0-V*V)/(2.0*V); // using massless approximation for leptons
      prob=prob1*prob2;
    }
  if(decayType==0) // hadronic tau
    {
      // exact formula for energy probability is sqrt(1-V^2)/(2*V*p_0)*dE
      // drop p_0 because it's a contstant for a given hadronic tau
      double prob1=0.5*sqrt(1-V*V)/V; // "energy" probability

      angl=Angle(nu_vec,vis_vec+nu_vec); // using neutrino direction
      double det1=1.0-V*cos(angl+dA);
      double det2= (angl-dA)>0.0 ? 1.0-V*cos(angl-dA) : 1.0-V;
      double prob2=std::abs(1.0/det1-1.0/det2)*(1.0-V*V)/(2.0*V);
      prob=prob1*prob2;
    }
  return prob;
}

//----- checks kinematics of solutions
// returns 1 if kinematics consistent with tau, 0 otherwise
int MissingMassCalculator::CheckSolutions(TLorentzVector nu_vec, TLorentzVector vis_vec, int decayType)
{
  int passcode=1;
  double M=1.777;
  double P=(vis_vec+nu_vec).P();
  double V=P/sqrt(P*P+M*M); // tau speed
  if(decayType==0) // hadronic tau
    {
      double m=vis_vec.M();
      if(m>M) m=2.0*M-m; // artificial hack: visible mass can be larger than M due to smearing
      if(m<=0.0) m=0.0; // this should not happen, but if it does things are already screwed up
      double E_nu=(M*M-m*m)/(2.0*M);
      double E_vis=(M*M+m*m)/(2.0*M);
      double P_vis=sqrt(pow(E_vis,2)-m*m);
      double Enu_lim[2];
      Enu_lim[0]=E_nu*sqrt((1.0-V)/(1.0+V));
      Enu_lim[1]=E_nu*sqrt((1.0+V)/(1.0-V));
      if(nu_vec.E()<Enu_lim[0] || nu_vec.E()>Enu_lim[1])
        {
          if(m_fUseVerbose==1) std::cout<<" DEBUG: MissingMassCalculator::CheckSolutions --- neutrino energy outside range "<<std::endl;
          return 0;
        }
      double Evis_lim[2];
      Evis_lim[0]=(E_vis-V*P_vis)/sqrt(1.0-V*V);
      Evis_lim[1]=(E_vis+V*P_vis)/sqrt(1.0-V*V);
      if(vis_vec.E()<Evis_lim[0] || vis_vec.E()>Evis_lim[1])
        {
          if(m_fUseVerbose==1) std::cout<<" DEBUG: MissingMassCalculator::CheckSolutions --- visTau energy outside range "<<std::endl;
          return 0;
        }
    }
  if(decayType==1) // leptonic tau
    {
      if((nu_vec+vis_vec).M()>M) return 0;
      if((nu_vec+vis_vec).M()<0.9*M) return 0;
      double m1=nu_vec.M();
      double m2=vis_vec.M();
      if(m2>M) m2=2.0*M-m2; // artificial hack: visible mass can be larger than M due to smearing
      if(m2<=0.0) m2=0.0; // this should not happen, but if it does things are already screwed up
      double E_nu=(M*M+m1*m1-m2*m2)/(2.0*M);
      if(E_nu<=m1) return 0;
      double P_nu=sqrt(pow(E_nu,2)-pow(m1,2));
      double E_vis=(M*M+m2*m2-m1*m1)/(2.0*M);
      if(E_vis<=m2) return 0;
      double P_vis=sqrt(pow(E_vis,2)-pow(m2,2));
      double Enu_lim[2];
      Enu_lim[0]=(E_nu-V*P_nu)/sqrt(1.0-V*V);
      Enu_lim[1]=(E_nu+V*P_nu)/sqrt(1.0-V*V);
      if(nu_vec.E()<Enu_lim[0] || nu_vec.E()>Enu_lim[1])
        {
          if(m_fUseVerbose==1) std::cout<<" DEBUG: MissingMassCalculator::CheckSolutions --- neutrino energy outside range "<<std::endl;
          return 0;
        }
      double Evis_lim[2];
      Evis_lim[0]=(E_vis-V*P_vis)/sqrt(1.0-V*V);
      Evis_lim[1]=(E_vis+V*P_vis)/sqrt(1.0-V*V);
      if(vis_vec.E()<Evis_lim[0] || vis_vec.E()>Evis_lim[1])
        {
          if(m_fUseVerbose==1) std::cout<<" DEBUG: MissingMassCalculator::CheckSolutions --- visTau energy outside range "<<std::endl;
          return 0;
        }
    }

  return passcode;
}

// returns Mnu probability according pol6 parameterization
double MissingMassCalculator::MnuProbability(double mnu, double binsize)
{
  double prob=1.0;
  double norm=4851900.0;
  double p[7];
  p[0]=-288.6/norm; p[1]=6.217E4/(2.0*norm); p[2]=2.122E4/(3.0*norm); p[3]=-9.067E4/(4.0*norm);
  p[4]=1.433E5/(5.0*norm); p[5]=-1.229E5/(6.0*norm); p[6]=3.434E4/(7.0*norm);
  double int1=0.0;
  double int2=0.0;
  double x1= mnu+0.5*binsize < 1.777-0.113 ? mnu+0.5*binsize : 1.777-0.113;
  double x2= mnu-0.5*binsize > 0.0 ? mnu-0.5*binsize : 0.0;
  for(int i=0; i<7; i++)
    {
      int1=p[i]*pow(x1,i+1)+int1;
      int2=p[i]*pow(x2,i+1)+int2;
    }
  prob=int1-int2;
  if(prob<0.0)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::MnuProbability: negative probability!!! "<<std::endl;
      return 0.0;
    }
  if(prob>1.0)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::MnuProbability: probability > 1!!! "<<std::endl;
      return 1.0;
    }
  return prob;
}

// returns Mnu probability according pol6 parameterization
double MissingMassCalculator::MnuProbability(double mnu)
{
  if(m_fUseMnuProbability==0) return 1.0;
  double prob=1.0;
  const double norm=4851900.0;
  double p[7];
  p[0]=-288.6; p[1]=6.217E4; p[2]=2.122E4; p[3]=-9.067E4;
  p[4]=1.433E5; p[5]=-1.229E5; p[6]=3.434E4;
  double int1=0.0;
  for(int i=0; i<7; i++)
    {
      int1+=p[i]*pow(mnu,i);
    }
  prob=int1/norm;
  if(prob<0.0)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::MnuProbability: negative probability!!! "<<std::endl;
      return 0.0;
    }
  if(prob>1.0)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::MnuProbability: probability > 1!!! "<<std::endl;
      return 1.0;
    }
  return prob;
}


inline void MissingMassCalculator::swapTLVPointers  (TLorentzVector * & p1,TLorentzVector * & p2){
  TLorentzVector * ptmp;
  ptmp=p1;
  p1=p2;
  p2=ptmp;
}



double MissingMassCalculator::Angle(const TLorentzVector & vec1, const TLorentzVector & vec2) {
  //SpeedUp (both are equivalent in fact)
  return acos((vec1.Px()*vec2.Px()+vec1.Py()*vec2.Py()+vec1.Pz()*vec2.Pz())/(vec1.P()*vec2.P()));
  //return vec1.Angle(vec2.Vect());
}



// returns probability of angle between two tau's
// assuming massless tau's for now, should be small effect for M>M_Z
double MissingMassCalculator::ResonanceProbability(TLorentzVector vec1, TLorentzVector vec2) {

  double prob=1.0;
  double boson_P=(vec1+vec2).P();
  if(boson_P==0.0) return 1.0;
  double boson_E=(vec1+vec2).E();
  double boson_V=0.0;
  if(boson_E>0.0) boson_V=boson_P/boson_E;
  else return 1.0E-10;

  double testMass=(vec1+vec2).M();
  double m=1.777; // tau mass
  double E_tau=testMass/2.0;
  double P_tau=sqrt(pow(E_tau,2)-m*m);
  double Evis_lim[2];
  Evis_lim[0]=(E_tau-boson_V*P_tau)/sqrt(1.0-boson_V*boson_V);
  Evis_lim[1]=(E_tau+boson_V*P_tau)/sqrt(1.0-boson_V*boson_V);
  if(vec1.E()<Evis_lim[0] || vec1.E()>Evis_lim[1]) return 1.0E-20;
  if(vec2.E()<Evis_lim[0] || vec2.E()>Evis_lim[1]) return 1.0E-20;

  double prob1=0.5*sqrt(1-boson_V*boson_V)/(P_tau*boson_V);

  if(vec1.P()*vec2.P()>0)
    {
      double theta=acos((vec1.Px()*vec2.Px()+vec1.Py()*vec2.Py()+vec1.Pz()*vec2.Pz())/(vec1.P()*vec2.P()));
      if(boson_V>0.0 && boson_V<1.0)
        {
          if(boson_V<cos(theta/2)) return 1.0E-10;
          double num=(1.0-boson_V*boson_V)*cos(theta/2);
          double denom=4*boson_V*sin(theta/2)*sin(theta/2)*sqrt(boson_V*boson_V-cos(theta/2)*cos(theta/2));
          prob=num/denom;
        }
      else
        {
          if(std::abs(theta-TMath::Pi())>0.0001) return 1.0E-10;
        }
    }
  else return 1.0E-10;
  prob=prob*prob1;
  if(prob<1.0E-20) prob=1.0E-20;
  return prob;
}


// like v9fast, but the parameter space scanning is now factorised out, to allow flexibility
int MissingMassCalculator::DitauMassCalculatorV9walk(){

  int nsuccesses=0;

  m_OutputInfo.AveSolRMS=0;

  int fit_code=0; // 0==bad, 1==good
  ClearDitauStuff(m_fDitauStuffFit);
  ClearDitauStuff(m_fDitauStuffHisto);
  m_OutputInfo.AveSolRMS=0.;


  m_fMfit_all->Reset();
  m_fMfit_allNoWeight->Reset();
  m_fPXfit1->Reset();
  m_fPYfit1->Reset();
  m_fPZfit1->Reset();
  m_fPXfit2->Reset();
  m_fPYfit2->Reset();
  m_fPZfit2->Reset();

#ifdef HISTITERATION
  m_fPhi1->Reset();
  m_fPhi2->Reset();
  m_fMnu1->Reset();
  m_fMnu2->Reset();
  m_fMetx->Reset();
  m_fMety->Reset();
#endif
#ifdef HISTPROBA
  m_fTheta3D->Reset();
  m_fTauProb->Reset();
#endif
  m_prob_tmp=0.0;


  //SpeedUp
  TStopwatch timer;
  if (m_fSpeedStudy) timer.Start();
  m_iter1=0;


  m_totalProbSum=0;
  m_mtautauSum=0;

  TVector2 deltamet_vec;

  if (m_fSpeedStudy) timer.Start();
  //initialize a spacewalker, which walks the parameter space according to some algorithm
  SpaceWalkerInit();

  while (SpaceWalkerWalk()) {
#ifdef DEBUGTHISITERATION
    m_debugThisIteration=(m_iter0==DEBUGTHISITERATION); // specify number
    if (m_debugThisIteration) std::cout << "Now debugging interation " << m_iter0 << std::endl;
#endif
    bool paramOutsideRange=false;
    m_nsol=0;

    if (WALKSTRATEGY==WalkStrategy::GRID) {
      if (!checkMEtInRange()) continue; // no simple way to cut corners in grid
    }if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
      paramOutsideRange=!checkAllParamInRange();
    }

#ifdef DEBUGTHISITERATION
    if (m_debugThisIteration) std::cout << " debugThisIteration " << __LINE__ << " paramOutsideRange " << paramOutsideRange << std::endl;
#endif

#ifdef DUMPITERATION
    //add 100 to get positive values, because sort is confused by negative values
    //const int dumpInt=100;
    const int dumpInt=0;
    std::cout << "DEBUGV9W " << m_iter0
              << " p1 " << m_Phi1+dumpInt
              << " p2 " << m_Phi2+dumpInt
              << " m1 "   << m_Mnu1+dumpInt
              << " m2 "    << m_Mnu2+dumpInt
              << " eL "   << m_MEtL+dumpInt
              << " eP "    << m_MEtP+dumpInt
#ifdef TAUESCAN
              << " t1 "   << m_eTau1+dumpInt
              << " t2 "    << m_eTau2+dumpInt
              << " t20 "    << m_eTau20+dumpInt
#endif
      //                << " i0 " << m_iter0
              << std::endl;
#endif


    // FIXME if no tau scanning, or symmetric matrices, rotatin is made twice which is inefficient
    const double deltaMetx=m_MEtL*m_metCovPhiCos-m_MEtP*m_metCovPhiSin;
    const double deltaMety=m_MEtL*m_metCovPhiSin+m_MEtP*m_metCovPhiCos;

    //    deltaMetVec.Set(met_smear_x,met_smear_y);
    m_metVec.Set(m_inputMEtX+deltaMetx,m_inputMEtY+deltaMety);

    // save in global variable for speed sake
    m_MEtX=m_metVec.Px();
    m_MEtY=m_metVec.Py();
    m_MEtT=m_metVec.Mod();

    //    std::cout << "DRDR met_smear " << met_smear_x << " " << met_smear_y << std::endl;
#ifdef HISTITERATION
    m_fPhi1->Fill(m_Phi1,1.);
    m_fPhi2->Fill(m_Phi2,1.);
    m_fMnu1->Fill(m_Mnu1,1.);
    m_fMnu2->Fill(m_Mnu2,1.);
    m_fMetx->Fill(deltaMetx,1.);
    m_fMety->Fill(deltaMety,1.);
#endif




    if (!paramOutsideRange)   probCalculatorV9fast(
                                                   m_Phi1, m_Phi2,
                                                   m_Mnu1, m_Mnu2);




    // DR for markov chain need to enter handleSolution also when zero solutions
    handleSolutions();
    // be careful that with markov, current solution is from now on stored in XYZOldSolVec


#ifdef DUMPITERATION
    if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
      std::cout << "DEBUGMARKOV nFullScan " << m_markovNFullScan << " nRejectNoSol " << m_markovNRejectNoSol
                << " nRejectMetro "  << m_markovNRejectMetropolis << " nAccept " << m_markovNAccept
                << " nCountuplicate " << m_markovCountDuplicate
                << std::endl ;
    }
#endif


    if (m_nsol<=0) continue;

#ifdef DEBUGTHISITERATION
    if (m_debugThisIteration) std::cout << " debugThisIteration " << __LINE__ << std::endl;
#endif

    if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
      // for markov, nsuccess more difficult to define. Decide this is the number of independent point accepted (hence without weight)
      nsuccesses=m_markovNAccept;
      m_iterNsuc=m_markovNAccept;
    }
    else
      {
        ++nsuccesses;
        ++m_iterNsuc;
      }

    m_iter1+=m_nsol;
    fit_code=1;




#ifdef DUMPITERATION
    for (int isol=0;isol<m_nsol;++isol){
      std::cout << "DEBUGV9W " << m_iter0 << " p1 " << m_Phi1
                << " p2 " << m_Phi2
                << " m1 "   << m_Mnu1
                << " m2 "    << m_Mnu2
#ifdef TAUESCAN
                << " t1 "   << m_eTau1
                << " t2 "    << m_eTau2
#endif
                << " ex "   << deltaMetx
                << " ey "    << deltaMety
                << " m " << m_mtautauFinalSolVec[isol]
                << " pr "  << m_probFinalSolVec[isol] << std::endl;
    }
#endif




  } // while loop


  m_OutputInfo.NTrials=m_iter0;
  m_OutputInfo.NSuccesses=nsuccesses;

  if (nsuccesses>0) {
    m_OutputInfo.AveSolRMS/=nsuccesses;
  } else {
    m_OutputInfo.AveSolRMS=-1.;
  }


  double Px1, Py1, Pz1;
  double Px2, Py2, Pz2;
  if(nsuccesses>0)
    {

      // note that smoothing can slightly change the integral of the histogram


#ifdef SMOOTH
      m_fMfit_all->Smooth();
      m_fMfit_allNoWeight->Smooth();
      m_fPXfit1->Smooth();
      m_fPYfit1->Smooth();
      m_fPZfit1->Smooth();
      m_fPXfit2->Smooth();
      m_fPYfit2->Smooth();
      m_fPZfit2->Smooth();
#endif


      // default max finding method defined in MissingMassCalculator.h
      // note that window defined in terms of number of bin, so depend on binning
      std::vector<double> histInfo(HistInfo::MAXHISTINFO);
      m_fDitauStuffHisto.Mditau_best=maxFromHist(m_fMfit_all,histInfo);
      double prob_hist=histInfo.at(HistInfo::PROB);

      if(prob_hist!=0.0) m_fDitauStuffHisto.Sign_best=-log10(std::abs(prob_hist));
      else {
        // this mean the histogram is empty.
        // possible but very rare if all entries outside histogram range
        // fall back to maximum
        m_fDitauStuffHisto.Sign_best=-999.;
        m_fDitauStuffHisto.Mditau_best=m_fDitauStuffFit.Mditau_best;

        //      std::cout << "ERROR should not happen. null prob " << prob_hist << std::endl;

      }

      if(m_fDitauStuffHisto.Mditau_best>0.0) m_fDitauStuffHisto.RMSoverMPV=m_fMfit_all->GetRMS()/m_fDitauStuffHisto.Mditau_best;
      std::vector<double> histInfoOther(HistInfo::MAXHISTINFO);
      //---- getting Nu1
      Px1=maxFromHist(m_fPXfit1,histInfoOther);
      Py1=maxFromHist(m_fPYfit1,histInfoOther);
      Pz1=maxFromHist(m_fPZfit1,histInfoOther);


      //---- getting Nu2
      Px2=maxFromHist(m_fPXfit2,histInfoOther);
      Py2=maxFromHist(m_fPYfit2,histInfoOther);
      Pz2=maxFromHist(m_fPZfit2,histInfoOther);

      //---- setting 4-vecs
      TLorentzVector fulltau1,fulltau2;

      fulltau1.SetXYZM(Px1,Py1,Pz1,1.777);
      fulltau2.SetXYZM(Px2,Py2,Pz2,1.777);
      if(fulltau1.P()<m_preparedInput.vistau1.P()) fulltau1=1.01*m_preparedInput.vistau1; // protection against cases when fitted tau momentum is smaller than visible tau momentum
      if(fulltau2.P()<m_preparedInput.vistau2.P()) fulltau2=1.01*m_preparedInput.vistau2; // protection against cases when fitted tau momentum is smaller than visible tau momentum
      m_fDitauStuffHisto.taufit1=fulltau1;
      m_fDitauStuffHisto.taufit2=fulltau2;
      m_fDitauStuffHisto.vistau1=m_preparedInput.vistau1; // FIXME should also be fitted if tau scan
      m_fDitauStuffHisto.vistau2=m_preparedInput.vistau2;
      m_fDitauStuffHisto.nutau1=fulltau1-m_preparedInput.vistau1; // these are the original tau vis
      m_fDitauStuffHisto.nutau2=fulltau2-m_preparedInput.vistau2; // FIXME neutrino mass not necessarily zero
    }


#ifdef HISTPROBA
#define ONEHIST
#endif
#ifdef HISTITERATION
#define ONEHIST
#endif
#ifdef HISTMASS
#define ONEHIST
#endif

#ifdef ONEHIST
  int Leptau1=0;
  int Leptau2=0;

  if(tau_type1==0) Leptau1=1;
  if(tau_type2==0) Leptau2=1;


  const int icase=4-Leptau1-2*Leptau2;
  const int algversion=3;

  std::stringstream sstream;
  sstream << "_case" << icase << "_alg" << algversion ;

  TString aHistSuffix,aHistName ;
  sstream >> aHistSuffix ;
  TH1F* theHist=0;
#endif



#ifdef HISTPROBA
  // need the stuff below as well

  aHistName="probTheta3D"+aHistSuffix;
  theHist=(TH1F*) m_fTheta3D->Clone(aHistName);
  theHist->Write();
  aHistName="TauProb"+aHistSuffix;
  theHist=(TH1F*) m_fTauProb->Clone(aHistName);
  theHist->Write();
#endif

#ifdef HISTITERATION

  aHistName="phi1"+aHistSuffix;
  theHist=(TH1F*) m_fPhi1->Clone(aHistName);
  theHist->Write();
  aHistName="phi2"+aHistSuffix;
  theHist=(TH1F*) m_fPhi2->Clone(aHistName);
  theHist->Write();
  aHistName="mnu1"+aHistSuffix;
  theHist=(TH1F*) m_fMnu1->Clone(aHistName);
  theHist->Write();
  aHistName="mnu2"+aHistSuffix;
  theHist=(TH1F*) m_fMnu2->Clone(aHistName);
  theHist->Write();
  aHistName="metx"+aHistSuffix;
  theHist=(TH1F*) m_fMetx->Clone(aHistName);
  theHist->Write();
  aHistName="mety"+aHistSuffix;
  theHist=(TH1F*) m_fMety->Clone(aHistName);
  theHist->Write();
#endif


  // save the histogram

#ifdef HISTMASS


  aHistName="m"+aHistSuffix;
  theHist=(TH1F*) m_fMfit_all->Clone(aHistName);
  theHist->Write();

  aHistName="mNoW"+aHistSuffix;
  theHist=(TH1F*) m_fMfit_allNoWeight->Clone(aHistName);
  theHist->Write();
#endif

  //  std::cout << "DRDR V9W totalProbSum=" << m_totalProbSum << " mtautauSum=" << m_mtautauSum << std::endl;


  // Note that for v9walk, points outside the METx MEty disk are counted, while this was not the case for v9
  if (m_fSpeedStudy || m_fUseVerbose==1) {
    std::cout << "Scanning " ;

    if (WALKSTRATEGY==WalkStrategy::GRID) {
      std::cout << " Grid " ;
    }
    else if (WALKSTRATEGY==WalkStrategy::RANDOM) {
      std::cout << " Random " ;
    }
    else if (WALKSTRATEGY==WalkStrategy::RANDOMNONUNIF) {
      std::cout << " Random non uniform " ;
    }
    else if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
      std::cout << " Markov " ;
    }



    std::cout << " V9W niters=" << m_iter0 << " " << m_iter1  ;
    if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
      std::cout << " nFullScan " << m_markovNFullScan ;
      std::cout << " nRejectNoSol "     << m_markovNRejectNoSol;
      std::cout  << " nRejectMetro "   << m_markovNRejectMetropolis;
      std::cout  << " nAccept " << m_markovNAccept;
    }
    std::cout << " probsum " << m_totalProbSum << " msum " << m_mtautauSum  << std::endl;
  }
  if (m_fSpeedStudy) {
    timer.Stop()  ;
    std::cout << "Scanning V9W nonreproducible CPU time = " << timer.CpuTime() << "s real = " << timer.RealTime() << " s " << std::endl;
  }


  //  std::cout << "Stopping criterion : " << m_NsucStop << std::endl;
  // std::cout << "m_iterNsuc = " << m_iterNsuc << std::endl;



  if(m_fUseVerbose==1)
    {
      if(fit_code==0)
        {
          std::cout << "!!!----> Warning-3 in MissingMassCalculator::DitauMassCalculatorV9Walk() : fit status=" << fit_code << std::endl;
          std::cout << "....... No solution is found. Printing input info ......." << std::endl;
          std::cout << "  " << std::endl;

          std::cout << "  vis Tau-1: Pt=" << m_preparedInput.vistau1.Pt() << "  M=" << m_preparedInput.vistau1.M() << " eta=" << m_preparedInput.vistau1.Eta() << "  phi=" << m_preparedInput.vistau1.Phi() << "  type=" << m_preparedInput.type_visTau1 << std::endl;
          std::cout << "  vis Tau-2: Pt=" << m_preparedInput.vistau2.Pt() << "  M=" << m_preparedInput.vistau2.M() << " eta=" << m_preparedInput.vistau2.Eta() << "  phi=" << m_preparedInput.vistau2.Phi() << "  type=" << m_preparedInput.type_visTau2 << std::endl;
          std::cout << "  MET=" << m_preparedInput.MetVec.Mod() << "  Met_X=" << m_preparedInput.MetVec.Px() << "  Met_Y=" << m_preparedInput.MetVec.Py() << std::endl;
          std::cout<<" ---------------------------------------------------------- "<<std::endl;
        }
    }



  return fit_code;
}



int MissingMassCalculator::DitauMassCalculatorV9lfv()
{



  //debugThisIteration=false;
  m_debugThisIteration=true;

  int fit_code=0; // 0==bad, 1==good
  ClearDitauStuff(m_fDitauStuffFit);
  ClearDitauStuff(m_fDitauStuffHisto);
  m_OutputInfo.NTrials = 0;
  m_OutputInfo.NSuccesses = 0;
  m_OutputInfo.AveSolRMS = 0.;

  //------- Settings -------------------------------
  int NiterMET = m_niter_fit2; // number of iterations for each MET scan loop
  int NiterMnu = m_niter_fit3; // number of iterations for Mnu loop
  const double Mtau=1.777;
  double Mnu_binSize = m_MnuScanRange/NiterMnu;

  double METresX = m_preparedInput.METsigmaL; // MET resolution in direction parallel to leading jet, for MET scan
  double METresY = m_preparedInput.METsigmaP; // MET resolution in direction perpendicular to leading jet, for MET scan

  //-------- end of Settings
  int Leptau1=0; // lepton-tau code
  int Leptau2=0;
  if(m_preparedInput.type_visTau1==0) Leptau1=1;
  if(m_preparedInput.type_visTau2==0) Leptau2=1;

  if((Leptau1+Leptau2) == 2) { // both tau's are leptonic
    m_nsigma_METscan = m_nsigma_METscan_ll;
  } else if (Leptau1+Leptau2 == 1) { // lep had
    m_nsigma_METscan = m_nsigma_METscan_lh;
  }

  double N_METsigma = m_nsigma_METscan; // number of sigmas for MET scan
  double METresX_binSize = 2*N_METsigma*METresX/NiterMET;
  double METresY_binSize = 2*N_METsigma*METresY/NiterMET;

  int solution = 0;

  std::vector<TLorentzVector> nu_vec;

  m_totalProbSum = 0;
  m_mtautauSum = 0;

  double metprob = 1.0;
  double sign_tmp = 0.0;
  double tauprob = 1.0;
  double totalProb = 0.0;

  m_prob_tmp = 0.0;

  double met_smear_x=0.0;
  double met_smear_y=0.0;
  double met_smearL=0.0;
  double met_smearP=0.0;

  double angle1=0.0;

  if(m_fMfit_all) {
    m_fMfit_all->Reset();
  }
  if(m_fMfit_allNoWeight) {
    m_fMfit_allNoWeight->Reset();
  }
  if(m_fPXfit1) {
    m_fPXfit1->Reset();
  }
  if(m_fPYfit1) {
    m_fPYfit1->Reset();
  }
  if(m_fPZfit1) {
    m_fPZfit1->Reset();
  }


  //SpeedUp
  TStopwatch timer;
  if (m_fSpeedStudy) timer.Start();
  int iter0 = 0;
  m_iter1 = 0;
  m_iter2 = 0;
  m_iter3 = 0;
  m_iter4 = 0;
  const double met_coscovphi = cos(m_preparedInput.METcovphi);
  const double met_sincovphi = sin(m_preparedInput.METcovphi);

  m_iang1low = 0;
  m_iang1high = 0;

  int nsuccesses = 0;
  int isol = 0;


  //   double Mvis=(tau_vec1+tau_vec2).M();
  //   TLorentzVector met4vec(0.0,0.0,0.0,0.0);
  //   met4vec.SetPxPyPzE(met_vec.X(),met_vec.Y(),0.0,met_vec.Mod());
  //   double Meff=(tau_vec1+tau_vec2+met4vec).M();
  //   double met_det=met_vec.Mod();

  //---------------------------------------------
  if((Leptau1+Leptau2)==2) // dilepton case
    {
      if(m_fUseVerbose==1) { std::cout << "Running in dilepton mode" << std::endl; }
      double input_metX=m_preparedInput.MetVec.X();
      double input_metY=m_preparedInput.MetVec.Y();

      TLorentzVector tau_tmp(0.0,0.0,0.0,0.0);
      TLorentzVector lep_tmp(0.0,0.0,0.0,0.0);
      int tau_type_tmp;
      int tau_ind=0;

      if(m_LFVmode==1) // muon case: H->mu+tau(->ele) decays
        {
          if(m_preparedInput.vistau1.M()>0.05 && m_preparedInput.vistau2.M()<0.05) // chosing lepton from Higgs decay
            {
              tau_tmp=m_preparedInput.vistau2;
              lep_tmp=m_preparedInput.vistau1;
              tau_type_tmp=m_preparedInput.type_visTau2;
              tau_ind=2;
            }
          else
            {
              tau_tmp=m_preparedInput.vistau1;
              lep_tmp=m_preparedInput.vistau2;
              tau_type_tmp=m_preparedInput.type_visTau1;
              tau_ind=1;
            }
        }
      if(m_LFVmode==0) // electron case: H->ele+tau(->mu) decays
        {
          if(m_preparedInput.vistau1.M()<0.05 && m_preparedInput.vistau2.M()>0.05) // chosing lepton from Higgs decay
            {
              tau_tmp=m_preparedInput.vistau2;
              lep_tmp=m_preparedInput.vistau1;
              tau_type_tmp=m_preparedInput.type_visTau2;
              tau_ind=2;
            }
          else
            {
              tau_tmp=m_preparedInput.vistau1;
              lep_tmp=m_preparedInput.vistau2;
              tau_type_tmp=m_preparedInput.type_visTau1;
              tau_ind=1;
            }
        }

      //------- Settings -------------------------------
      double Mlep=tau_tmp.M();
      //       double dMnu_max=m_MnuScanRange-Mlep;
      //       double Mnu_binSize=dMnu_max/NiterMnu;
      //-------- end of Settings

      //       double M=Mtau;
      double M_nu=0.0;
      double MnuProb=1.0;
      //---------------------------------------------
      for(int i3=0; i3<NiterMnu; i3++) //---- loop-3: virtual neutrino mass
        {
          M_nu=Mnu_binSize*i3;
          if(M_nu>=(Mtau-Mlep)) continue;
          //      M=sqrt(Mtau*Mtau-M_nu*M_nu);
          MnuProb=MnuProbability(M_nu,Mnu_binSize); // Mnu probability
          //---------------------------------------------
          for(int i4=0; i4<NiterMET+1; i4++) // MET_X scan
            {
              met_smearL=METresX_binSize*i4-N_METsigma*METresX;
              for(int i5=0; i5<NiterMET+1; i5++) // MET_Y scan
                {
                  met_smearP=METresY_binSize*i5-N_METsigma*METresY;
                  if(pow(met_smearL/METresX,2)+pow(met_smearP/METresY,2)>pow(N_METsigma,2)) continue; // use ellipse instead of square
                  met_smear_x=met_smearL*met_coscovphi-met_smearP*met_sincovphi;
                  met_smear_y=met_smearL*met_sincovphi+met_smearP*met_coscovphi;
                  metvec_tmp.Set(input_metX+met_smear_x,input_metY+met_smear_y);

                  solution=NuPsolutionLFV(metvec_tmp,tau_tmp,M_nu,nu_vec);

                  ++iter0;

                  if(solution<1) continue;
                  ++m_iter1;
                  ++isol;
                  // if fast sin cos, result to not match exactly nupsolutionv2, so skip test
                  //SpeedUp no nested loop to compute individual probability
                  int ngoodsol1=0;

                  metprob=MetProbability(met_smearL,met_smearP,METresX,METresY);
                  if(metprob<=0) continue;
                  for(unsigned int j1=0; j1<nu_vec.size(); j1++)
                    {
                      if(tau_tmp.E()+nu_vec[j1].E()>=m_beamEnergy) continue;
                      const double tau1_tmpp=(tau_tmp+nu_vec[j1]).P();
                      angle1=Angle(nu_vec[j1],tau_tmp);

                      if(angle1<dTheta3DLimit(tau_type_tmp,0,tau1_tmpp)) {++m_iang1low ; continue;} // lower 99% bound
                      if(angle1>dTheta3DLimit(tau_type_tmp,1,tau1_tmpp)) {++m_iang1high ; continue;} // upper 99% bound
                      double tauvecprob1j=dTheta3d_probabilityFast(tau_type_tmp,angle1,tau1_tmpp);
                      if(tauvecprob1j==0.) continue;
                      tauprob=TauProbabilityLFV(tau_type_tmp,tau_tmp,nu_vec[j1]);
                      totalProb=tauvecprob1j*metprob*MnuProb*tauprob;

                      m_tautau_tmp.SetPxPyPzE(0.0,0.0,0.0,0.0);
                      m_tautau_tmp+=tau_tmp;
                      m_tautau_tmp+=lep_tmp;
                      m_tautau_tmp+=nu_vec[j1];

                      const double mtautau=m_tautau_tmp.M();

                      m_totalProbSum+=totalProb;
                      m_mtautauSum+=mtautau;

#ifdef DUMPITERATION
                      //add 100 to get positive values, because sort is confused by negative values
                      std::cout << "DEBUGV9 "
                                << " eL "   << met_smearL+100
                                << " eP "    << met_smearP+100
                                << " tob "  << totalProb
                                << " tb1 " << tauvecprob1j
                                << " meb " << metprob
                        //<< " met " << met_det
                                << " i0 " << iter0
                                << std::endl;
#endif

                      fit_code=1; // at least one solution is found

                      m_fMfit_all->Fill(mtautau,totalProb);
                      m_fMfit_allNoWeight->Fill(mtautau,1.);
                      //----------------- using P*fit to fill Px,y,z_tau
                      m_fPXfit1->Fill((tau_tmp+nu_vec[j1]).Px(),totalProb);
                      m_fPYfit1->Fill((tau_tmp+nu_vec[j1]).Py(),totalProb);
                      m_fPZfit1->Fill((tau_tmp+nu_vec[j1]).Pz(),totalProb);

                      if(totalProb>m_prob_tmp) // fill solution with highest probability
                        {
                          sign_tmp=-log10(totalProb);
                          m_prob_tmp=totalProb;
                          m_fDitauStuffFit.Mditau_best=mtautau;
                          m_fDitauStuffFit.Sign_best=sign_tmp;
                          if(tau_ind==1) m_fDitauStuffFit.nutau1=nu_vec[j1];
                          if(tau_ind==2) m_fDitauStuffFit.nutau2=nu_vec[j1];
                        }

                      ++ngoodsol1;
                    }

                  if (ngoodsol1==0) continue;
                  m_iter2+=1;

                  ++nsuccesses;
                  m_iter3+=1;

                }
            }
        }
    }
  else if((Leptau1+Leptau2)==1) // lepton+tau case
    {
      if(m_fUseVerbose==1) { std::cout << "Running in lepton+tau mode" << std::endl; }
      //------- Settings -------------------------------

      //----- Stuff below are for Winter 2012 lep-had analysis only; it has to be replaced by a more common scheme once other channels are optimized
      //       TVector2 mht_vec((tau_vec1+tau_vec2).Px(),(tau_vec1+tau_vec2).Py()); // missing Ht vector for Njet25=0 events
      //       const double mht=mht_vec.Mod();
      double input_metX=m_preparedInput.MetVec.X();
      double input_metY=m_preparedInput.MetVec.Y();

      //       double mht_offset=0.0;
      //       if(InputInfo.UseHT)  // use missing Ht (for 0-jet events only for now)
      //        {
      //          input_metX=-mht_vec.X();
      //          input_metY=-mht_vec.Y();
      //          // dPhi(l-t) dependence of misHt-trueMET
      //          if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2011){
      //            if(met_det<20.0) mht_offset=InputInfo.DelPhiTT>1.8 ? -161.9+235.5*InputInfo.DelPhiTT-101.6*pow(InputInfo.DelPhiTT,2)+13.57*pow(InputInfo.DelPhiTT,3) : 12.0;
      //            else mht_offset=115.5-78.1*InputInfo.DelPhiTT+12.83*pow(InputInfo.DelPhiTT,2);

      //          } else if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2012){
      //            if(met_det<20.0) mht_offset=132.1-79.26*InputInfo.DelPhiTT+11.77*pow(InputInfo.DelPhiTT,2);
      //            else mht_offset=51.28-23.56*InputInfo.DelPhiTT+2.637*pow(InputInfo.DelPhiTT,2);
      //          }
      //        }
      //       else // use MET (for 0-jet and 1-jet events)
      //        {
      //          input_metX=met_vec.X();
      //          input_metY=met_vec.Y();
      //        }



      TLorentzVector tau_tmp(0.0,0.0,0.0,0.0);
      TLorentzVector lep_tmp(0.0,0.0,0.0,0.0);
      int tau_type_tmp;
      if(Leptau1==1)
        {
          tau_tmp=m_preparedInput.vistau2;
          lep_tmp=m_preparedInput.vistau1;
          tau_type_tmp=m_preparedInput.type_visTau2;
        }
      if(Leptau2==1)
        {
          tau_tmp=m_preparedInput.vistau1;
          lep_tmp=m_preparedInput.vistau2;
          tau_type_tmp=m_preparedInput.type_visTau1;
        }

      //---------------------------------------------
      for(int i4=0; i4<NiterMET+1; i4++) // MET_X scan
        {
          met_smearL=METresX_binSize*i4-N_METsigma*METresX;
          for(int i5=0; i5<NiterMET+1; i5++) // MET_Y scan
            {
              met_smearP=METresY_binSize*i5-N_METsigma*METresY;
              if(pow(met_smearL/METresX,2)+pow(met_smearP/METresY,2)>pow(N_METsigma,2)) continue; // use ellipse instead of square
              met_smear_x=met_smearL*met_coscovphi-met_smearP*met_sincovphi;
              met_smear_y=met_smearL*met_sincovphi+met_smearP*met_coscovphi;
              metvec_tmp.Set(input_metX+met_smear_x,input_metY+met_smear_y);

              solution=NuPsolutionLFV(metvec_tmp,tau_tmp,0.0,nu_vec);

              ++iter0;

              if(solution<1) continue;
              ++m_iter1;
              ++isol;
              // if fast sin cos, result to not match exactly nupsolutionv2, so skip test
              //SpeedUp no nested loop to compute individual probability
              int ngoodsol1=0;

              metprob=MetProbability(met_smearL,met_smearP,METresX,METresY);
              if(metprob<=0) continue;
              for(unsigned int j1=0; j1<nu_vec.size(); j1++)
                {
                  if(tau_tmp.E()+nu_vec[j1].E()>=m_beamEnergy) continue;
                  const double tau1_tmpp=(tau_tmp+nu_vec[j1]).P();
                  angle1=Angle(nu_vec[j1],tau_tmp);

                  if(angle1<dTheta3DLimit(tau_type_tmp,0,tau1_tmpp)) {++m_iang1low ; continue;} // lower 99% bound
                  if(angle1>dTheta3DLimit(tau_type_tmp,1,tau1_tmpp)) {++m_iang1high ; continue;} // upper 99% bound
                  double tauvecprob1j=dTheta3d_probabilityFast(tau_type_tmp,angle1,tau1_tmpp);
                  if(tauvecprob1j==0.) continue;
                  tauprob=TauProbabilityLFV(tau_type_tmp,tau_tmp,nu_vec[j1]);
                  totalProb=tauvecprob1j*metprob*tauprob;

                  m_tautau_tmp.SetPxPyPzE(0.0,0.0,0.0,0.0);
                  m_tautau_tmp+=tau_tmp;
                  m_tautau_tmp+=lep_tmp;
                  m_tautau_tmp+=nu_vec[j1];

                  const double mtautau=m_tautau_tmp.M();

                  m_totalProbSum+=totalProb;
                  m_mtautauSum+=mtautau;

#ifdef DUMPITERATION
                  //add 100 to get positive values, because sort is confused by negative values
                  std::cout << "DEBUGV9 "
                            << " eL "   << met_smearL+100
                            << " eP "    << met_smearP+100
                            << " tob "  << totalProb
                            << " tb1 " << tauvecprob1j
                            << " meb " << metprob
                    //<< " met " << met_det
                            << " i0 " << iter0
                            << std::endl;
#endif

                  fit_code=1; // at least one solution is found

                  m_fMfit_all->Fill(mtautau,totalProb);
                  m_fMfit_allNoWeight->Fill(mtautau,1.);
                  ////----------------- using P*fit to fill Px,y,z_tau
                  //m_fPXfit1->Fill((tau_tmp+nu_vec[j1]).Px(),totalProb);
                  //m_fPYfit1->Fill((tau_tmp+nu_vec[j1]).Py(),totalProb);
                  //m_fPZfit1->Fill((tau_tmp+nu_vec[j1]).Pz(),totalProb);

                  if(totalProb>m_prob_tmp) // fill solution with highest probability
                    {
                      sign_tmp = -log10(totalProb);
                      m_prob_tmp = totalProb;
                      m_fDitauStuffFit.Mditau_best = mtautau;
                      m_fDitauStuffFit.Sign_best = sign_tmp;
                      if(Leptau1 == 1) {
                        m_fDitauStuffFit.vistau1 = lep_tmp;
                        m_fDitauStuffFit.vistau2 = tau_tmp;
                        m_fDitauStuffFit.nutau2 = nu_vec[j1];
                      } else if(Leptau2 == 1) {
                        m_fDitauStuffFit.vistau2 = lep_tmp;
                        m_fDitauStuffFit.vistau1 = tau_tmp;
                        m_fDitauStuffFit.nutau1 = nu_vec[j1];
                      }
                      m_fDitauStuffFit.taufit1 = m_fDitauStuffFit.vistau1 + m_fDitauStuffFit.nutau1;
                      m_fDitauStuffFit.taufit2 = m_fDitauStuffFit.vistau2 + m_fDitauStuffFit.nutau2;
                    }

                  ++ngoodsol1;
                }

              if (ngoodsol1==0) continue;
              m_iter2+=1;

              ++nsuccesses;
              m_iter3+=1;

            }
        }
    } else {
    std::cout << "Running in an unknown mode?!?!" << std::endl;
  }

  m_OutputInfo.NTrials=iter0;
  m_OutputInfo.NSuccesses=m_iter3;

  //  std::cout << "DRDR V9 totalProbSum=" << m_totalProbSum << " mtautauSum=" << m_mtautauSum << std::endl;
  if (m_fSpeedStudy || m_fUseVerbose==1) {
    std::cout << "SpeedUp niters=" << iter0 << " " << m_iter1
              << " " << m_iter2 <<" " << m_iter3
              << "skip:" << m_iang1low << " " << m_iang1high  << std::endl;
  }

  if (m_fSpeedStudy){
    timer.Stop()  ;
    std::cout << "SpeedUp nonreproducible CPU time = " << timer.CpuTime() << "s real = " << timer.RealTime() << " s " << std::endl;
  }


  if(m_fMfit_all->GetEntries()>0 && m_iter3>0)
    {
#ifdef SMOOTH
      m_fMfit_all->Smooth();
      m_fMfit_allNoWeight->Smooth();
      m_fPXfit1->Smooth();
      m_fPYfit1->Smooth();
      m_fPZfit1->Smooth();
#endif

      // default max finding method defined in MissingMassCalculator.h
      // note that window defined in terms of number of bin, so depend on binning
      std::vector<double> histInfo(HistInfo::MAXHISTINFO);
      m_fDitauStuffHisto.Mditau_best = maxFromHist(m_fMfit_all, histInfo);
      double prob_hist = histInfo.at(HistInfo::PROB);

      if(prob_hist!=0.0) m_fDitauStuffHisto.Sign_best=-log10(std::abs(prob_hist));
      else
        {
          // this mean the histogram is empty.
          // possible but very rare if all entries outside histogram range
          // fall back to maximum
          m_fDitauStuffHisto.Sign_best=-999.;
          m_fDitauStuffHisto.Mditau_best=m_fDitauStuffFit.Mditau_best;

          //    std::cout << "ERROR should not happen. null prob " << prob_hist << std::endl;
        }

      if(m_fDitauStuffHisto.Mditau_best>0.0) m_fDitauStuffHisto.RMSoverMPV = m_fMfit_all->GetRMS()/m_fDitauStuffHisto.Mditau_best;
      std::vector<double> histInfoOther(HistInfo::MAXHISTINFO);
      //---- getting Nu1
      double Px1=maxFromHist(m_fPXfit1,histInfoOther);
      double Py1=maxFromHist(m_fPYfit1,histInfoOther);
      double Pz1=maxFromHist(m_fPZfit1,histInfoOther);
      //---- setting 4-vecs
      TLorentzVector nu1_tmp(0.0,0.0,0.0,0.0);
      TLorentzVector nu2_tmp(0.0,0.0,0.0,0.0);
      if(Leptau1==1)
        {
          nu1_tmp = m_preparedInput.vistau1;
          nu2_tmp.SetXYZM(Px1,Py1,Pz1,1.777);
        }
      if(Leptau2==1)
        {
          nu2_tmp = m_preparedInput.vistau2;
          nu1_tmp.SetXYZM(Px1,Py1,Pz1,1.777);
        }
      m_fDitauStuffHisto.taufit1 = nu1_tmp;
      m_fDitauStuffHisto.taufit2 = nu2_tmp;
      m_fDitauStuffHisto.nutau1 = nu1_tmp-m_preparedInput.vistau1;
      m_fDitauStuffHisto.nutau2 = nu2_tmp-m_preparedInput.vistau2;
    }



#ifdef HISTPROBA
#define ONEHIST
#endif
#ifdef HISTITERATION
#define ONEHIST
#endif
#ifdef HISTMASS
#define ONEHIST
#endif

#ifdef ONEHIST
  const int icase=4-Leptau1-2*Leptau2;
  const int algversion=1;

  std::stringstream sstream;
  sstream << "_case" << icase << "_alg" << algversion ;

  TString aHistSuffix,aHistName ;
  sstream >> aHistSuffix ;
  TH1F* theHist=0;
#endif



#ifdef HISTPROBA
  // need the stuff below as well

  aHistName="probTheta3D"+aHistSuffix;
  theHist=(TH1F*) m_fTheta3D->Clone(aHistName);
  theHist->Write();
  aHistName="TauProb"+aHistSuffix;
  theHist=(TH1F*) m_fTauProb->Clone(aHistName);
  theHist->Write();
#endif

#ifdef HISTITERATION

  aHistName="phi1"+aHistSuffix;
  theHist=(TH1F*) m_fPhi1->Clone(aHistName);
  theHist->Write();
  aHistName="phi2"+aHistSuffix;
  theHist=(TH1F*) m_fPhi2->Clone(aHistName);
  theHist->Write();
  aHistName="mnu1"+aHistSuffix;
  theHist=(TH1F*) m_fMnu1->Clone(aHistName);
  theHist->Write();
  aHistName="mnu2"+aHistSuffix;
  theHist=(TH1F*) m_fMnu2->Clone(aHistName);
  theHist->Write();
  aHistName="metx"+aHistSuffix;
  theHist=(TH1F*) m_fMetx->Clone(aHistName);
  theHist->Write();
  aHistName="mety"+aHistSuffix;
  theHist=(TH1F*) m_fMety->Clone(aHistName);
  theHist->Write();
#endif


  // save the histogram

#ifdef HISTMASS


  aHistName="m"+aHistSuffix;
  theHist=(TH1F*) m_fMfit_all->Clone(aHistName);
  theHist->Write();

  aHistName="mNoW"+aHistSuffix;
  theHist=(TH1F*) m_fMfit_allNoWeight->Clone(aHistName);
  theHist->Write();
#endif


  if(m_fUseVerbose==1)
    {
      if(fit_code==0)
        {
          std::cout << "!!!----> Warning-3 in MissingMassCalculator::DitauMassCalculatorV9lfv() : fit status=" << fit_code << std::endl;
          std::cout << "....... No solution is found. Printing input info ......." << std::endl;
          std::cout << "  " << std::endl;

          std::cout << "  vis Tau-1: Pt=" << m_preparedInput.vistau1.Pt() << "  M=" << m_preparedInput.vistau1.M() << " eta=" << m_preparedInput.vistau1.Eta() << "  phi=" << m_preparedInput.vistau1.Phi() << "  type=" << m_preparedInput.type_visTau1 << std::endl;
          std::cout << "  vis Tau-2: Pt=" << m_preparedInput.vistau2.Pt() << "  M=" << m_preparedInput.vistau2.M() << " eta=" << m_preparedInput.vistau2.Eta() << "  phi=" << m_preparedInput.vistau2.Phi() << "  type=" << m_preparedInput.type_visTau2 << std::endl;
          std::cout << "  MET=" << m_preparedInput.MetVec.Mod() << "  Met_X=" << m_preparedInput.MetVec.Px() << "  Met_Y=" << m_preparedInput.MetVec.Py() << std::endl;
          std::cout << " ---------------------------------------------------------- " << std::endl;
        }
    }
  return fit_code;
}

// function to fit maximum
Double_t MissingMassCalculator::maxFitting(Double_t *x, Double_t *par)
//Double_t maxFitting(Double_t *x, Double_t *par)
{
  // parabola with parameters max, mean and invwidth
  const double mM=x[0];
  const double mMax=par[0];
  const double mMean=par[1];
  const double mInvWidth2=par[2]; // if param positif distance between intersection of the parabola with x axis: 1/Sqrt(mInvWidth2)
  const double fitval=mMax*(1-4*mInvWidth2*std::pow(mM-mMean,2));
  return fitval;
}








// determine the maximum from the histogram
// if input prob not default , compute also some probability
// MaxHistStrategy : different method to find maximum
// TODO should get the array on work on it
// should also find the effective range of the hist

double MissingMassCalculator::maxFromHist(TH1F* theHist, std::vector<double> & histInfo, const MaxHistStrategy::e maxHistStrategy,const int winHalfWidth,bool debug)
{
  //namespace HistInfo
  // enum e { PROB=0,INTEGRAL,CHI2,DISCRI,TANTHETA,TANTHETAW,FITLENGTH,RMS,RMSVSDISCRI,MAXHISTINFO };
  //  std::cout << "DRDR maxFromHist called " << maxHistStrategy << std::endl;
  double maxPos=0.;
  double prob=0.;

  for (std::vector<double>::iterator itr=histInfo.begin(); itr != histInfo.end();++itr ){
    *itr=-1;
  }

  histInfo[HistInfo::INTEGRAL]=theHist->Integral();


  if ( maxHistStrategy==MaxHistStrategy::MAXBIN || (( maxHistStrategy==MaxHistStrategy::MAXBINWINDOW || maxHistStrategy==MaxHistStrategy::SLIDINGWINDOW) && winHalfWidth==0 ) ) {

    // simple max search
    // original version, simple bin maximum
    int max_bin=theHist->GetMaximumBin();
    maxPos=theHist->GetBinCenter(max_bin);



    //FIXME GetEntries is unweighted
    prob=theHist->GetBinContent(max_bin)/double(theHist->GetEntries());
    if (prob>1.) prob=1.;
    histInfo[HistInfo::PROB]=prob;
    return maxPos;
  }

  int hNbins=theHist->GetNbinsX();

  if (maxHistStrategy==MaxHistStrategy::MAXBINWINDOW){
    // average around maximum bin (nearly useless in fact)
    // could be faster
    int max_bin=theHist->GetMaximumBin();
    int iBinMin=max_bin-winHalfWidth;
    if (iBinMin<0) iBinMin=0;
    int iBinMax=max_bin+winHalfWidth;
    if (iBinMax>hNbins) iBinMax=hNbins-1;
    double sumw=0;
    double sumx=0;
    for (int iBin=iBinMin ; iBin<=iBinMax ; ++iBin){
      const double weight=theHist->GetBinContent(iBin);
      sumw+=weight;
      sumx+=weight*theHist->GetBinCenter(iBin);
    }
    maxPos=sumx/sumw;



    //FIXME GetEntries is unweighted
    prob=sumw/theHist->GetEntries();
    if (prob>1.) prob=1.;

    return maxPos;
  }

  // now compute sliding window anyway
  if ( maxHistStrategy!=MaxHistStrategy::SLIDINGWINDOW && maxHistStrategy!=MaxHistStrategy::FIT ) {
    std::cout << "ERROR undefined maxHistStrategy:" << maxHistStrategy << std::endl ;
    return -10.;
  }


  // first iteration to find the first and last non zero bin, and the histogram integral (not same as Entries because of weights)
  int lastNonZeroBin=-1;
  int firstNonZeroBin=-1;
  double totalSumw=0.;
  bool firstNullPart=true;
  for (int iBin=0 ; iBin<hNbins ; ++iBin){
    const double weight=theHist->GetBinContent(iBin);
    if (weight>0) {
      totalSumw+=weight;
      lastNonZeroBin=iBin;
      if (firstNullPart){
        firstNullPart=false;
        firstNonZeroBin=iBin;
      }
    }
  }

  //enlarge first and last non zero bin with window width to avoid side effect (maximum close to the edge)
  firstNonZeroBin=std::max(0,firstNonZeroBin-winHalfWidth-1);
  lastNonZeroBin=std::min(hNbins-1,lastNonZeroBin+winHalfWidth+1);


  //  std::cout << "DRDR first non zero bin " << firstNonZeroBin << " last " << lastNonZeroBin << std::endl ;

  // if null histogram quit
  if(firstNullPart) return maxPos;


  // determine the size of the sliding window in the fit case


  // sliding window
  const int nwidth=2*winHalfWidth+1;
  double winsum=0.;


  for (int ibin=0 ; ibin<nwidth ; ++ibin){
    winsum+=theHist->GetBinContent(ibin);
  }
  double winmax=winsum;

  int max_bin=0.;
  int iBinL=firstNonZeroBin;
  int iBinR=iBinL+2*winHalfWidth;
  bool goingUp=true;

  do {
    ++iBinL;
    ++iBinR;
    const double deltawin=theHist->GetBinContent(iBinR)-theHist->GetBinContent(iBinL-1);

    //     std::cout << "DRDR ibinL " << iBinL << " iBinR " << iBinR << " deltawin=" << deltawin << " " << winsum << " " << winmax << std::endl;

    if (deltawin<0 ) {
      if (goingUp) {
        // if were climbing and now loose more on the left
        // than win on the right. This was a local maxima
        if (winsum>winmax) {
          // global maximum one so far
          winmax=winsum;
          max_bin=(iBinR+iBinL)/2-1;
          //       std::cout << "DRDR best local max " << max_bin << " " << winmax << std::endl;
        }
        //       else {

        //       std::cout << "DRDR another local max " << (iBinR+iBinL)/2-1 << " " << winsum << std::endl;
        //       }
        goingUp=false; // now going down
      }
    } else
      {
        // do not care about minima, simply indicate we are going down
        goingUp=true;
      }

    winsum+=deltawin;


  }  while (iBinR<lastNonZeroBin);


  //now compute average
  int iBinMin=max_bin-winHalfWidth;
  if (iBinMin<0) iBinMin=0;
  int iBinMax=max_bin+winHalfWidth;
  if (iBinMax>=hNbins) iBinMax=hNbins-1;
  double sumw=0;
  double sumx=0;
  for (int iBin=iBinMin ; iBin<=iBinMax ; ++iBin){
    const double weight=theHist->GetBinContent(iBin);
    sumw+=weight;
    sumx+=weight*theHist->GetBinCenter(iBin);
  }

  double maxPosWin=-1.;

  if (sumw>0.) {
    maxPosWin=sumx/sumw;
  }
  // std::cout << "DRDR now sum " << sumw << " max_bin " << max_bin << " rough max" << theHist->GetMaximumBin() << " best max " << maxPos << std::endl;



  //prob if the fraction of events in the window
  prob=sumw/totalSumw;


  // Definitions of some useful parameters


  const double h_rms=theHist->GetRMS(1);
  histInfo[HistInfo::RMS]=h_rms;

  double num=0;
  double numerator=0;
  double denominator=0;
  bool nullBin = false;

  for(int i=iBinMin;i<iBinMax;++i)
    {
      double binError=theHist->GetBinError(i);
      if(binError<1e-10)
        {
          nullBin = true;
        }
      double binErrorSquare=std::pow(binError,2);
      num = theHist->GetBinContent(i)/(binErrorSquare);
      numerator=numerator+num;
      denominator=denominator+(1/(binErrorSquare));
    }
  if(numerator<1e-10 || denominator<1e-10 || nullBin==true)
    {
      histInfo[HistInfo::MEANBIN]=-1;
    }
  else
    {
      histInfo[HistInfo::MEANBIN]=sqrt(1/denominator)/(numerator/denominator);
    }


  // stop here if only looking for sliding window
  if (maxHistStrategy==MaxHistStrategy::SLIDINGWINDOW ) {
    return maxPosWin;
  }

  maxPos=maxPosWin;
  // now FIT   maxHistStrategy==MaxHistStrategy::FIT


  // now mass fit in range defined by sliding window
  // window will be around maxPos
  const double binWidth=theHist->GetBinCenter(2)-theHist->GetBinCenter(1);
  double fitWidth=(winHalfWidth+0.5)*binWidth;
  // fit range 2 larger than original window range, 3 if less than 20% of the histogram in slinding window

  if (prob>0.2) {
    fitWidth*=2.;
  }
  else {
    fitWidth*=3.;
  }
  //   std::cout << " prob=" << prob << " fitwidth=" << fitWidth << " binwidth=" << binWidth << std::endl;

  // fit option : Q == Quiet, no printout S result of the fit returned in TFitResultPtr N do not draw the resulting function

  // if debug plot the fitted function
  TString fitOption= debug ? "QS" : "QNS";
  //   if (debug) std::cout << "MaxFromHist : debug option : draw fit " << std::endl ;



#ifndef MAXFITTINGBYHAND
  // root fit
  // Sets initial values
  m_fFitting->SetParameters(sumw/winHalfWidth,maxPos,0.0025);
  //   TFitResultPtr fitRes=theHist->Fit("pol2",fitOption,"",maxPos-fitWidth,maxPos+fitWidth);
  TFitResultPtr fitRes=theHist->Fit(m_fFitting,fitOption,"",maxPos-fitWidth,maxPos+fitWidth);

#else
  int fitRes=0;
  // experimental, do the fit by hand
  // works but time saving not useful.
  // The very first root fit takes 0.05 s but not the next one.
  // min bin and max bin
  // TODO : also do the plot (to be done only if we really want to do the fit by hand)

  int iBinFitMin=theHist->FindFixBin(maxPos-fitWidth);
  int iBinFitMax=theHist->FindFixBin(maxPos+fitWidth);
  if (iBinFitMin<=0) iBinFitMin=1;
  if (iBinFitMax>=hNbins) iBinFitMax=hNbins-1;
  int iBinFitWidth=iBinFitMax-iBinFitMin+1;
  if (iBinFitWidth>20) {
    std::cout << "MaxFromHist ERROR iBinFitMax-iBinFitMin too large ! " << iBinFitMin << " " << iBinFitMax << " " << iBinFitWidth << std::endl;
    iBinFitWidth=20;
  }
  //
  double mv[20],hv[20],invsigm2v[20];
  // fill the array
  for (int i=0;i<iBinFitWidth;++i) {
    mv[i]=theHist->GetBinCenter(i+iBinFitMin);
    hv[i]=theHist->GetBinContent(i+iBinFitMin);
    const double binError=theHist->GetBinError(i+iBinFitMin);
    if (binError>=1e-20) {
      invsigm2v[i]=1./std::pow(binError,2);
    }
    else {
      invsigm2v[i]=0.;
    }
  }
  //vector of initial value
  // polynomial a0+a1*m+a2*m*m


  // fill the vector of first derivative
  double der1[3];
  der1[0]=0.;
  der1[1]=0.;
  der1[2]=0.;


  for (int i=0;i<iBinFitWidth;++i) {
    //     double delta=a0+mv[i]*(a1+a2*mv[i])-hv[i];
    double delta=-hv[i];
    double der10=2*delta*invsigm2v[i];
    der1[0]+=der10;
    der1[1]+=der10*mv[i];
    der1[2]+=der10*std::pow(mv[i],2);
  }

  // fill the vector of second derivatives
  double der2[3][3];
  for (int i=0;i<3;++i){
    for (int j=0;j<3;++j){
      der2[i][j]=0.;
    }
  }

  for (int i=0;i<iBinFitWidth;++i) {
    double sca=2*invsigm2v[i];
    der2[0][0]+=sca;
    der2[0][1]+=sca*mv[i];
    der2[0][2]+=sca*std::pow(mv[i],2);
    der2[1][1]+=sca*std::pow(mv[i],2);
    der2[1][2]+=sca*std::pow(mv[i],3);
    der2[2][2]+=sca*std::pow(mv[i],4);
  }
  der2[1][0]=der2[0][1];
  der2[2][0]=der2[0][2];
  der2[2][1]=der2[1][2];

  // build root vector and matrix
  TVectorD tder1(3,&der1[0]);
  TMatrixD tder2(3,3,&der2[0][0]);
  // solve
  tder2.InvertFast();
  TVectorD asolve(3);// = tder2inv * tder1;
  asolve = tder2 * tder1;
  asolve*=-1.;
#endif   // else ifndef MAXFITTINGBYHAND



  double maxPosFit=-1.;


  if (int(fitRes)==0) {
#ifndef MAXFITTINGBYHAND
    //root fit
    histInfo[HistInfo::CHI2]=fitRes->Chi2();
    const double mMax=fitRes->Parameter(0);
    const double mMean=fitRes->Parameter(1);
    const double mInvWidth2=fitRes->Parameter(2);
    double mMaxError=fitRes->ParError(0);
    m_printmMaxError=mMaxError;
    double mMeanError=fitRes->ParError(1);
    m_printmMeanError=mMeanError;
    double mInvWidth2Error=fitRes->ParError(2);
    m_printmInvWidth2Error=mInvWidth2Error;
    mMeanError=0.; // avoid warning
    mInvWidth2Error=0.; // avoid warning
    const double c=mMax*(1-4*mMean*mMean*mInvWidth2);
    const double b=8*mMax*mMean*mInvWidth2;
    const double a=-4*mMax*mInvWidth2;
    // when built in polynomial fit
    // const double c=fitRes->Parameter(0);
    // const double b=fitRes->Parameter(1);
    // const double a=fitRes->Parameter(2);

#else
    // fit by hand
    // to be done : proper plot if required
    histInfo[HistInfo::CHI2]=0.;
    const double mMax=0.;
    const double mMean=0.;
    const double mInvWidth2=0.;
    double mMaxError=0.;
    m_printmMaxError=mMaxError;
    double mMeanError=0.;
    m_printmMeanError=mMeanError;
    double mInvWidth2Error=0.;
    m_printmInvWidth2Error=mInvWidth2Error;
    mMeanError=0.;
    mInvWidth2Error=0.;
    double a=asolve[2];
    double b=asolve[1];
    double c=asolve[0];

#endif // else ifndef MAXFITTINGBYHAND

    //     std::cout << "a=" << a << " b=" << b << " c=" << c << " m=" << -b/2/a << std::endl;

    const double h_discri=b*b - 4*a*c;
    histInfo[HistInfo::DISCRI]=h_discri;
    const double sqrth_discri=sqrt(h_discri);
    const double h_fitLength=sqrth_discri/a;
    histInfo[HistInfo::FITLENGTH]=h_fitLength;
    histInfo[HistInfo::TANTHETA]=2*a/sqrth_discri;
    histInfo[HistInfo::TANTHETAW]=2*a*sumw/sqrth_discri;
    histInfo[HistInfo::RMSVSDISCRI]=h_rms/h_fitLength;
    // compute maximum position (only if inverted parabola)
    if (a<0) maxPosFit=-b/(2*a);
  }

  // keep fit result only if within 80% of fit window, and fit succeeded
  if (maxPosFit>=0. and std::abs(maxPosFit-maxPosWin)<0.8*fitWidth){
    histInfo[HistInfo::PROB]=prob;
    return maxPosFit;
  } else{
    // otherwise keep the weighted average
    // negate prob just to flag such event
    prob=-prob;
    histInfo[HistInfo::PROB]=prob;
    return maxPosWin;
  }

}






// compute probability for any input value,can be called from a pure parameter scan
//deltametvec is along phijet
// returns number of solution if positive, return code if negative, vector of probability and mass
int MissingMassCalculator::probCalculatorV9fast(const double & phi1, const double & phi2,
                                                const double & M_nu1, const double & M_nu2){




  //  bool debug=true;


  int nsol1;
  int nsol2;




  //---------------------------------------------

  //------- Settings -------------------------------
  if ( M_nu1 > m_mTau-m_tauVec1M) return -1;

  if ( M_nu2 > m_mTau-m_tauVec2M) return -2;

  //-------- end of Settings


#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {

    std::cout << "probCalculatorV9Fast : phi1 " << phi1 << " phi2 " << phi2
              << " tauvec 1 " << m_tauVec1.Px() << " " << m_tauVec1.Py() << " " << m_tauVec1.Pz() << " " << m_tauVec1.E() << " " << m_tauVec1.M()
              << " tauvec 2 " << m_tauVec2.Px() << " " << m_tauVec2.Py() << " " << m_tauVec2.Pz() << " " << m_tauVec2.E() << " " << m_tauVec2.M()
              << " met " << m_metVec.X() << " " << m_metVec.Y() << " " <<  "Mnu1 " << M_nu1 << " Mnu2 " << M_nu2 << std::endl;
  }
#endif

  const int solution=NuPsolutionV3(M_nu1,M_nu2,phi1,phi2,nsol1,nsol2);

#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {
    std::cout << "probCalculatorV9Fast : solution " << solution << std::endl;
  }
#endif

  if(solution!=1) return -4;
  //  refineSolutions (            M_nu1,M_nu2,
  //                              met_smearL,met_smearP,metvec_tmp.Mod(),
  //                              nsol1, nsol2,m_Mvis,m_Meff);
  refineSolutions (                M_nu1,M_nu2,
                                   nsol1, nsol2,m_Mvis,m_Meff);

#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {
    std::cout << "refineSolutions : solution " << m_nsol << " nsol1:" << nsol1 << " nsol2:" << nsol2 << std::endl;
  }
#endif

  if (m_nsol<=0) return 0;


  //success

  return m_nsol; // for backward compatibility
}



// nuvecsol1 and nuvecsol2 passed by MMC
int MissingMassCalculator::refineSolutions (const double & M_nu1, const double & M_nu2,
                                            const int nsol1, const int nsol2,
                                            const double & Mvis, const double & Meff)

{
  const int  tau_type1 = m_preparedInput.type_visTau1;
  const int  tau_type2 = m_preparedInput.type_visTau2;

  m_nsol=0;

  if (int(m_probFinalSolVec.size())<m_nsolfinalmax) std::cout << "refineSolutions ERROR " << " probFinalSolVec.size() should be " << m_nsolfinalmax << std::endl;
  if (int(m_mtautauFinalSolVec.size())<m_nsolfinalmax) std::cout << "refineSolutions ERROR " << " mtautauSolVec.size() should be " << m_nsolfinalmax << std::endl;
  if (int(m_nu1FinalSolVec.size())<m_nsolfinalmax) std::cout << "refineSolutions ERROR " << " nu1FinalSolVec.size() should be " << m_nsolfinalmax << std::endl;
  if (int(m_nu2FinalSolVec.size())<m_nsolfinalmax) std::cout << "refineSolutions ERROR " << " nu1FinalSolVec.size() should be " << m_nsolfinalmax << std::endl;
  if (nsol1>int(m_nsolmax)) std::cout << "refineSolutions ERROR " << " nsol1 " << nsol1 << "  > nsolmax !" << m_nsolmax << std::endl;
  if (nsol2>int(m_nsolmax)) std::cout << "refineSolutions ERROR " << " nsol1 " << nsol2 << "  > nsolmax !" << m_nsolmax << std::endl;

  //  std::cout << "DRDR in refine solutions nsol1=" << nsol1 << " nsol2=" << nsol2 << std::endl ;

  //  m_iter2+=(nsol1*nsol2);
  int ngoodsol1=0;
  for(int j1=0; j1<nsol1; j1++)
    {
      double &tauvecprob1j  = m_tauvecprob1[j1];
      tauvecprob1j=0.;
      //SpeedUp reference to avoid picking again in again in array
      TLorentzVector & nuvec1_tmpj=m_nuvecsol1[j1];

#ifndef USEALLSOLUTION
      // take first or second solution
      // no time to call rndm, switch more or less randomely, according to an oscillating switch perturbed by m_phi1
      if(nsol1>1)  {
        if (j1==0)  {  // decide at the first solution which one we will take
          const int pickInt=std::abs(10000*m_Phi1);
          const int pickDigit=pickInt-10*(pickInt/10);
          if (pickDigit<5) m_switch1=!m_switch1;
        }
        m_switch1=!m_switch1;

        if (m_switch1) {
          //      std::cout << "DRDR1 skip solution " << j1 << " out of " << nsol1 << std::endl;
          continue;
        }
      }
#endif
      TLorentzVector & tauvecsol1j=m_tauvecsol1[j1];




      nuvec1_tmpj.SetXYZM(nuvec1_tmpj.Px(),
                          nuvec1_tmpj.Py(),
                          nuvec1_tmpj.Pz(),
                          M_nu1);



      tauvecsol1j.SetPxPyPzE(0.,0.,0.,0.);
      tauvecsol1j+=nuvec1_tmpj;
      tauvecsol1j+=m_tauVec1;

      if(tauvecsol1j.E()>=m_beamEnergy) continue;

      const double tau1_tmpp=tauvecsol1j.P();
      const double angle1=Angle(nuvec1_tmpj,m_tauVec1);
#ifdef USETAUVISFORPROB
      tauvecprob1j=dTheta3d_probabilityFast(tau_type1,angle1,m_tauVec1P);
#else
      tauvecprob1j=dTheta3d_probabilityFast(tau_type1,angle1,tau1_tmpp);
#endif

      ++ngoodsol1;
    }

#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {
    std::cout << "refineSolutions : ngoodsol1 " << ngoodsol1 << std::endl;
  }
#endif

  if (ngoodsol1==0)     {
    //    std::cout << "leaving refineSolution ngoodsol1==0" << std::endl;
    return -1;
  }

  int ngoodsol2=0;

  for(int j2=0; j2<nsol2; j2++)
    {
      double &tauvecprob2j = m_tauvecprob2[j2];
      tauvecprob2j=0.;

#ifndef USEALLSOLUTION
      // take first or second solution
      // no time to call rndm, switch more or less randomely, according to an oscillating switch perturbed by m_phi2
      if(nsol2>1)  {
        if (j2==0)  {  // decide at the first solution which one we will take
          const int pickInt=std::abs(10000*m_Phi2);
          const int pickDigit=pickInt-10*int(pickInt/10);
          if (pickDigit<5) m_switch2=!m_switch2;
        }
        m_switch2=!m_switch2;
        if (m_switch2) {
          //      std::cout << "DRDR2 skip solution " << j2 << " out of " << nsol2 << std::endl;
          continue;
        }
      }

#endif
      TLorentzVector & nuvec2_tmpj=m_nuvecsol2[j2];
      TLorentzVector & tauvecsol2j=m_tauvecsol2[j2];

      nuvec2_tmpj.SetXYZM(nuvec2_tmpj.Px(),
                          nuvec2_tmpj.Py(),
                          nuvec2_tmpj.Pz(),
                          M_nu2);


      tauvecsol2j.SetPxPyPzE(0.,0.,0.,0.);
      tauvecsol2j+=nuvec2_tmpj;
      tauvecsol2j+=m_tauVec2;
      if (tauvecsol2j.E()>=m_beamEnergy) continue;
      const double tau2_tmpp=tauvecsol2j.P();
      const double angle2=Angle(nuvec2_tmpj,m_tauVec2);
#ifdef USETAUVISFORPROB
      tauvecprob2j=dTheta3d_probabilityFast(tau_type2,angle2,m_tauVec2P);
#else
      tauvecprob2j=dTheta3d_probabilityFast(tau_type2,angle2,tau2_tmpp);
#endif
      ++ngoodsol2;

    }

#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {
    std::cout << "refineSolutions : ngoodsol1 " << ngoodsol1 << " ngoodsol2:" << ngoodsol2 << std::endl;
  }
#endif

  if (ngoodsol2==0)     {
    //    std::cout << "leaving refineSolution ngoodsol2==0" << std::endl;
    return -2;
  }


  double mettauprob=-1.;

  // now reloop
  for (int j1=0; j1<nsol1;++j1)
    {
      double &tauvecprob1j = m_tauvecprob1[j1];
      // quit already if this sol was no good
      if (tauvecprob1j==0.) continue;
      TLorentzVector & nuvec1_tmpj=m_nuvecsol1[j1];
      TLorentzVector & tauvecsol1j=m_tauvecsol1[j1];

      for (int j2=0; j2<nsol2;++j2)
        {
          double &tauvecprob2j = m_tauvecprob2[j2];
          // quit already if this sol was no good
          if (tauvecprob2j==0.) continue;
          TLorentzVector & nuvec2_tmpj=m_nuvecsol2[j2];
          TLorentzVector & tauvecsol2j=m_tauvecsol2[j2];

          //cannot happen if(totalProb<=0.0) continue;

          // ++m_iter4;
          m_tautau_tmp.SetPxPyPzE(0.,0.,0.,0.);
          m_tautau_tmp+=tauvecsol1j;
          m_tautau_tmp+=tauvecsol2j;
          const double mtautau=m_tautau_tmp.M();

          //
          if(TailCleanUp(tau_type1,m_tauVec1,nuvec1_tmpj,
                         tau_type2,m_tauVec2,nuvec2_tmpj,
                         mtautau,Mvis,Meff,m_preparedInput.m_DelPhiTT)==0) {
#ifdef DEBUGTHISITERATION
            if (m_debugThisIteration) {
              std::cout << "refineSolutions : exit tailcleanup" << std::endl;
            }
#endif

            continue;
          }

          // compute met probability only if not done before
          if (mettauprob==-1.) {
            if ( WALKSTRATEGY==WalkStrategy::RANDOMNONUNIF ){
              mettauprob=1. ;
            }else{

              mettauprob=mEtAndTauProbability();
            }
          }





          // also for h h
          double ditauProb=1.0;
          double mnuProb=1.0;
          if(m_preparedInput.UseHT || (tau_type1!=0 && tau_type2!=0 ))
            {
              ditauProb=TauProbability(tau_type1,m_tauVec1,nuvec1_tmpj,tau_type2,m_tauVec2,nuvec2_tmpj,m_DetMEt); // customized prob for Njet25=0
            }
          else
            {
              ditauProb=TauProbability(tau_type1,m_tauVec1,nuvec1_tmpj,tau_type2,m_tauVec2,nuvec2_tmpj);
            }

          if(m_fUseMnuProbability==1)
            {
              if(tau_type1==0 && tau_type2==0) mnuProb=MnuProbability(nuvec1_tmpj.M())*MnuProbability(nuvec2_tmpj.M()); // lep-lep
              if(tau_type1==0 && tau_type2!=0) mnuProb=MnuProbability(nuvec1_tmpj.M()); // lep-had: tau1==lepton
              if(tau_type1!=0 && tau_type2==0) mnuProb=MnuProbability(nuvec2_tmpj.M()); // lep-had: tau2==lepton
            }

          //      const double totalProb=m_walkWeight*tauvecprob1j*tauvecprob2j*mettauprob*ditauProb;
          const double totalProb=m_walkWeight*tauvecprob1j*tauvecprob2j*mettauprob*ditauProb*mnuProb;

#ifdef DUMPITERATION
          //add 100 to get positive values, because sort is confused by negative values
          //const int dumpInt=100;
          const int dumpInt=0;
          std::cout << "DEBUGV9W1 " << m_iter0
                    << " p1 " << m_Phi1+dumpInt
                    << " p2 " << m_Phi2+dumpInt
                    << " m1 "   << m_Mnu1+dumpInt
                    << " m2 "    << m_Mnu2+dumpInt
                    << " eL "   << m_MEtL+dumpInt
                    << " eP "    << m_MEtP+dumpInt
                    << " tob "  << totalProb
                    << " tb1 " << tauvecprob1j
                    << " tb2 " << tauvecprob2j
                    << " metb " << mettauprob
                    << " dtb " << ditauProb
                    << " i0 " << m_iter0
                    << std::endl;
#endif
#ifdef DEBUGTHISITERATION
          //debugThisIteration=(totalProb>1e3);

          if (m_debugThisIteration) {

            std::cout << " refineSolutions from TauProbability "
                      << "tau_type1 " << tau_type1 << " tauVec1.E" << m_tauVec1.E() << " tauVec1.M" << m_tauVec1.M()
                      << "tau_type2 " << tau_type2 << " tauVec2.E" << m_tauVec2.E() << " tauVec2.M" << m_tauVec2.M()
                      << " nu_vec1.E" << nuvec1_tmpj.E() << " nu_vec1.M" << nuvec1_tmpj.M()
                      << " nu_vec2.E" << nuvec2_tmpj.E()  << " nu_vec2.M" << nuvec2_tmpj.M()
                      << "ditauprob " << ditauProb  << std::endl;
            std::cout << "refineSolutions "
                      << " p1 " << m_Phi1
                      << " p2 " << m_Phi2
                      << " m1 "   << m_Mnu1
                      << " m2 "    << m_Mnu2
                      << " eL "   << m_MEtL
                      << " eP "    << m_MEtP
                      << std::endl;

            std::cout << "refineSolutions : totalProb "  << totalProb << " tauprob1 " << tauvecprob1j << " tauprob2 " << tauvecprob2j << " mettauprob " << mettauprob << " ditauProb " << ditauProb << std::endl;
          }
#endif

          if (totalProb<=0)  {
            //      std::cout << " WARNING null proba solution, rejected " << totalProb << std::endl;
          } else  {
            // only count solution with non zero probability
            m_totalProbSum+=totalProb;
            m_mtautauSum+=mtautau;

            if (m_nsol>=int(m_nsolfinalmax)) {
              std::cout << "refineSolutions ERROR nsol getting larger than nsolfinalmax!!! " << m_nsol << std::endl;
              std::cout << " j1 " << j1 << " j2 " << j2 << " nsol1 " << nsol1 << " nsol2 " << nsol2 << std::endl;
              --m_nsol; //overwrite last solution. However this should really never happen
            }

            // good solution found, copy in vector
            m_mtautauFinalSolVec[m_nsol]=mtautau;
            m_probFinalSolVec[m_nsol]=totalProb;

            TLorentzVector & nu1Final = m_nu1FinalSolVec[m_nsol];
            TLorentzVector & nu2Final = m_nu2FinalSolVec[m_nsol];
            //      for (int iv=0;iv<4;++iv){

            nu1Final.SetPxPyPzE(nuvec1_tmpj.Px(),nuvec1_tmpj.Py(),nuvec1_tmpj.Pz(),nuvec1_tmpj.E());
            nu2Final.SetPxPyPzE(nuvec2_tmpj.Px(),nuvec2_tmpj.Py(),nuvec2_tmpj.Pz(),nuvec2_tmpj.E());
            // }


            ++m_nsol;
          }  //else totalProb<=0

        } // loop j2
    }  // loop j1
  //  std::cout << "DRDR leaving refine solutions with sol:" << m_nsol << std::endl ;
  return m_nsol;
}

int MissingMassCalculator::TailCleanUp(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1,
                                       const int & type2, const TLorentzVector & vis2, const TLorentzVector & nu2,
                                       const double & mmc_mass, const double & vis_mass, const double & eff_mass,
                                       const double & dphiTT) {

#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {
    std::cout << "tailcleanup : enter" << std::endl;
  }
#endif


  int pass_code=1;
  if(m_fUseTailCleanup==0) return pass_code;

#ifdef DEBUGTHISITERATION
  if (m_debugThisIteration) {
    std::cout << "tailcleanup : will be done" << std::endl;
  }
#endif

  //the Clean-up cuts are specifically for rel16 analyses.
  // the will change in rel17 analyses and after the MMC is updated

  if(type1==0 && type2==0) // lepton-lepton channel
    {
      const double MrecoMvis=mmc_mass/vis_mass;
      if(MrecoMvis>2.6) return 0;
      const double MrecoMeff=mmc_mass/eff_mass;
      if(MrecoMeff>1.9) return 0;
      const double e1p1=nu1.E()/vis1.P();
      const double e2p2=nu2.E()/vis2.P();
      if((e1p1+e2p2)>4.5) return 0;
      if(e2p2>4.0) return 0;
      if(e1p1>3.0) return 0;
    }



  //-------- these are new cuts for lep-had analysis for Moriond
  if((type1==0 || type2==0) && (type1+type2>0)) // lepton-hadron channel
    {

      if (m_mmcCalibrationSet==MMCCalibrationSet::MMC2012
          || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015
          || m_mmcCalibrationSet==MMCCalibrationSet::MMC2015HIGHMASS
          || m_mmcCalibrationSet==MMCCalibrationSet::MMC2016MC15C
          || m_mmcCalibrationSet==MMCCalibrationSet::UPGRADE)
        return pass_code; // don't use TailCleanup for 8 & 13 TeV data

      //--------- leave code uncommented to avoid Compilation warnings
      if(m_preparedInput.UseHT)
        {
          const double MrecoMvis=mmc_mass/vis_mass;
          const double MrecoMeff=mmc_mass/eff_mass;
          const double x=dphiTT>1.5 ? dphiTT : 1.5;
          if((MrecoMeff+MrecoMvis)>5.908-1.881*x+0.2995*x*x)
            return 0;
        }
    }

  // if(type1>0 && type2>0) // hadron-hadron channel : no tail cleanup
  //   {
  //     const double MrecoMvis=mmc_mass/vis_mass;
  //     if(MrecoMvis>1.7) return 0;
  //     const double e1p1=nu1.E()/vis1.P();
  //     const double e2p2=nu2.E()/vis2.P();
  //     if((e1p1+e2p2)>1.5) return 0;
  //     if(e2p2>1.2) return 0;
  //   }

  return pass_code;
}

//------------------- simple TauProbability for LFV
double MissingMassCalculator::TauProbabilityLFV(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1)
{
  double prob=1.0;
  if(m_fUseTauProbability==0) return prob; // don't apply TauProbability
  double prob1=1.0;
  const double mtau=1.777;
  const double R1=nu1.E()/vis1.E();
  //--- dealing with 1st tau
  double m1=nu1.M();
  double m2=vis1.M();
  double E1=0.5*(mtau*mtau+m1*m1-m2*m2)/mtau;
  double E2=mtau-E1;
  if(E1<=m1 || E1>=mtau)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::TauProbability: bad E1, returning 0 "<<std::endl;
      return 0.0;
    }
  if(E2<=m2 || E2>=mtau)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::TauProbability: bad E2, returning 0 "<<std::endl;
      return 0.0;
    }
  m_tlv_tmp.SetPxPyPzE(0.,0.,0.,0.);
  m_tlv_tmp+=nu1;
  m_tlv_tmp+=vis1;
  //  double p=(nu1+vis1).P();
  double p=m_tlv_tmp.P();
  double V=p/sqrt(p*p+mtau*mtau);
  double p0;
  if(type1==0) p0=sqrt(E2*E2-m2*m2); // leptonic tau
  else p0=E1; // hadronic tau
  prob1=0.5*mtau/(p0*V*pow(R1+1.0,2));
  // avoid too large values
  prob=std::min(prob1,1.);
  return prob;
}


double MissingMassCalculator::TauProbability(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1,
                                             const int & type2, const TLorentzVector & vis2, const TLorentzVector & nu2)
{
  double prob=1.0;
  if(m_fUseTauProbability==0) return prob; // don't apply TauProbability
  double prob1=1.0;
  double prob2=1.0;
  const double mtau=1.777;
  const double R1=nu1.E()/vis1.E();
  const double R2=nu2.E()/vis2.E();
  //--- dealing with 1st tau
  double m1=nu1.M();
  double m2=vis1.M();
  double E1=0.5*(mtau*mtau+m1*m1-m2*m2)/mtau;
  double E2=mtau-E1;
  if(E1<=m1 || E1>=mtau)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::TauProbability: bad E1, returning 0 "<<std::endl;
      return 0.0;
    }
  if(E2<=m2 || E2>=mtau)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::TauProbability: bad E2, returning 0 "<<std::endl;
      return 0.0;
    }
  m_tlv_tmp.SetPxPyPzE(0.,0.,0.,0.);
  m_tlv_tmp+=nu1;
  m_tlv_tmp+=vis1;
  //  double p=(nu1+vis1).P();
  double p=m_tlv_tmp.P();
  double V=p/sqrt(p*p+mtau*mtau);
  double p0;
  if(type1==0) p0=sqrt(E2*E2-m2*m2); // leptonic tau
  else p0=E1; // hadronic tau
  prob1=0.5*mtau/(p0*V*pow(R1+1.0,2));
  // avoid too large values
  prob1=std::min(prob1,1.);


  //--- dealing with 2nd tau
  m1=nu2.M();
  m2=vis2.M();
  E1=0.5*(mtau*mtau+m1*m1-m2*m2)/mtau;
  E2=mtau-E1;
  if(E1<=m1 || E1>=mtau)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::TauProbability: bad E1, returning 0 "<<std::endl;
      return 0.0;
    }
  if(E2<=m2 || E2>=mtau)
    {
      if(m_fUseVerbose==1) std::cout<<" Warning in MissingMassCalculator::TauProbability: bad E2, returning 0 "<<std::endl;
      return 0.0;
    }
  m_tlv_tmp.SetPxPyPzE(0.,0.,0.,0.);
  m_tlv_tmp+=nu2;
  m_tlv_tmp+=vis2;
  //  p=(nu2+vis2).P();
  p=m_tlv_tmp.P();


  V=p/sqrt(p*p+mtau*mtau);
  if(type2==0) p0=sqrt(E2*E2-m2*m2); // leptonic tau
  else p0=E1; // hadronic tau
  prob2=0.5*mtau/(p0*V*pow(R2+1.0,2));
  // avoid too large values
  prob2=std::min(prob2,1.);
  prob=prob1*prob2;
  return prob;
}


// --------- Updated version of TauProbability for lep-had events with Njet25=0, takes into account Winter-2012 analysis cuts
double MissingMassCalculator::TauProbability(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1,
                                             const int & type2, const TLorentzVector & vis2, const TLorentzVector & nu2, const double & detmet) {
  double prob=1.0;

  // #ifdef  TAUESCAN
  //   return prob;
  // #endif
  if(m_fUseTauProbability==0) return prob; // don't apply TauProbability

  if(m_preparedInput.UseHT)
    {
      if(detmet<20.0) // low MET Njet=0 category
        {
          const double R1=nu1.P()/vis1.P();
          const double R2=nu2.P()/vis2.P();
          const double lep_p1[4]={0.417,0.64,0.52,0.678};
          const double lep_p2[4]={0.23,0.17,0.315,0.319};
          const double lep_p3[4]={0.18,0.33,0.41,0.299};
          const double lep_p4[4]={0.033,0.109,0.129,0.096};
          const double lep_p5[4]={0.145,0.107,0.259,0.304};
          int ind=3;
          int indT=3;
          const double n_1pr[4]={-0.15,-0.13,-0.25,-0.114};
          const double s_1pr[4]={0.40,0.54,0.62,0.57};
          const double n_3pr[4]={-1.08,-1.57,-0.46,-0.39};
          const double s_3pr[4]={0.53,0.85,0.61,0.53};
          double Ptau=0.0;
          double Plep=0.0;
          if(type1==1 || type1==3)
            {
              Ptau=(nu1+vis1).P();
              Plep=(nu2+vis2).P();
            }
          if(type2==1 || type2==3)
            {
              Ptau=(nu2+vis2).P();
              Plep=(nu1+vis1).P();
            }
          if(Plep<50.0 && Plep>=45.0) ind=2;
          if(Plep<45.0 && Plep>=40.0) ind=1;
          if(Plep<40.0) ind=0;
          if(Ptau<50.0 && Ptau>=45.0) indT=2;
          if(Ptau<45.0 && Ptau>=40.0) indT=1;
          if(Ptau<40.0) indT=0;
          if(type1==0) prob=prob*(lep_p5[ind]*TMath::Gaus(R1,lep_p1[ind],lep_p2[ind])+TMath::Landau(R1,lep_p3[ind],lep_p4[ind]))/(1+lep_p5[ind]);
          if(type2==0) prob=prob*(lep_p5[ind]*TMath::Gaus(R2,lep_p1[ind],lep_p2[ind])+TMath::Landau(R2,lep_p3[ind],lep_p4[ind]))/(1+lep_p5[ind]);

          // #ifdef  TAUESCAN
          //      return prob;
          // #endif

          if(type1==1) prob=prob*TMath::Gaus(R1,n_1pr[indT],s_1pr[indT]);
          if(type2==1) prob=prob*TMath::Gaus(R2,n_1pr[indT],s_1pr[indT]);
          if(type1==3) prob=prob*TMath::Gaus(R1,n_3pr[indT],s_3pr[indT]);
          if(type2==3) prob=prob*TMath::Gaus(R2,n_3pr[indT],s_3pr[indT]);

        }
      else // high MET Njet=0 category
        {
          const double R1=nu1.P()/vis1.P();
          const double R2=nu2.P()/vis2.P();
          const double lep_p1[4]={0.441,0.64,0.79,0.8692};
          const double lep_p2[4]={0.218,0.28,0.29,0.3304};
          const double lep_p3[4]={0.256,0.33,0.395,0.4105};
          const double lep_p4[4]={0.048,0.072,0.148,0.1335};
          const double lep_p5[4]={0.25,0.68,0.10,0.2872};
          int ind=3;
          const double p_1prong=-3.706;
          const double p_3prong=-5.845;
          double Ptau=0.0;
          double Plep=0.0;
          if(type1==1 || type1==3)
            {
              Ptau=(nu1+vis1).P();
              Plep=(nu2+vis2).P();
            }
          if(type2==1 || type2==3)
            {
              Ptau=(nu2+vis2).P();
              Plep=(nu1+vis1).P();
            }
          if(Plep<50.0 && Plep>=45.0) ind=2;
          if(Plep<45.0 && Plep>=40.0) ind=1;
          if(Plep<40.0) ind=0;
          const double scale1prong=Ptau>45.0 ? 1.0 : -1.019/((Ptau*0.0074-0.059)*p_1prong);
          const double scale3prong=Ptau>40.0 ? 1.0 : -1.24/((Ptau*0.0062-0.033)*p_3prong);
          if(type1==0) prob=prob*(lep_p5[ind]*TMath::Gaus(R1,lep_p1[ind],lep_p2[ind])+TMath::Landau(R1,lep_p3[ind],lep_p4[ind]))/(1+lep_p5[ind]);
          if(type2==0) prob=prob*(lep_p5[ind]*TMath::Gaus(R2,lep_p1[ind],lep_p2[ind])+TMath::Landau(R2,lep_p3[ind],lep_p4[ind]))/(1+lep_p5[ind]);

          // #ifdef  TAUESCAN
          //      return prob;
          // #endif

          if(type1==1) prob=prob*exp(p_1prong*R1*scale1prong)*std::abs(p_1prong*scale1prong)*0.02; // introduced normalization to account for sharpening of probability at low E(tau)
          if(type2==1) prob=prob*exp(p_1prong*R2*scale1prong)*std::abs(p_1prong*scale1prong)*0.02;
          if(type1==3) prob=prob*exp(p_3prong*R1*scale3prong)*std::abs(p_3prong*scale3prong)*0.02;
          if(type2==3) prob=prob*exp(p_3prong*R2*scale3prong)*std::abs(p_3prong*scale3prong)*0.02;
        }
    }
  //----------------- had-had channel ---------------------------------------
  if(type1>0 && type2>0)
    {

      // #ifdef  TAUESCAN
      //          return prob;
      // #endif

      if(m_preparedInput.UseHT) // Events with no jets
        {

          const double R[2]={nu1.P()/vis1.P(),nu2.P()/vis2.P()};
          const double E[2]={(nu1+vis1).E(),(nu2+vis2).E()};
          const int tau_type[2]={type1,type2};
          int order1= vis1.Pt()>vis2.Pt() ? 0 : 1;
          int order2= vis1.Pt()>vis2.Pt() ? 1 : 0;

          double par_1p[2][6]; // P(tau)-scaling; lead, sub-lead
          double par_3p[2][6]; // P(tau)-scaling; lead, sub-lead

          par_1p[0][0]=0.1273; par_1p[0][1]=-0.2479; par_1p[0][2]=1.0; par_1p[0][3]=-43.16; par_1p[0][4]=0.0; par_1p[0][5]=0.0;
          par_1p[1][0]=0.3736; par_1p[1][1]=-1.441; par_1p[1][2]=1.0; par_1p[1][3]=-29.82; par_1p[1][4]=0.0; par_1p[1][5]=0.0;
          par_3p[0][0]=0.1167; par_3p[0][1]=-0.1388; par_3p[0][2]=1.0; par_3p[0][3]=-44.77; par_3p[0][4]=0.0; par_3p[0][5]=0.0;
          par_3p[1][0]=0.3056; par_3p[1][1]=-2.18; par_3p[1][2]=1.0; par_3p[1][3]=-19.09; par_3p[1][4]=0.0; par_3p[1][5]=0.0;
          // parameters for sub-leading tau
          const double C1p=0.062;
          const double C3p=0.052;
          const double G1p=1.055;
          const double G3p=1.093;
          // Probability for leading tau
          //      std::cout << "DRDR no jets " << prob << std::endl;

          if(tau_type[order1]==1) // 1-prong
            {
              //double x=std::min(300.,std::max(E[order1],45.0));
              // 50 to be safe. TO be finalised.
              //              double x=std::min(300.,std::max(E[order1],50.0));
              double x=std::min(E[order1],300.0);
              const double slope=par_1p[0][0]+par_1p[0][1]/(par_1p[0][2]*x+par_1p[0][3])+par_1p[0][4]*x > 0.01 ?
                par_1p[0][0]+par_1p[0][1]/(par_1p[0][2]*x+par_1p[0][3])+par_1p[0][4]*x : 0.01;
              prob=prob*exp(-R[order1]/slope)*0.04/std::abs(slope);
            }
          if(tau_type[order1]==3) // 3-prong
            {
              //double x=std::min(300.,std::max(E[order1],45.0));
              //              double x=std::min(300.,std::max(E[order1],50.0));
              double x=std::min(E[order1],300.0);
              const double slope=par_3p[0][0]+par_3p[0][1]/(par_3p[0][2]*x+par_3p[0][3])+par_3p[0][4]*x > 0.01 ?
                par_3p[0][0]+par_3p[0][1]/(par_3p[0][2]*x+par_3p[0][3])+par_3p[0][4]*x : 0.01;
              prob=prob*exp(-R[order1]/slope)*0.04/std::abs(slope);
            }
          // Probability for sub-leading tau
          if(tau_type[order2]==1) // 1-prong
            {
              const double par[4]={0.1147,-0.09675,-35.0,3.711E-11};
              double x=std::min(300.,std::max(E[order2],30.0));
              //              double x=std::min(300.,std::max(E[order2],50.0));
              const double sigma=G1p*(par_1p[1][0]+par_1p[1][1]/(par_1p[1][2]*x+par_1p[1][3])+par_1p[1][4]*x+par_1p[1][5]*x*x) > 0.01 ?
                G1p*(par_1p[1][0]+par_1p[1][1]/(par_1p[1][2]*x+par_1p[1][3])+par_1p[1][4]*x+par_1p[1][5]*x*x) : 0.01;
              if(x<36.0) x=36.0;
              const double mean=par[0]+par[1]/(x+par[2])+par[3]*pow(x,4);
              prob=prob*C1p*TMath::Gaus(R[order2],mean,sigma);
            }
          if(tau_type[order2]==3) // 3-prong
            {
              double x=std::min(300.,std::max(E[order2],20.0));
              //              double x=std::min(300.,std::max(E[order2],50.0));
              const double sigma=G3p*(par_3p[1][0]+par_3p[1][1]/(par_3p[1][2]*x+par_3p[1][3])+par_3p[1][4]*x+par_3p[1][5]*x*x) > 0.01 ?
                G3p*(par_3p[1][0]+par_3p[1][1]/(par_3p[1][2]*x+par_3p[1][3])+par_3p[1][4]*x+par_3p[1][5]*x*x) : 0.01;
              const double par[4]={0.2302,-2.012,-36.08,-0.000373};
              if(x<37.0) x=37.0;
              const double mean=par[0]+par[1]/(x+par[2])+par[3]*x;
              prob=prob*C3p*TMath::Gaus(R[order2],mean,sigma);
            }
        }
      if(!m_preparedInput.UseHT) // Events with jets
        {
          const double R1=nu1.P()/vis1.P();
          const double R2=nu2.P()/vis2.P();
          const double E1=(nu1+vis1).E();
          const double E2=(nu2+vis2).E();
          int order1= vis1.Pt()>vis2.Pt() ? 0 : 1;
          int order2= vis1.Pt()>vis2.Pt() ? 1 : 0;
          const double slope_1p[2]={-3.185,-2.052}; // lead, sub-lead
          const double slope_3p[2]={-3.876,-2.853}; // lead, sub-lead
          double par_1p[2][3]; // scaling below 100 GeV; lead, sub-lead
          double par_3p[2][3]; // scaling below 100 GeV; lead, sub-lead
          par_1p[0][0]=-0.3745; par_1p[0][1]=0.01417; par_1p[0][2]=-7.285E-5; // [0][i] is always adjusted to match slope at 100 GeV
          par_1p[1][0]=-0.3683; par_1p[1][1]=0.01807; par_1p[1][2]=-9.514E-5;
          par_3p[0][0]=-0.3055; par_3p[0][1]=0.01149; par_3p[0][2]=-5.855E-5;
          par_3p[1][0]=-0.3410; par_3p[1][1]=0.01638; par_3p[1][2]=-9.465E-5;
          double scale1;
          double scale2;
          if(type1==1) // 1-prong
            {
              scale1=E1>100.0 ? 1.0 : 1.0/std::abs((par_1p[order1][0]+par_1p[order1][1]*E1+par_1p[order1][2]*E1*E1)*slope_1p[order1]);
              if(scale1<1.0) scale1=1.0;
              if(scale1>100.0) scale1=100.0;
              prob=prob*std::abs(slope_1p[order1])*scale1*exp(slope_1p[order1]*scale1*R1)*0.04;
            }
          if(type1==3) // 3-prong
            {
              scale1=E1>100.0 ? 1.0 : 1.0/std::abs((par_3p[order1][0]+par_3p[order1][1]*E1+par_3p[order1][2]*E1*E1)*slope_3p[order1]);
              if(scale1<1.0) scale1=1.0;
              if(scale1>100.0) scale1=100.0;
              prob=prob*std::abs(slope_3p[order1])*scale1*exp(slope_3p[order1]*scale1*R1)*0.04;
            }
          if(type2==1) // 1-prong
            {
              scale2=E2>100.0 ? 1.0 : 1.0/std::abs((par_1p[order2][0]+par_1p[order2][1]*E2+par_1p[order2][2]*E2*E2)*slope_1p[order2]);
              if(scale2<1.0) scale2=1.0;
              if(scale2>100.0) scale2=100.0;
              prob=prob*std::abs(slope_1p[order2])*scale2*exp(slope_1p[order2]*scale2*R2)*0.04;
            }
          if(type2==3) // 3-prong
            {
              scale2=E2>100.0 ? 1.0 : 1.0/std::abs((par_3p[order2][0]+par_3p[order2][1]*E2+par_3p[order2][2]*E2*E2)*slope_3p[order2]);
              if(scale2<1.0) scale2=1.0;
              if(scale2>100.0) scale2=100.0;
              prob=prob*std::abs(slope_3p[order2])*scale2*exp(slope_3p[order2]*scale2*R2)*0.04;

            }
        }

    }
  //   prob=std::min(prob,1.); // Sasha commented out this line, it was introduced by David. Have to ask about its purpose.

  return prob;
}


//-------- New function for ditauprob like Weizman/Freiburg guys --------//

double MissingMassCalculator::TauProbabilityMatrix(const int & type1, const TLorentzVector & tau1, const TLorentzVector & nu1,
                                                   const int & type2, const TLorentzVector & tau2, const TLorentzVector & nu2) {

  double prob=0;
  double prob1,prob2;

  // Variables

  double mNu1 = nu1.M();
  double mNu2 = nu2.M();
  double eNu1 = nu1.E();
  double eNu2 = nu1.E();
  double pNu1 = nu1.P();
  double pNu2 = nu2.P();

  double mTau1 = tau1.M();
  double mTau2 = tau2.M();
  double eTau1 = tau1.E();
  double eTau2 = tau2.E();
  double pTau1 = tau1.P();
  double pTau2 = tau2.P();

  double fcut=0.15;
  double ct1,ct2;

  double a1 = Angle(tau1,nu1);
  double a2 = Angle(tau2,nu2);

  double ca1,ca2,sa1,sa2;

  fastSinCos(a1,sa1,ca1);
  fastSinCos(a2,sa2,ca2);

  // Calculation of costhetastar ct
  //    costThetaStar=(ca*etau*pn - en*ptau)/
  //   Sqrt(Power(ca,2)*Power(etau,2)*Power(pn,2) + Power(mtau,2)*Power(pn,2) -
  //     Power(ca,2)*Power(mtau,2)*Power(pn,2) - 2*ca*en*etau*pn*ptau +
  //     Power(en,2)*Power(ptau,2))

  ct1 = (ca1*eTau1*pNu1 - eNu1*pTau1)/(sqrt(ca1*ca1*eTau1*eTau1*pNu1*pNu1 + mTau1*mTau1*pNu1*pNu1 - ca1*ca1*mTau1*mTau1*pNu1*pNu1 - 2*ca1*eNu1*eTau1*pNu1*pTau1 + eNu1*eNu1*pTau1*pTau1));

  ct2 = (ca2*eTau2*pNu2 - eNu2*pTau2)/(sqrt(ca2*ca2*eTau2*eTau2*pNu2*pNu2 + mTau2*mTau2*pNu2*pNu2 - ca2*ca2*mTau2*mTau2*pNu2*pNu2 - 2*ca2*eNu2*eTau2*pNu2*pTau2 + eNu2*eNu2*pTau2*pTau2));

  // Calculation of probability
  // weight=std::abs(x*(1 - x^2)*(1 + x^2 - 2*x^4)*(1 - fcut*(1 - x^2))*(4 - 5*ct));

  double x1 = mNu1/mTau1;
  double x2 = mNu2/mTau2;

  if(type1==0 && type2==0)
    {
      prob1 = std::abs(x1 * (1-std::pow(x1,2)) * (1+std::pow(x1,2)-2*std::pow(x1,4)) * (1-fcut*(1-std::pow(x1,2))*(4-5*ct1)));
      prob2 = std::abs(x2 * (1-std::pow(x2,2)) * (1+std::pow(x2,2)-2*std::pow(x2,4)) * (1-fcut*(1-std::pow(x2,2))*(4-5*ct2)));
    }

  if(type1>0 && type2==0)
    {
      prob1 = 1;
      prob2 = std::abs(x2 * (1-std::pow(x2,2)) * (1+std::pow(x2,2)-2*std::pow(x2,4)) * (1-fcut*(1-std::pow(x2,2))*(4-5*ct2)));
    }

  if(type1==0 && type2>0)
    {
      prob1 = std::abs(x1 * (1-std::pow(x1,2)) * (1+std::pow(x1,2)-2*std::pow(x1,4)) * (1-fcut*(1-std::pow(x1,2))*(4-5*ct1)));
      prob2 = 1;
    }

  prob=prob1*prob2;

  return prob;
}


//-------- This function applies correction to compensate for the off-set
double MissingMassCalculator::MassScale(int method, double mass, const int & tau_type1, const int & tau_type2) {
  double Fscale=1.0;
  // calibration for rel16 lep-had analysis only
  if(m_fApplyMassScale==1)
    {
      if((tau_type1+tau_type2)>0 && (tau_type1==0 || tau_type2==0))
        {
          if(method!=1) return 1.0;
          //      float p0, p1, p2, p3;
          //      if(tau_type1==1 || tau_type2==1) // 1-prong tau's
          //        {
          //          p0=3.014; p1=-71.86; p2=1.018; p3=0.8912;
          //          if(mass>91.2) Fscale=p0/(p1+p2*mass)+p3;
          //          else Fscale=p0/(p1+p2*91.2)+p3;
          //        }
          //      if(tau_type1==3 || tau_type2==3) // 3-prong tau's
          //        {
          //          p0=0.4576; p1=-84.22; p2=0.9783; p3=0.9136;
          //          if(mass>91.2) Fscale=p0/(p1+p2*mass)+p3;
          //          else Fscale=p0/(p1+p2*91.2)+p3;
          //        }
          //      if(Fscale>1.0) Fscale=1.0;
          //      if(Fscale<0.89) Fscale=0.89;

          float p0, p1, p2, p3, p4, p5, p6, p7;
          if(tau_type1==1 || tau_type2==1) return 1.0; // 1-prong tau's
          if(tau_type1==3 || tau_type2==3) // 3-prong tau's
            {
              p0=3.014; p1=-71.86; p2=1.018; p3=0.8912;
              p4=0.4576; p5=-84.22; p6=0.9783; p7=0.9136;
              double scale1=p0/(p1+p2*mass)+p3;
              double scale3=p4/(p5+p6*mass)+p7;
              if(mass>91.2) Fscale=scale3/scale1;
              else
                {
                  scale1=p0/(p1+p2*91.2)+p3;
                  scale3=p4/(p5+p6*91.2)+p7;
                  Fscale=scale3/scale1;
                }
            }
          if(Fscale>1.0) Fscale=1.0;
          if(Fscale<0.95) Fscale=0.95;
        }
    }
  return 1.0/Fscale;
}


// note that if MarkovChain the input solutions can be modified
void MissingMassCalculator::handleSolutions()

{

  bool reject=true;
  double totalProbSumSol=0.;
  double totalProbSumSolOld=0.;
  bool firstPointWithSol=false;

  for (int isol=0;isol<m_nsol;++isol){
    totalProbSumSol  += m_probFinalSolVec[isol];
  }

  if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
    double uMC=-1.;
    bool notSureToKeep=true;
    // note : if no solution, the point is treated as having a zero probability
    if (m_fullParamSpaceScan) {
      reject=false; // accept anyway in this mode
      notSureToKeep=false; // do not need to test on prob
      if (m_nsol<=0) {
        // if initial full scaning and no sol : continue
        m_markovNFullScan+=1;
      }
      else
        {
          // if we were in in full scan mode and we have a solution, switch it off
          m_fullParamSpaceScan=false;
          firstPointWithSol=true; // as this is the first point without a solution there is no old sol
          m_iter0=0; // reset the counter so that separately the full scan pphase and the markov phase use m_NiterRandomLocal points
          // hack for hh : allow 10 times less iteration for markov than for the fullscan phase
          if (m_preparedInput.type_visTau1!=0 && m_preparedInput.type_visTau2!=0){
            m_niterRandomLocal/=10;
          }

        }
    }


    if (notSureToKeep)
      {
        // apply Metropolis algorithm to decide to keep this point.
        // compute the probability of the previous point and the current one
        for (int isol=0;isol<m_nsolOld;++isol){
          totalProbSumSolOld  +=m_probFinalSolOldVec[isol];
        }

        // accept anyway if null old probability (should only happen for the very first point with a solution)
        if (!firstPointWithSol  && totalProbSumSolOld<=0. ) {
          std::cout << " ERROR null old probability !!! " << totalProbSumSolOld << " nsolOld " << m_nsolOld << std::endl;
          reject=false;
        }
        else if (totalProbSumSol > totalProbSumSolOld) {
          // if going up, accept anyway
          reject=false;
          // else if (totalProbSumSol < 1E-16) { // if null target probability, reject anyway
        } else if (totalProbSumSol < totalProbSumSolOld*1E-6) { // if ratio of probability <1e6, point will be accepted only  every 1E6 iteration, so can reject anyway
          reject=true;
        } else  {
          // if going down, reject with a probability 1-totalProbSum/totalProbSumOld)
          uMC=m_randomGen->Rndm();
          reject= (uMC > totalProbSumSol/totalProbSumSolOld);
        }
      } // if reject

    // proceed with the handling of the solutions wether the old or the new ones
#ifdef DUMPITERATION
    std::cout << "DEBUGV9W MARKOV " << m_iter0
              << " full " << m_fullParamSpaceScan
              << " op " << totalProbSumSolOld
              << " np " << totalProbSumSol
              << " rej " << reject
              << " first " << firstPointWithSol
              << " ndupl " <<      m_markovCountDuplicate
              << " u " << uMC
              << " nos " << m_nsolOld
              << " ns " << m_nsol
              << " m_MEtP0 " << m_MEtP0
              << " m_MEtP " << m_MEtP
              << std::endl;
#endif

  } // if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN)


  // optionally fill the vectors with the complete list of points (for all walkstrategy)

#ifdef FILLPOINTSOUTPUT
  // if only include the point with solutions (recommended)
  double aProb=-999;
  double amtautau=-999.;

  int pCode=0;
  if (!checkAllParamInRange())  pCode=-10;

  // fill the vector of points (for mtautau take only first sol)
  if (m_nsol>0)   {
    aProb=totalProbSumSol;
    amtautau=m_mtautauFinalSolVec.at(0);
    pCode=10;
  }
  if (not reject) pCode+=1;

#ifdef FILLPOINTSOUTPUTONLYSOL
  if (m_nsol>0) {
#endif

    m_aPOI.prob.push_back(aProb);
    m_aPOI.mtautau.push_back(amtautau);
    m_aPOI.Phi1.push_back(m_Phi1);
    m_aPOI.Phi2.push_back(m_Phi2);
    m_aPOI.MEtP.push_back(m_MEtP);
    m_aPOI.MEtL.push_back(m_MEtL);
    m_aPOI.MEtX.push_back(m_MEtX);
    m_aPOI.MEtY.push_back(m_MEtY);
    m_aPOI.Mnu1.push_back(m_Mnu1);
    m_aPOI.Mnu2.push_back(m_Mnu2);
    m_aPOI.eTau1.push_back(m_eTau1);
    m_aPOI.eTau2.push_back(m_eTau2);
    m_aPOI.pCode.push_back(pCode);
    m_aPOI.index.push_back(m_apoiIndex);
    ++m_apoiIndex;
#ifdef FILLPOINTSOUTPUTONLYSOL
  }
#endif

#endif


  if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN){
    if (reject) {
      // current point reset to the previous one
      // Note : only place where m_MEtP etc... are modified outside spacewalkerXYZ
      m_MEtP=m_MEtP0;
      m_MEtL=m_MEtL0;
      m_Phi1=m_Phi10;
      m_Phi2=m_Phi20;
      m_eTau1=m_eTau10;
      m_eTau2=m_eTau20;
      if (m_scanMnu1) m_Mnu1=m_Mnu10;
      if (m_scanMnu2) m_Mnu2=m_Mnu20;
    }
  } // if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN)




  // default case : fill the histogram with solution, using current point
  bool fillSolution=true;
  bool oldToBeUsed=false;


  // now handle the reject or accept cases
  // the tricky thing is that for markov, we accept the old point as soon as a new accepted point is found
  // with a weight equal to one plus the number of rejected point inbetween

  if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
    if (reject) {
      fillSolution=false; // do not fill solution, just count number of replication
      m_markovCountDuplicate+=1;
      if (m_nsol<=0)
        {
          m_markovNRejectNoSol+=1;
        }
      else
        {
          m_markovNRejectMetropolis+=1;
        }

    } else {
      // if accept, will fill solution (except for very first point)  but taking the values from the previous point
      if (!m_fullParamSpaceScan) {
        m_markovNAccept+=1;
      }
      if (!firstPointWithSol) {
        fillSolution=true;
        oldToBeUsed=true;
      } else {
        fillSolution=false;
      }
    }  // else reject


  } // if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN)





  // if do not fill solution exit now
  // for the first point with solution we need to copy the new sol into the old one before leaving
  if (!fillSolution) {
    if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN && firstPointWithSol ) {
      // current point is the future previous one
      m_nsolOld=m_nsol;
      for (int isol=0;isol<m_nsol;++isol){
        m_probFinalSolOldVec[isol]=m_probFinalSolVec[isol];
        m_mtautauFinalSolOldVec[isol]=m_mtautauFinalSolVec[isol];
        m_nu1FinalSolOldVec[isol]=m_nu1FinalSolVec[isol];
        m_nu2FinalSolOldVec[isol]=m_nu2FinalSolVec[isol];
      }

    }

    return;
  }




  // compute RMS of the different solutions
  double solSum=0.;
  double solSum2=0.;

  for (int isol=0;isol<m_nsol;++isol){
    ++m_iter5;
    double totalProb;
    double mtautau;
    const TLorentzVector * pnuvec1_tmpj;
    const TLorentzVector * pnuvec2_tmpj;

    if (oldToBeUsed)
      {
        totalProb  =m_probFinalSolOldVec[isol];
        mtautau = m_mtautauFinalSolOldVec[isol];
        pnuvec1_tmpj = & m_nu1FinalSolOldVec[isol];
        pnuvec2_tmpj = & m_nu2FinalSolOldVec[isol];
      } else
      {
        totalProb  =m_probFinalSolVec[isol];
        mtautau = m_mtautauFinalSolVec[isol];
        pnuvec1_tmpj = & m_nu1FinalSolVec[isol];
        pnuvec2_tmpj = & m_nu2FinalSolVec[isol];
      }
    const TLorentzVector & nuvec1_tmpj = *pnuvec1_tmpj;
    const TLorentzVector & nuvec2_tmpj = *pnuvec2_tmpj;

    solSum+=mtautau;
    solSum2+=mtautau*mtautau;


    //if (m_iter5<=0) std::cout << m_iter1 << " " << mtautau << " totalprob " << totalProb << " tauvecsolE " << tauvecsol1[0].E() << " " << tauvecsol2[0].E()<< " tauvecsolM " << tauvecsol1[0].M() << " " << tauvecsol2[0].M() << " " << m_fMfit_all << " " << m_fMfit_allNoWeight << std::endl;

    double weight;
    if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
      // MarkovChain : accepted events already distributed according to probability distribution, so weight is 1.
      // acutally to have a proper estimate of per bin error, instead of putting several time the same point when metropolis alg reject one (or no solution),
      // rather put it with the multiplicity weight. Should only change the error bars
      // might change if weighted markov chain are used
      // there is also an issue with the 4 very close nearly identical solution
      weight=m_markovCountDuplicate+1; // incremented only when a point is rejected, hence need to add 1
    }else {
      // all other cases
      weight=totalProb;
    }

#ifdef BURNIN
    bool fillHistos=true;
    // ignore the first half number of points
    //    if (m_iter0<m_NiterRandomLocal/2) fillHistos=false;
    // ignore first thousand points
    if (m_iter0<1000) fillHistos=false;


    if (fillHistos)
      {
#endif
        m_fMfit_all->Fill(mtautau,weight);
        //    std::cout << "DRDR mtautau " << mtautau << " weight " << weight << std::endl ;
        m_fMfit_allNoWeight->Fill(mtautau,1.);

        //      m_fPXfit1->Fill(nuvec1_tmpj.Px(),weight);
        //      m_fPYfit1->Fill(nuvec1_tmpj.Py(),weight);
        //      m_fPZfit1->Fill(nuvec1_tmpj.Pz(),weight);
        //      m_fPXfit2->Fill(nuvec2_tmpj.Px(),weight);
        //      m_fPYfit2->Fill(nuvec2_tmpj.Py(),weight);
        //      m_fPZfit2->Fill(nuvec2_tmpj.Pz(),weight);

        //----------------- using P*fit to fill Px,y,z_tau
        // Note that the original vistau are used there deliberately,
        // since they will be subtracted after histogram fitting
        // DR, kudos Antony Lesage : do not create temporary TLV within each Fill, saves 10% CPU
        m_fPXfit1->Fill(m_preparedInput.vistau1.Px()+nuvec1_tmpj.Px(),totalProb);
        m_fPYfit1->Fill(m_preparedInput.vistau1.Py()+nuvec1_tmpj.Py(),totalProb);
        m_fPZfit1->Fill(m_preparedInput.vistau1.Pz()+nuvec1_tmpj.Pz(),totalProb);
        m_fPXfit2->Fill(m_preparedInput.vistau2.Px()+nuvec2_tmpj.Px(),totalProb);
        m_fPYfit2->Fill(m_preparedInput.vistau2.Py()+nuvec2_tmpj.Py(),totalProb);
        m_fPZfit2->Fill(m_preparedInput.vistau2.Pz()+nuvec2_tmpj.Pz(),totalProb);

#ifdef BURNIN
      }
#endif

    if(totalProb>m_prob_tmp) // fill solution with highest probability
      {
#ifdef DUMPITERATION
        std::cout << " mtautau " << mtautau << " totalProb " << totalProb << " prob_tmp " << m_prob_tmp
                  << " nu1: " << nuvec1_tmpj.Px() <<" " << nuvec1_tmpj.Py()
                  << " nu2: " << nuvec2_tmpj.Px() <<" " << nuvec2_tmpj.Py()
                  << " oldToBeUsed " << oldToBeUsed
                  << std::endl ;
#endif
        m_prob_tmp=totalProb;
        m_fDitauStuffFit.Mditau_best=mtautau;
        m_fDitauStuffFit.Sign_best=-log10(totalProb);;
        m_fDitauStuffFit.nutau1=nuvec1_tmpj;
        m_fDitauStuffFit.nutau2=nuvec2_tmpj;
        m_fDitauStuffFit.vistau1=m_tauVec1;
        m_fDitauStuffFit.vistau2=m_tauVec2;
        m_fDitauStuffFit.taufit1=m_tauVec1+nuvec1_tmpj;
        m_fDitauStuffFit.taufit2=m_tauVec2+nuvec2_tmpj;

      }
  }  // loop on solutions

  m_markovCountDuplicate=0; // now can reset the duplicate count

  if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN && oldToBeUsed ) {

    // current point is the future previous one
    // TLV copy not super efficient but not dramatic
    m_nsolOld=m_nsol;
    for (int isol=0;isol<m_nsol;++isol){
      m_probFinalSolOldVec[isol]=m_probFinalSolVec[isol];
      m_mtautauFinalSolOldVec[isol]=m_mtautauFinalSolVec[isol];
      m_nu1FinalSolOldVec[isol]=m_nu1FinalSolVec[isol];
      m_nu2FinalSolOldVec[isol]=m_nu2FinalSolVec[isol];
    }


  }

  // compute rms of solutions
  const double solRMS=sqrt(solSum2/m_nsol-std::pow(solSum/m_nsol,2));
  m_OutputInfo.AveSolRMS+=solRMS;

  return;
}



void MissingMassCalculator::SpaceWalkerInit() {
  // FIXME could use function pointer to switch between functions
  m_nsolOld=0;

  double METresX=m_preparedInput.METsigmaL; // MET resolution in direction parallel to MET resolution major axis, for MET scan
  double METresY=m_preparedInput.METsigmaP; // MET resolution in direction perpendicular to to MET resolution major axis, for MET scan



  // precompute some quantities and store in m_ data members
  precomputeCache();
  int Leptau1=0; // lepton-tau code
  int Leptau2=0;
  if(m_preparedInput.type_visTau1==0) Leptau1=1;
  if(m_preparedInput.type_visTau2==0) Leptau2=1;


  if((Leptau1+Leptau2)==2) // both tau's are leptonic
    {
      m_nsigma_METscan=m_nsigma_METscan_ll;
    }
  else if (Leptau1+Leptau2==1) // lep had
    {
      m_nsigma_METscan=m_nsigma_METscan_lh;
    }
  else // hh
    {
      m_nsigma_METscan=m_nsigma_METscan_hh;
    }


  m_nsigma_METscan2=std::pow(m_nsigma_METscan,2);

  const double deltaPhi1=MaxDelPhi(m_preparedInput.type_visTau1,m_tauVec1P);
  const double deltaPhi2=MaxDelPhi(m_preparedInput.type_visTau2,m_tauVec2P);

  m_walkWeight=1.;

  // dummy initial value to avoid printout with random values
  m_Phi10=0.;
  m_Phi20=0.;
  m_MEtL0=0.;
  m_MEtP0=0.;
  m_Mnu10=0.;
  m_Mnu20=0.;

  if (WALKSTRATEGY==WalkStrategy::GRID){
#ifdef TAUESCAN
    std::cout << " GRID scanning with tau energy scan not implemented!" << std::endl;
    throw;
#endif

    m_mTau=1.777;

    int Niter=m_niter_fit1; // number of points for each dR loop
    int NiterMET=m_niter_fit2; // number of iterations for each MET scan loop
    int NiterMnu=m_niter_fit3; // number of iterations for Mnu loop

    m_Phi1Step=deltaPhi1/Niter;
    m_Phi1Min=m_tauVec1Phi-deltaPhi1+m_Phi1Step;
    m_Phi1Max=m_tauVec1Phi+deltaPhi1-m_Phi1Step/2.;

    m_Phi2Step=deltaPhi2/Niter;
    m_Phi2Min=m_tauVec2Phi-deltaPhi2+m_Phi2Step;
    m_Phi2Max=m_tauVec2Phi+deltaPhi2-m_Phi2Step/2.;

    m_Mnu1Min=0.;
    m_Mnu1Max=0.;
    if (Leptau1==1) {
      //      m_Mnu1Max=m_mTau-m_tauVec1M;
      m_Mnu1Max=m_MnuScanRange-m_tauVec1M;
      m_Mnu1Step=m_Mnu1Max/NiterMnu;
      // reduce max to avoid rounding error (there is no bin value close to max)
      m_Mnu1Max-=m_Mnu1Step/100.;
      m_scanMnu1=true;
    }
    else {
      // hadronic, no iteration
      m_Mnu1Step=m_mTau;
      m_Mnu1Max=m_Mnu1Step/2.;
      m_scanMnu1=false;
    }



    m_Mnu2Min=0.;
    m_Mnu2Max=0.;
    if (Leptau2==1) {
      // m_Mnu2Max=m_mTau-m_tauVec2M;
      m_Mnu2Max=m_MnuScanRange-m_tauVec2M;
      m_Mnu2Step=m_Mnu2Max/NiterMnu;
      // reduce max to avoid rounding error (there is no bin value close to max)
      m_Mnu2Max-=m_Mnu2Step/100.;
      m_scanMnu2=true;
    }
    else {
      // hadronic, no iteration
      m_Mnu2Step=m_mTau;
      m_Mnu2Max=m_Mnu2Step/2.;
      m_scanMnu2=false;
    }


    m_MEtLMin=-m_nsigma_METscan*METresX;
    m_MEtLMax=+m_nsigma_METscan*METresX;
    m_MEtLStep=(m_MEtLMax-m_MEtLMin)/NiterMET;
    //margin for rounding error. Here last point should be close to MEtLMax
    m_MEtLMax+=m_MEtLStep/10.;


    m_MEtPMin=-m_nsigma_METscan*METresY;
    m_MEtPMax=+m_nsigma_METscan*METresY;
    m_MEtPStep=(m_MEtPMax-m_MEtPMin)/NiterMET;
    //margin for rounding error. Here last point should be close to MEtLMax
    m_MEtPMax+=m_MEtPStep/10.;


    // initial value
    // kind of a hack, have to initialize to one step less
    m_iter0=-1;
    m_iterNuPV3=0;
    m_testptn1=0;
    m_testptn2=0;
    m_testdiscri1=0;
    m_testdiscri2=0;
    m_iterNsuc=0;

    m_Phi1=m_Phi1Min;
    m_Phi2=m_Phi2Min;
    m_MEtL=m_MEtLMin;
    m_MEtP=m_MEtPMin;
    m_Mnu1=m_Mnu1Min;
    m_Mnu2=m_Mnu2Min;
    //decrement innermost loop
    m_Mnu2-=m_Mnu2Step;




  }


  else if (WALKSTRATEGY==WalkStrategy::RANDOM || WALKSTRATEGY==WalkStrategy::MARKOVCHAIN || WALKSTRATEGY==WalkStrategy::RANDOMNONUNIF ){
    m_mTau=1.777;

    //seeds the random generator in a reproducible way from the phi of both tau;
    double aux=std::abs(m_tauVec1Phi+double(m_tauVec2Phi)/100./TMath::Pi())*100;
    m_seed=(aux-floor(aux))*1E6*(1+m_RndmSeedAltering)+13;

    m_randomGen->SetSeed(m_seed);
    //   std::cout << " DRDR : seed before warmup aux " << aux << " seed " << m_seed << " altering " <<  RndmSeedAltering << std::endl;
    // warm up the random generator
    double xgen=0;
    for (int igen=0;igen<1000;++igen){
      xgen+=m_randomGen->Rndm();
    }
    // std::cout << " DRDR : seed after warmup " << randomGen->GetSeed() << std::endl;

    //int Niter=Niter_fit1; // number of points for each dR loop
    // int NiterMET=Niter_fit2; // number of iterations for each MET scan loop
    //int NiterMnu=Niter_fit3; // number of iterations for Mnu loop

    // approximately compute the number of points from the grid scanning
    // divide by abritry number to recover timing with still better results
    //    m_NiterRandom=(NiterMET+1)*(NiterMET+1)*4*Niter*Niter/10;

    m_Phi1Min=m_tauVec1Phi-deltaPhi1;
    m_Phi1Max=m_tauVec1Phi+deltaPhi1;
    m_Phi1Range=m_Phi1Max-m_Phi1Min;

    m_Phi2Min=m_tauVec2Phi-deltaPhi2;
    m_Phi2Max=m_tauVec2Phi+deltaPhi2;
    m_Phi2Range=m_Phi2Max-m_Phi2Min;

    m_Mnu1Min=0.;
    m_scanMnu1=false;
    m_Mnu1=m_Mnu1Min;

    if (WALKSTRATEGY==WalkStrategy::RANDOM || WALKSTRATEGY==WalkStrategy::RANDOMNONUNIF ){
      // number of random iteration is NiterRandom for lh. Multiply or divide by 10 for ll and hh respectively
      m_niterRandomLocal=m_NiterRandom/10;
    }
    else if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN ){
      // for markov chain use factor 2
      m_niterRandomLocal=m_NiterRandom/2;
    }

    // NiterRandom set by user (default is -1). If negative, defines the default here.
    // no more automatic scaling for ll hl hh
    if (m_NiterRandom<=0) {
      if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN){
        m_niterRandomLocal=100000; // number of iterations for Markov for lh
        if (m_preparedInput.type_visTau1==0 && m_preparedInput.type_visTau2==0) m_niterRandomLocal*=2; // multiplied for ll , unchecked
        if (m_preparedInput.type_visTau1!=0 && m_preparedInput.type_visTau2!=0) m_niterRandomLocal/=2; // divided for hh ,checked
      } else {
        m_niterRandomLocal=2000000; // number of random iterations for lh. Multiplied by 10 for ll, divided by 10 for hh (to be optimised)
        if (m_preparedInput.type_visTau1==0 && m_preparedInput.type_visTau2==0) m_niterRandomLocal*=10; // multiplied for ll , unchecked
        if (m_preparedInput.type_visTau1!=0 && m_preparedInput.type_visTau2!=0) m_niterRandomLocal/=10; // divided for hh , checked
      }
    } else {
      m_niterRandomLocal=m_NiterRandom;
    }

    // hack for hh : allow 10 times more iteration for the fullscan phase
    if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN && m_preparedInput.type_visTau1!=0 && m_preparedInput.type_visTau2!=0){
      m_niterRandomLocal*=10;
    }


    if (Leptau1==1) {
      //      m_Mnu1Max=m_mTau-m_tauVec1M;
      m_Mnu1Max=m_MnuScanRange-m_tauVec1M;
      m_Mnu1Range=m_Mnu1Max-m_Mnu1Min;
      m_scanMnu1=true;
    }

    m_Mnu2Min=0.;
    m_scanMnu2=false;
    m_Mnu2=m_Mnu2Min;
    if (Leptau2==1) {
      //      m_Mnu2Max=m_mTau-m_tauVec2M;
      m_Mnu2Max=m_MnuScanRange-m_tauVec2M;
      m_Mnu2Range=m_Mnu2Max-m_Mnu2Min;
      m_scanMnu2=true;
    }

    if (WALKSTRATEGY==WalkStrategy::RANDOM ||  WALKSTRATEGY==WalkStrategy::MARKOVCHAIN){
      m_MEtLMin=-m_nsigma_METscan*METresX;
      m_MEtLMax=+m_nsigma_METscan*METresX;
      m_MEtLRange=m_MEtLMax-m_MEtLMin;


      m_MEtPMin=-m_nsigma_METscan*METresY;
      m_MEtPMax=+m_nsigma_METscan*METresY;
      m_MEtPRange=m_MEtPMax-m_MEtPMin;
    }else {
      // if gaussian random
      // could do as well for tau E scanning
      m_MEtLRange=METresX;
      m_MEtPRange=METresY;
    }


    if (m_preparedInput.sigmaEtau1>0) {
      // sigmaEtau1 is back to be the absolute energy error
      m_eTau1Min=-m_nsigma_METscan*m_preparedInput.sigmaEtau1+m_preparedInput.vistau1.E();
      m_eTau1Max=+m_nsigma_METscan*m_preparedInput.sigmaEtau1+m_preparedInput.vistau1.E();
      //      m_eTau1Min=(1.0-m_nsigma_METscan*preparedInput.sigmaEtau1)*preparedInput.vistau1.E();
      //      m_eTau1Max=(1.0+m_nsigma_METscan*preparedInput.sigmaEtau1)*preparedInput.vistau1.E();

      // minEtau is not fully optimal and should be studied more. current value is motivated by end-point in truth Pt(vis_tau) distribution
      // minimum pT translated in minimum energy
      //      if(m_eTau1Min<minEtau) m_eTau1Min=minEtau;
      m_eTau1Min=std::max(m_eTau1Min,sqrt(pow(1.777,2)+pow(15.0/m_preparedInput.vistau1.Pt()*m_preparedInput.vistau1.P(),2))); // minimum E(tau) which is allowed in TER scan


      m_eTau1Range=m_eTau1Max-m_eTau1Min;
      m_scanEtau1=true;
    } else {
      m_eTau1Min=-1;
      m_eTau1Max=-1;
      m_scanEtau1=false;
    }


    if (m_preparedInput.sigmaEtau2>0) {
      // sigmaEtau2 is back to be the absolute energy error
      m_eTau2Min=-m_nsigma_METscan*m_preparedInput.sigmaEtau2+m_preparedInput.vistau2.E();
      m_eTau2Max=+m_nsigma_METscan*m_preparedInput.sigmaEtau2+m_preparedInput.vistau2.E();
      //      m_eTau2Min=(1.0-m_nsigma_METscan*preparedInput.sigmaEtau2)*preparedInput.vistau2.E();
      //      m_eTau2Max=(1.0+m_nsigma_METscan*preparedInput.sigmaEtau2)*preparedInput.vistau2.E();

      // minimum pT translated in minimum energy
      //      if(m_eTau2Min<minEtau) m_eTau2Min=minEtau;
      m_eTau2Min=std::max(m_eTau2Min,sqrt(pow(1.777,2)+pow(15.0/m_preparedInput.vistau2.Pt()*m_preparedInput.vistau2.P(),2))); // minimum E(tau) which is allowed in TER scan
      m_eTau2Range=m_eTau2Max-m_eTau2Min;
      m_scanEtau2=true;
    } else {
      m_eTau2Min=-1;
      m_eTau2Max=-1;
      m_scanEtau2=false;
    }



#ifndef USEALLSOLUTION
    m_switch1=true;
    m_switch2=true;
#endif
    m_nsucStop=m_NsucStop;
    m_rmsStop=m_RMSStop;

    m_iter0=-1;
    m_iterNuPV3=0;
    m_testptn1=0;
    m_testptn2=0;
    m_testdiscri1=0;
    m_testdiscri2=0;
    m_iterNsuc=0;
    if (m_meanbinStop>0) {
      m_meanbinToBeEvaluated=true;
    }
    else {
      m_meanbinToBeEvaluated=false;
    }

    if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {

      m_markovCountDuplicate=0;
      m_markovNRejectNoSol=0;
      m_markovNRejectMetropolis=0;
      m_markovNAccept=0;
      m_markovNFullScan=0;
      // set full parameter space scannning for the first steps, until a solution is found
      m_fullParamSpaceScan=true;
      // size of step. Needs to be tune. Start with simple heuristic.
      if(m_proposalTryMEt<0)
        {
          m_MEtProposal=m_MEtPRange/30.;
        }
      else
        {
          m_MEtProposal=m_MEtPRange*m_proposalTryMEt;
        }
      if(m_ProposalTryPhi<0)
        {
          m_PhiProposal=0.04;
        }
      else
        {
          m_PhiProposal=m_ProposalTryPhi;
        }
      // FIXME if m_Mnu1Range !ne m_Mnu2Range same proposal will be done
      if (m_scanMnu1)
        {
          if(m_ProposalTryMnu<0)
            {
              m_MnuProposal=m_Mnu1Range/10.;
            }
          else
            {
              m_MnuProposal=m_Mnu1Range*m_ProposalTryMnu;
            }
        }
      if (m_scanMnu2)
        {
          if(m_ProposalTryMnu<0)
            {
              m_MnuProposal=m_Mnu2Range/10.;
            }
          else
            {
              m_MnuProposal=m_Mnu2Range*m_ProposalTryMnu;
            }
        }

      if (m_scanEtau1)
        {
          if(m_ProposalTryEtau<0)
            {
              m_eTau1Proposal=m_eTau1Range*0.05;
            }
          else
            {
              m_eTau1Proposal=m_eTau1Range*m_ProposalTryEtau;
            }
        }
      if (m_scanEtau2)
        {
          if(m_ProposalTryEtau<0)
            {
              m_eTau2Proposal=m_eTau2Range*0.05;
            }
          else
            {
              m_eTau2Proposal=m_eTau2Range*m_ProposalTryEtau;
            }
        }

    }

  }

  else {
    std::cout << "SpaceWalker::SpaceWalker ERROR unknown strategy " << WALKSTRATEGY << std::endl;
  }


}

// iterator. walk has internal counters, should only be used in a while loop
// so far only implement grid strategy
// act on MMC data member to be fast
bool MissingMassCalculator::SpaceWalkerWalk(){

  m_MEtX=-999.;
  m_MEtY=-999.;



  // //   const int iterMax= 200000000;

  // //   // avoid infinite loop just to be sure
  // //   if (m_iter0==iterMax){
  // //     std::cout << "SpaceWalkerWalk ERROR max number of iterations " << iterMax << " reached. Stop. " << std::endl;
  // //     return false;
  // //   }

  if (WALKSTRATEGY==WalkStrategy::GRID) {
    ++m_iter0;
    m_Mnu2+=m_Mnu2Step;
    if (m_Mnu2<m_Mnu2Max) return true;
    m_Mnu2=m_Mnu2Min;



    m_Mnu1+=m_Mnu1Step;
    if (m_Mnu1<m_Mnu1Max) return true;
    m_Mnu1=m_Mnu1Min;



    m_MEtP+=m_MEtPStep;
    if (m_MEtP<m_MEtPMax) return true;
    m_MEtP=m_MEtPMin;

    m_MEtL+=m_MEtLStep;
    if (m_MEtL<m_MEtLMax) return true;
    m_MEtL=m_MEtLMin;


    m_Phi2+=m_Phi2Step;
    if (m_Phi2<m_Phi2Max) return true;
    m_Phi2=m_Phi2Min;

    m_Phi1+=m_Phi1Step;
    if (m_Phi1<m_Phi1Max) return true;
    m_Phi1=m_Phi1Min;


  }

  else if (WALKSTRATEGY==WalkStrategy::RANDOM || WALKSTRATEGY==WalkStrategy::RANDOMNONUNIF) {
    ++m_iter0;

    // Critere d'arret
    if (m_meanbinToBeEvaluated && m_iterNsuc==500 )
      {
        // for markov chain m_iterNsuc is the number of *accepted* points, so there can be several iterations without
        // any increment of m_iterNsuc. Hence need to make sure meanbin is evaluated only once
        m_meanbinToBeEvaluated=false;


        // Meanbin stopping criterion
        std::vector<double> histInfo(HistInfo::MAXHISTINFO);
        // SLIDINGWINDOW strategy to avoid doing the parabola fit now given it will not be used
        maxFromHist(m_fMfit_all,histInfo,MaxHistStrategy::SLIDINGWINDOW);
        double meanbin=histInfo.at(HistInfo::MEANBIN);
        //      std::cout << "Meanbin after 500 successes = " << meanbin << std::endl;
        if(meanbin<0)
          {
            m_nsucStop=-1; // no meaningful meanbin switch back to niter criterion
          }
        else
          {
            double stopdouble = 500*std::pow((meanbin/m_meanbinStop),2);
            int stopint = stopdouble;
            m_nsucStop = stopint;
          }
        // RMS stopping criterion
        //    double rms=m_fMfit_all->GetRMS(1);
        //    m_NsucStop=rms*m_rmsStop;
        if (m_nsucStop<500) return false;
      }

    // should be outside m_meanbinStop test
    if (m_iterNsuc==m_nsucStop) return false; // Critere d'arret pour nombre de succes
    if (m_iter0==m_niterRandomLocal) return false; // Critere d'arret pour nombre iteration


    if (WALKSTRATEGY==WalkStrategy::RANDOM){
      // cut the corners in MissingET (not optimised at all)
      // not needed if distribution is already gaussian
      do {
        m_MEtP=m_MEtPMin+m_MEtPRange*m_randomGen->Rndm();
        m_MEtL=m_MEtLMin+m_MEtLRange*m_randomGen->Rndm();
      } while ( !checkMEtInRange());
    }
    else { // if WalkStrategy::RANDOMNONUNIF
      if (!m_preparedInput.UseHT){ // "!" was missing. Significant bug, only for RANDOMNONUNIF
        // if gauss distribution metprange is the sigma
        m_MEtP=m_randomGen->Gaus(0.,m_MEtPRange);
        m_MEtL=m_randomGen->Gaus(0.,m_MEtLRange);
      }
      else
        {
          if (m_randomGen->Rndm()>m_preparedInput.MHtGaussFr/(1+m_preparedInput.MHtGaussFr)){
            // first gaussian
            m_MEtP=m_randomGen->Gaus(0.,m_preparedInput.m_MHtSigma1);                   }
          else{
            // second gaussian
            m_MEtP=m_randomGen->Gaus(0.,m_preparedInput.m_MHtSigma2);
          }
          if (m_randomGen->Rndm()>m_preparedInput.MHtGaussFr/(1+m_preparedInput.MHtGaussFr)){
            // first gaussian
            m_MEtL=m_randomGen->Gaus(0.,m_preparedInput.m_MHtSigma1);                   }
          else{
            // second gaussian
            m_MEtL=m_randomGen->Gaus(0.,m_preparedInput.m_MHtSigma2);
          }
        }


    }




    if (m_scanMnu1){
      m_Mnu1=m_Mnu1Min+m_Mnu1Range*m_randomGen->Rndm();
    }

    if (m_scanMnu2){
      m_Mnu2=m_Mnu2Min+m_Mnu2Range*m_randomGen->Rndm();
    }



    m_Phi1=m_Phi1Min+m_Phi1Range*m_randomGen->Rndm();
    m_Phi2=m_Phi2Min+m_Phi2Range*m_randomGen->Rndm();


#ifdef TAUESCAN
    if (m_scanEtau1){
      m_eTau1=m_eTau1Min+m_eTau1Range*m_randomGen->Rndm();
    }

    if (m_scanEtau2){
      m_eTau2=m_eTau2Min+m_eTau2Range*m_randomGen->Rndm();
    }
#endif

    updateTauKine(); // update tau kinematics if tau energy scan

    return true;

  }
  else if (WALKSTRATEGY==WalkStrategy::MARKOVCHAIN) {
    ++m_iter0;

    if (m_meanbinToBeEvaluated && m_iterNsuc==500 )
      {
        std::cout << " in m_meanbinToBeEvaluated && m_iterNsuc==500 " << std::endl;
        // for markov chain m_iterNsuc is the number of *accepted* points, so there can be several iterations without
        // any increment of m_iterNsuc. Hence need to make sure meanbin is evaluated only once
        m_meanbinToBeEvaluated=false;

        // Meanbin stopping criterion
        std::vector<double> histInfo(HistInfo::MAXHISTINFO);
        // SLIDINGWINDOW strategy to avoid doing the parabola fit now given it will not be use
        maxFromHist(m_fMfit_all,histInfo,MaxHistStrategy::SLIDINGWINDOW);
        double meanbin=histInfo.at(HistInfo::MEANBIN);
        //      std::cout << "Meanbin after 500 successes = " << meanbin << std::endl;
        if(meanbin<0)
          {
            m_nsucStop=-1; // no meaningful meanbin switch back to niter criterion
          }
        else
          {
            double stopdouble = 500*std::pow((meanbin/m_meanbinStop),2);
            int stopint = stopdouble;
            m_nsucStop = stopint;
          }
        if (m_nsucStop<500) return false;

      }
    // should be outside m_meanbinStop test
    if (m_iterNsuc==m_nsucStop) return false; // Critere d'arret pour nombre de succes

    if (m_iter0==m_niterRandomLocal) return false; // for now simple stopping criterion on number of iteration

    if (m_fullParamSpaceScan)  {
      // as long as no solution found need to randomise on the full parameter space

      // cut the corners in MissingET (not optimised at all)
      // not needed if distribution is already gaussian
      do {
        m_MEtP=m_MEtPMin+m_MEtPRange*m_randomGen->Rndm();
        m_MEtL=m_MEtLMin+m_MEtLRange*m_randomGen->Rndm();
      } while ( !checkMEtInRange());

      if (m_scanMnu1){
        m_Mnu1=m_Mnu1Min+m_Mnu1Range*m_randomGen->Rndm();
      }

      if (m_scanMnu2){
        m_Mnu2=m_Mnu2Min+m_Mnu2Range*m_randomGen->Rndm();
      }



      m_Phi1=m_Phi1Min+m_Phi1Range*m_randomGen->Rndm();
      m_Phi2=m_Phi2Min+m_Phi2Range*m_randomGen->Rndm();

#ifdef TAUESCAN
      if (m_scanEtau1){
        m_eTau1=m_eTau1Min+m_eTau1Range*m_randomGen->Rndm();
      }

      if (m_scanEtau2){
        m_eTau2=m_eTau2Min+m_eTau2Range*m_randomGen->Rndm();
      }
#endif


      updateTauKine(); // update tau kinematics if tau energy scan

      return true;
    }

    // here the real markov chain takes place : "propose" the new point
    // note that if one parameter goes outside range, this should not be fixed here
    // but later in handleSolution, otherwise would cause a bias

    //   m_MEtP0 etc... also store the position of the previous Markov Chain step, which is needed by the algorithm
    m_MEtP0=m_MEtP;
    m_MEtL0=m_MEtL;

    m_MEtP=m_randomGen->Gaus(m_MEtP0,m_MEtProposal);

    m_MEtL=m_randomGen->Gaus(m_MEtL0,m_MEtProposal);



    if (m_scanMnu1){
      m_Mnu10=m_Mnu1;
      m_Mnu1=m_randomGen->Gaus(m_Mnu10,m_MnuProposal);
    }

    if (m_scanMnu2){
      m_Mnu20=m_Mnu2;
      m_Mnu2=m_randomGen->Gaus(m_Mnu20,m_MnuProposal);
    }






    m_Phi10=m_Phi1;
    m_Phi1=m_randomGen->Gaus(m_Phi10,m_PhiProposal);

    m_Phi20=m_Phi2;

    m_Phi2=m_randomGen->Gaus(m_Phi20,m_PhiProposal);

#ifdef TAUESCAN
    if (m_scanEtau1){
      m_eTau10=m_eTau1;
      m_eTau1=m_randomGen->Gaus(m_eTau10,m_eTau1Proposal);
    }

    if (m_scanEtau2){
      m_eTau20=m_eTau2;
      m_eTau2=m_randomGen->Gaus(m_eTau20,m_eTau2Proposal);
    }
#endif

    updateTauKine(); // update tau kinematics if tau energy scan

    return true;


  }


  return false;


}

//update tau kinematics if tau energy scan
inline void MissingMassCalculator::updateTauKine() {
  // FIXME need protection if to small tau energy

  if (m_scanEtau1){
    // scale momentum keeping the same mass and direction
    const double newPscale =sqrt(std::pow(m_eTau1,2)-m_preparedInput.vistau1.M2())/m_preparedInput.vistau1.P();
    m_tauVec1Px=m_preparedInput.vistau1.Px()*newPscale;
    m_tauVec1Py=m_preparedInput.vistau1.Py()*newPscale;
    m_tauVec1Pz=m_preparedInput.vistau1.Pz()*newPscale;
    m_tauVec1E=m_eTau1;
    m_tauVec1.SetPxPyPzE(m_tauVec1Px,m_tauVec1Py,m_tauVec1Pz,m_tauVec1E);
    m_tauVec1P=m_tauVec1.P();
    // tauvec phi and M do not change

  }



  if (m_scanEtau2){
    // scale momentum keeping the same mass and direction
    //    double newPscale =sqrt(std::pow(m_eTau2,2)-std::pow(m_tauVec2M,2))/m_tauVec2P;
    //
    const double newPscale =sqrt(std::pow(m_eTau2,2)-m_preparedInput.vistau2.M2())/m_preparedInput.vistau2.P();
    m_tauVec2Px=m_preparedInput.vistau2.Px()*newPscale;
    m_tauVec2Py=m_preparedInput.vistau2.Py()*newPscale;
    m_tauVec2Pz=m_preparedInput.vistau2.Pz()*newPscale;
    m_tauVec2E=m_eTau2;
    m_tauVec2.SetPxPyPzE(m_tauVec2Px,m_tauVec2Py,m_tauVec2Pz,m_tauVec2E);
    m_tauVec2P=m_tauVec2.P();
    // tauvec phi and M do not change

  }


}

// compute cached values (this value do not change within one call of MMC, except for tau e scanning)
// return true if cache was already uptodatexs
inline bool MissingMassCalculator::precomputeCache() {


  // copy tau 4 vect. If tau E scanning, these vectors will be modified
  m_tauVec1= m_preparedInput.vistau1;
  m_tauVec2= m_preparedInput.vistau2;

  const TVector2 & metVec = m_preparedInput.MetVec;

  bool same=true;
  same=updateDouble(m_tauVec1.Phi(),m_tauVec1Phi) && same;
  same=updateDouble(m_tauVec2.Phi(),m_tauVec2Phi) && same ;
  same=updateDouble(m_tauVec1.M(),m_tauVec1M) && same;
  same=updateDouble(m_tauVec2.M(),m_tauVec2M) && same;
  same=updateDouble(m_tauVec1.E(),m_tauVec1E) && same;
  same=updateDouble(m_tauVec2.E(),m_tauVec2E) && same;
  same=updateDouble(m_tauVec1.Px(),m_tauVec1Px) && same;
  same=updateDouble(m_tauVec1.Py(),m_tauVec1Py) && same;
  same=updateDouble(m_tauVec1.Pz(),m_tauVec1Pz) && same;
  same=updateDouble(m_tauVec2.Px(),m_tauVec2Px) && same;
  same=updateDouble(m_tauVec2.Py(),m_tauVec2Py) && same;
  same=updateDouble(m_tauVec2.Pz(),m_tauVec2Pz) && same;
  same=updateDouble(m_tauVec1.P(),m_tauVec1P) && same;
  same=updateDouble(m_tauVec2.P(),m_tauVec2P) && same;




  same=updateDouble(1.777,m_mTau) && same;
  same=updateDouble(std::pow(m_mTau,2),m_mTau2) && same;
  same=updateDouble(cos(m_preparedInput.METcovphi),m_metCovPhiCos) && same;
  same=updateDouble(sin(m_preparedInput.METcovphi),m_metCovPhiSin) && same;
  same=updateDouble((m_tauVec1+m_tauVec2).M(),m_Mvis) && same ;

  TLorentzVector detMet4vec;
  detMet4vec.SetPxPyPzE(m_preparedInput.detMetVec.X(),m_preparedInput.detMetVec.Y(),0.0,m_preparedInput.detMetVec.Mod());
  same=updateDouble((m_tauVec1+m_tauVec2+detMet4vec).M(),m_Meff) && same ;
  same=updateDouble(m_preparedInput.detMetVec.Mod(),m_DetMEt) && same;



  same = updateDouble (m_preparedInput.HtOffset,m_HtOffset) && same ;
  // note that if useHT met_vec is actually -HT
  same = updateDouble(metVec.X(),m_inputMEtX) && same;
  same = updateDouble(metVec.Y(),m_inputMEtY) && same;
  same = updateDouble(metVec.Mod(),m_inputMEtT) && same;



  return same;
}

// return true if cache was uptodate
inline bool MissingMassCalculator::updateDouble  (const double in, double & out)
{
  if (out==in) return true;
  out=in;
  return false;
}




// return true if all parameters are within their domain
inline bool MissingMassCalculator::checkAllParamInRange  ()
{

  if (m_scanMnu1) {
    if (m_Mnu1<m_Mnu1Min) return false;
    if (m_Mnu1>m_Mnu1Max) return false;
  }

  if (m_scanMnu2) {
    if (m_Mnu2<m_Mnu2Min) return false;
    if (m_Mnu2>m_Mnu2Max) return false;
  }

  // FIXME note that since there is a coupling between Met and tau, should rigorously test both together
  // however since the 3 sigma range is just a hack, it is probably OK

#ifdef TAUESCAN
  if (m_scanEtau1) {
    if (m_eTau1<m_eTau1Min) return false;
    if (m_eTau1>m_eTau1Max) return false;
  }

  if (m_scanEtau2) {
    if (m_eTau2<m_eTau2Min) return false;
    if (m_eTau2>m_eTau2Max) return false;
  }
#endif

  if (m_Phi1<m_Phi1Min) return false;
  if (m_Phi1>m_Phi1Max) return false;

  if (m_Phi2<m_Phi2Min) return false;
  if (m_Phi2>m_Phi2Max) return false;


  if (!checkMEtInRange()) return false;

  return true;

}



// return true if Met is within disk instead of withing square (cut the corners)
inline bool MissingMassCalculator::checkMEtInRange  ()
{
  // check MEt is in allowed range
  // range is 3sigma disk ("cutting the corners")
  //    std::cout << "m_metL:" << m_MEtL << "m_metp:" << m_MEtP << std::endl;
#ifndef METNOTCUTCORNER
  if (std::pow(m_MEtL/m_preparedInput.METsigmaL,2)+std::pow(m_MEtP/m_preparedInput.METsigmaP,2) > m_nsigma_METscan2) {
    return false;
  }
  else {
    return true;
  }
#else
  // simple square cut.
  // small margin added otherwise some good points might fail in grid case
  if (std::abs(m_MEtL)-1E-10> m_nsigma_METscan*m_preparedInput.METsigmaL) {
    return false;
  }
  if (std::abs(m_MEtP)-1E-10> m_nsigma_METscan*m_preparedInput.METsigmaP) {
    return false;
  }
  return true;
#endif

}




//put back phi within -pi, +pi
inline double  MissingMassCalculator::fixPhiRange  (const double & phi)
{
  double phiOut=phi;

  if (phiOut>0){
    while (phiOut>pi) {
      phiOut-=twopi;
    }
  }

  else{
    while (phiOut<-pi) {
      phiOut+=twopi;
    }
  }
  return phiOut;
}





// fast approximate calculation of sin and cos
// approximation good to 1 per mill. s²+c²=1 strictly exact though
// it is like using slightly different values of phi1 and phi2



const double piOverTwo=pi/2.;
const double fastB=4/pi;
const double fastC=-4/(pi*pi);
const double fastP=9./40.;
const double fastQ=31./40.;


inline void MissingMassCalculator::fastSinCos (const double & phiInput, double & sinPhi, double & cosPhi)
{
  // use normal sin cos if switch off
#ifdef NOFASTSINCOS
  sinPhi=sin(phiInput);
  cosPhi=cos(phiInput);
#else


  double phi = fixPhiRange(phiInput);

  // http://devmaster.net/forums/topic/4648-fast-and-accurate-sinecosine/
  // accurate to 1 per mille

  // even faster
  //  const double y=fastB*phi+fastC*phi*std::abs(phi);
  // sinPhi=fastP*(y*std::abs(y)-y)+y;
  const double y=phi*(fastB+fastC*std::abs(phi));
  sinPhi=y*(fastP*std::abs(y)+fastQ);


  //note that one could use cos(phi)=sin(phi+pi/2), however then one would not have c²+s²=1 (would get it only within 1 per mille)
  // the choice here is to keep c^2+s^2=1 so everything is as one would compute c and s from a slightly (1 per mille) different angle
  cosPhi=sqrt(1-std::pow(sinPhi,2));
  if (std::abs(phi)>piOverTwo) cosPhi=-cosPhi;


#endif

}

inline double MissingMassCalculator::MaxDelPhi(int tau_type, double Pvis)
{
  // note that PhiProposal has to be fixed as well
#ifndef USEMAXDELPHI
  return m_dRmax_tau+0*Pvis*tau_type; // hack to avoid warning
#else
  double maxDphi=0.2;
  if(tau_type==0) maxDphi=exp(-3.1-0.0059*Pvis)+0.08;
  if(tau_type==1) maxDphi=0.21+0.00031*Pvis;
  if(tau_type==3) maxDphi=0.2+0.00011*Pvis;
  return maxDphi;
#endif
}

// ----- returns dTheta3D lower and upper boundaries:
// limit_code=0: 99% lower limit
// limit_code=1; 99% upper limit
// limit_code=2; 95% upper limit
double MissingMassCalculator::dTheta3DLimit(const int & tau_type,const  int & limit_code,const  double & P_tau) {

#ifndef WITHDTHETA3DLIM
  // make the test ineffective if desired
  if (limit_code==0) return 0.;
  if (limit_code==1) return 10.;
  if (limit_code==2) return 10.;
#endif

  double limit=1.0;
  if(limit_code==0) limit=0.0;
  double par[3]={0.0,0.0,0.0};
  // ---- leptonic tau's
  if(tau_type==0)
    {
      if(limit_code==0) // lower 99% limit
        {
          par[0]=0.3342;
          par[1]=-0.3376;
          par[2]=-0.001377;
        }
      if(limit_code==1) // upper 99% limit
        {
          par[0]=3.243;
          par[1]=-12.87;
          par[2]=0.009656;
        }
      if(limit_code==2) // upper 95% limit
        {
          par[0]=2.927;
          par[1]=-7.911;
          par[2]=0.007783;
        }
    }
  // ---- 1-prong tau's
  if(tau_type==1)
    {
      if(limit_code==0) // lower 99% limit
        {
          par[0]=0.2673;
          par[1]=-14.8;
          par[2]=-0.0004859;
        }
      if(limit_code==1) // upper 99% limit
        {
          par[0]=9.341;
          par[1]=-15.88;
          par[2]=0.0333;
        }
      if(limit_code==2) // upper 95% limit
        {
          par[0]=6.535;
          par[1]=-8.649;
          par[2]=0.00277;
        }
    }
  // ---- 3-prong tau's
  if(tau_type==3)
    {
      if(limit_code==0) // lower 99% limit
        {
          par[0]=0.2308;
          par[1]=-15.24;
          par[2]=-0.0009458;
        }
      if(limit_code==1) // upper 99% limit
        {
          par[0]=14.58;
          par[1]=-6.043;
          par[2]=-0.00928;
        }
      if(limit_code==2) // upper 95% limit
        {
          par[0]=8.233;
          par[1]=-0.3018;
          par[2]=-0.009399;
        }
    }

  if(std::abs(P_tau+par[1])>0.0) limit=par[0]/(P_tau+par[1])+par[2];
  if(limit_code==0){
    if (limit<0.0){
      limit=0.0;
    }
    else if ( limit>0.03 ){
      limit=0.03;
    }
  } else {
    if (limit<0.0 || limit>0.5*TMath::Pi()) {
      limit=0.5*TMath::Pi();
    }
    else if (limit<0.05 && limit>0.0) {
      limit=0.05; // parameterization only runs up to P~220 GeV in this regime will set an upper bound of 0.05
    }
  }


  return limit;
}

