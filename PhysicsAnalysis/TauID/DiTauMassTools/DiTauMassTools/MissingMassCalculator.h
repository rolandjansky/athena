/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Caution: This version of the MMC is planned to phased out around 2020/06/30, see README

/*
------ MissingMassCalculator
------ Author: Aliaksandr Pranko (appranko@lbl.gov)
------ Code developers: David Rousseau (rousseau@lal.in2p3.fr), Dimitris Varouchas (Dimitris.Varouchas@cern.ch)
MissingMassCalculator is designed to reconstruct mass in
events where two particles decay into states with missing ET.
*/
#ifndef MissingMassCalculator_h
#define MissingMassCalculator_h



#if !defined (__CINT__) || defined (__MAKECINT__)

#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <vector>
#include <TObject.h>
#include <TDirectory.h>

#include <memory>

#include <TRandom2.h>

class TH1F;


namespace MMCCalibrationSet 
{
  enum e { MMC2011=0, MMC2012, MMC2015, MMC2015HIGHMASS, UPGRADE, LFVMMC2012, MMC2016MC15C, MAXMMCCALIBRATIONSET };
  const std::string name[MAXMMCCALIBRATIONSET]={ "MMC2011", "MMC2012", "MMC2015", "MMC2015HIGHMASS", "UPGRADE", "LFVMMC2012", "MMC2016MC15C"};
}

namespace MMCFitMethod 
{
  enum e { MAXW=0, MLM, MLNU3P,MAX};
  const std::string name[MAX]={ "MAXW=MaximumWeight", "MLM=MostLikelyMass", "MLNU3P=MostLikelyNeUtrino3Momentum"};
  const std::string shortName[MAX]={ "MAXW", "MLM", "MLNU3P"};
}

namespace WalkStrategy 
{
  enum e { GRID=0, RANDOM,RANDOMNONUNIF, MARKOVCHAIN, MAXWALKSTRATEGY };
}

namespace MaxHistStrategy 
{
  enum e { MAXBIN=0,MAXBINWINDOW, SLIDINGWINDOW, FIT,MAXMAXHISTSTRATEGY };
}


namespace HistInfo 
{
  enum e { PROB=0, INTEGRAL, CHI2, DISCRI, TANTHETA, TANTHETAW, FITLENGTH, RMS, RMSVSDISCRI, MEANBIN, MAXHISTINFO };
}



#endif

/* class MissingMassCalculator: public TObject { */
class MissingMassCalculator {

 public:
  //DR
  /* #if !defined(__CINT__) | defined(__MAKECINT__) */
  /* // Define the class for the cint dictionary */
  /* ClassDef(MissingMassCalculator,1); */
  /* #endif */
    

  // keep the whole list of points
  struct AllPointsOutputInfo {
    std::vector<double> prob;
    std::vector<double> mtautau;
    std::vector<double> Phi1;
    std::vector<double> Phi2;
    std::vector<double> MEtP;
    std::vector<double> MEtL;
    std::vector<double> MEtX;
    std::vector<double> MEtY;
    std::vector<double> Mnu1;
    std::vector<double> Mnu2;
    std::vector<double> eTau1;
    std::vector<double> eTau2;
    std::vector<int> pCode; // -10 : outside parameter space boundary. 0 no solution. 10 some solutions. +1 if accepted
    std::vector<int> index;
  };

  
	  

private:
  AllPointsOutputInfo m_aPOI;
  int m_apoiIndex;

  //---------------- structures
  struct DitauStuff {
    double Mditau_best; // best fitted M(ditau)
    double Sign_best;   // best significance of M(ditau) fit
    TLorentzVector nutau1;  // fitted 4-vec for neutrino from tau-1
    TLorentzVector nutau2;  // fitted 4-vec for neutrino from tau-2
    TLorentzVector vistau1;  // fitted 4-vec for visible tau-1
    TLorentzVector vistau2;  // fitted 4-vec for visible tau-2
    TLorentzVector taufit1;  // fitted 4-vec for full tau-1
    TLorentzVector taufit2;  // fitted 4-vec for full tau-2
    double RMSoverMPV;
  };

  struct InputInfoStuff {
    TVector2 MetVec;
    TVector2 detMetVec; // need to keep separate instead Metvec recalibrated or replaced by -HT
    TLorentzVector vistau1;
    TLorentzVector vistau2;
    int type_visTau1; // 0: l, 1:1-prong, 3:3-prong
    int type_visTau2;  // 0: l, 1:1-prong, 3:3-prong
    int Nprong_tau1;
    int Nprong_tau2;
    int dataType;
    double METcovphi;
    double METsigmaP;
    double METsigmaL;
    double m_SumEt;
    double sigmaEtau1;
    double sigmaEtau2;
    std::vector<TLorentzVector> m_jet4vecs;
    int m_Njet25;
    double m_DelPhiTT;
    bool allowUseHT;
    bool UseHT;
    double m_MHtSigma1; // sigma of 1st Gaussian in missing Ht resolution
    double m_MHtSigma2; // sigma of 2nd Gaussian in missing Ht resolution
    double MHtGaussFr; // relative fraction of 2nd Gaussian
    double HtOffset; // HT offset

  };

  struct OutputInfoStuff {
    int FitStatus;
    double FitSignificance[MMCFitMethod::MAX];
    double FittedMass[MMCFitMethod::MAX];
    TLorentzVector nuvec1[MMCFitMethod::MAX];
    TLorentzVector objvec1[MMCFitMethod::MAX];
    TLorentzVector nuvec2[MMCFitMethod::MAX];
    TLorentzVector objvec2[MMCFitMethod::MAX];
    TLorentzVector totalvec[MMCFitMethod::MAX];
    TVector2 FittedMetVec[MMCFitMethod::MAX];
    double RMS2MPV;
    std::shared_ptr<TH1F> hMfit_all;
    std::shared_ptr<TH1F> hMfit_allNoWeight;
    int NTrials;
    int NSuccesses;
    int NSolutions;
    double SumW;
    double AveSolRMS;
    bool UseHT;
  };

  MMCCalibrationSet::e m_mmcCalibrationSet;
  //  WalkStrategy::e walkStrategy;
  TRandom2 * m_randomGen;
  
  //SpeedUp static array for efficient access
  static double s_fit_param[2][3][6][5];
  static double s_ter_sigma_par[2][10][3];
  //cache quantities for efficient NuPSolution calculation


  int m_nsolmax,m_nsolfinalmax;
  int m_niterRandomLocal;
  int m_nsucStop;
  int m_rmsStop;
  double m_meanbinStop;
  


  
  // these are temporary vectors. Dclared globally to avoid construction/destruction
  std::vector<TLorentzVector> m_nuvecsol1;
  std::vector<TLorentzVector> m_nuvecsol2;

  std::vector<TLorentzVector> m_tauvecsol1;
  std::vector<TLorentzVector> m_tauvecsol2;
  std::vector<double> m_tauvecprob1;
  std::vector<double> m_tauvecprob2;

  std::vector<TLorentzVector> m_nuvec1_tmp;
  std::vector<TLorentzVector> m_nuvec2_tmp;

  TVector2 m_metVec;
  TLorentzVector m_tautau_tmp;
  TLorentzVector m_tlv_tmp;

  
  //  std::vector<TLorentzVector> tauvecsol1;
  // std::vector<TLorentzVector> tauvecsol2;
  bool m_debugThisIteration;
  
  int m_nCallprobCalculatorV9fast;
  

  int m_iter1,m_iter2,m_iter3,m_iter4,m_iter5,m_iang1low,m_iang1high;
  // int m_iang2low,m_iang2high;
  int m_iterTheta3d;
  
  double m_prob_tmp;
  
  double m_totalProbSum;
  double m_mtautauSum;

  int m_seed;
  // data member for the spaceWalker approach

  int m_iter0;
  int m_iterNuPV3;
  int m_testptn1;
  int m_testptn2;
  int m_testdiscri1;
  int m_testdiscri2;
  int m_iterNsuc;
  bool m_switch1;
  bool m_switch2;

  bool m_meanbinToBeEvaluated;
  
  int m_markovCountDuplicate;
  int m_markovNFullScan;
  int m_markovNRejectNoSol;
  int m_markovNRejectMetropolis;
  int m_markovNAccept;

  double m_printmMaxError;
  double m_printmMeanError;
  double m_printmInvWidth2Error;

  double m_proposalTryMEt;
  double m_ProposalTryPhi;
  double m_ProposalTryMnu;
  double m_ProposalTryEtau;


  double m_mTau,m_mTau2;
  double m_MEtL,m_MEtP,m_Phi1,m_Phi2,m_Mnu1,m_Mnu2;
  double m_eTau1, m_eTau2;
  double m_eTau10, m_eTau20;
  double m_MEtL0,m_MEtP0,m_Phi10,m_Phi20,m_Mnu10,m_Mnu20;
  double m_MEtLMin,m_MEtPMin,m_Phi1Min,m_Phi2Min,m_Mnu1Min,m_Mnu2Min;
  double m_MEtLMax,m_MEtPMax,m_Phi1Max,m_Phi2Max,m_Mnu1Max,m_Mnu2Max;
  double m_MEtLStep,m_MEtPStep,m_Phi1Step,m_Phi2Step,m_Mnu1Step,m_Mnu2Step;
  double m_MEtLRange,m_MEtPRange,m_Phi1Range,m_Phi2Range,m_Mnu1Range,m_Mnu2Range;
  double m_MEtProposal,m_PhiProposal,m_MnuProposal;
  double m_eTau1Proposal,m_eTau2Proposal;

  double m_MEtX, m_MEtY,m_MEtT;
  double m_eTau1Min,m_eTau1Max,m_eTau1Range;
  double m_eTau2Min,m_eTau2Max,m_eTau2Range;
  bool m_fullParamSpaceScan;
  int m_nsolOld;
  std::vector<double>   m_probFinalSolOldVec;
  std::vector<double>   m_mtautauFinalSolOldVec;
  std::vector<TLorentzVector>  m_nu1FinalSolOldVec;
  std::vector<TLorentzVector>  m_nu2FinalSolOldVec;

  int m_nsol;
  std::vector<double>   m_probFinalSolVec;
  std::vector<double>   m_mtautauFinalSolVec;
  std::vector<TLorentzVector>  m_nu1FinalSolVec;
  std::vector<TLorentzVector>  m_nu2FinalSolVec;


  double m_walkWeight;
  double m_cosPhi1, m_cosPhi2, m_sinPhi1, m_sinPhi2;
  
  bool m_scanMnu1,m_scanMnu2;
  bool m_scanEtau1,m_scanEtau2;

  TLorentzVector m_tauVec1,m_tauVec2;
  double m_tauVec1Phi, m_tauVec2Phi;
  double m_tauVec1M, m_tauVec2M;
  double m_tauVec1Px, m_tauVec1Py, m_tauVec1Pz;
  double m_tauVec2Px, m_tauVec2Py, m_tauVec2Pz;
  double m_tauVec1P, m_tauVec2P;
  double m_tauVec1E;
  double m_tauVec2E;
  double m_m2Nu1;
  double m_m2Nu2;
  double m_ET2v1;
  double m_ET2v2;
  double m_E2v1;
  double m_E2v2;
  double m_Ev2;
  double m_Ev1;
  double m_metCovPhiCos,m_metCovPhiSin;
  double m_Mvis,m_Meff;
  double m_DetMEt;
  double m_inputMEtX,m_inputMEtY,m_inputMEtT;
  double m_HtOffset;
  bool m_reRunWithBestMET;
  
  //--- define histograms for histogram method
  //--- upper limits need to be revisied in the future!!! It may be not enough for some analyses
  std::shared_ptr<TH1F> m_fMfit_all;
  std::shared_ptr<TH1F> m_fMfit_allNoWeight;
  
  std::shared_ptr<TH1F> m_fPXfit1;
  std::shared_ptr<TH1F> m_fPYfit1;
  std::shared_ptr<TH1F> m_fPZfit1;
  std::shared_ptr<TH1F> m_fPXfit2;
  std::shared_ptr<TH1F> m_fPYfit2;
  std::shared_ptr<TH1F> m_fPZfit2;

  TF1 *m_fFitting;

  // for intermediate calc
  TLorentzVector m_TLVdummy;

  //---------------- protected variables
  DitauStuff m_fDitauStuffFit; // results based on fit method
  DitauStuff m_fDitauStuffHisto; // results based on histo method
  InputInfoStuff m_rawInput; // raw input 
  InputInfoStuff m_preparedInput; // corrected input (DR should maybe not be a datamember)
  OutputInfoStuff m_OutputInfo; // output info

  int m_fUseVerbose; // code to turn ON printouts for debugging
  bool m_fSpeedStudy; // code to turn ON speed study
  int m_AlgorithmVersion; // version of the algorithm
  int m_SearchMode; // search Mode: 0=di-tau, 1=WW, 2=W->tau+nu
  int m_fApplyMassScale; // switch to apply mass scale correction
  int m_fUseTailCleanup; // switch to apply tail clean-up
  int m_fUseTauProbability; // switch to apply TauProbability
  int m_fUseMnuProbability; // switch to apply MnuProbability
  int m_fUseDefaults; // switch to control defaults: 1== use defaults, 0== don't use defaults (useful for studies) 
  int m_fUseEfficiencyRecovery; // switch to turn ON/OFF re-fit in order to recover efficiency

  int m_niter_fit1; // number of iterations for dR-dPhi scan 
  int m_niter_fit2; // number of iterations for MET-scan 
  int m_niter_fit3; // number of iterations for Mnu-scan 
  int m_NiterRandom; // number of random iterations (for lh, multiply or divide by 10 for ll and hh)
  int m_NsucStop;
  int m_RMSStop;
  int m_RndmSeedAltering; // reset seed (not necessary by default)

  int m_InputReorder; // flag for input re-order
  int m_LFVmode; // flag to determine which LFV decay to be reconstructed: 0=H->e+tau(mu) or 1=H->mu+tau(e) 

  int m_fJERsyst; // switch for JER systematics
  int m_METresSyst; // switch to turn on/off MET resolution systematics
  double m_dTheta3d_binMin; // minimal step size for dTheta3D
  double m_dTheta3d_binMax; // maximum step size for dTheta3D
  double m_dRmax_tau; // maximum dR(nu-visTau)
  double m_nsigma_METscan,m_nsigma_METscan2,m_nsigma_METscan_ll,m_nsigma_METscan_lh,m_nsigma_METscan_hh; // number of sigmas for MET-scan

  double m_beamEnergy; // beam energy (Tevatron=980, LHC-1=3500.0) 
  int m_METScanScheme; // MET-scan scheme: 0- use JER; 1- use simple sumEt & missingHt for Njet=0 events in (lep-had) 
  double m_MnuScanRange; // range of M(nunu) scan; M(nunu) range can be affected by selection cuts
  
  bool m_fUseDphiLL; //for leplep

  //---------------- protected functions
  void ClearDitauStuff(DitauStuff &fStuff);
  void ClearInputStuff();
  void ClearOutputStuff();
  void DoOutputInfo();
  void DoMetResolution(InputInfoStuff &fStuff);
  void PrintInputInfo(const InputInfoStuff & fStuff);
  void PrintOtherInput();
  void PrintResults();
  void FinalizeInputStuff();//const InputInfoStuff &rawInput, InputInfoStuff &preparedInput);
  int NuPsolution(TVector2 met_vec, double theta1, double phi1, 
		  double theta2, double phi2, double &P1, double &P2); // keep this version for simple tests


  inline int NuPsolutionV3(const double & mNu1, const double & mNu2, const double & phi1, const double & phi2, 
			   int & nsol1, int & nsol2);

  inline int NuPsolutionLFV(const TVector2 & met_vec, const TLorentzVector & tau, 
			    const double & m_nu, std::vector<TLorentzVector> &nu_vec);

  
  inline double tauSigmaE(const TLorentzVector & tauVec, const int tauType );
  inline double dTheta3Dparam(const int & parInd, const int & tau_type, const double & P_tau,const double *par); 

  //DR public:  
  double dTheta3d_probabilityFast(const int & tau_type,const double & dTheta3d,const double & P_tau);
 protected:
  inline double myDelThetaHadFunc(double *x, double *par);
  inline double myDelThetaLepFunc(double *x, double *par);

  inline double MetProbability(const double & met1,const  double & met2,const  double & MetSigma1, const double & MetSigma2);

  inline double MHtProbability(const double & d_mhtX, const double & d_mhtY, const double & mht, 
			       const double & trueMetGuess, const double & mht_offset);
  inline double MHtProbabilityHH(const double & d_mhtX, const double & d_mhtY, const double & mht, 
				 const double & trueMetGuess, const double & mht_offset);

  inline double mEtAndTauProbability();

  inline double TERProbabilitySimple(int tau_type, const TLorentzVector & vec, 
				     const double & Pt_scan);
  inline double TERSimpleSigma(int tau_type, const TLorentzVector & vec);

  inline void swapTLVPointers (TLorentzVector * & p1,TLorentzVector * & p2);
  inline double Angle(const TLorentzVector & vec1, const TLorentzVector & vec2);
  inline double AngularProbability(TLorentzVector nu_vec, TLorentzVector vis_vec, int decayType);
  inline int CheckSolutions(TLorentzVector nu_vec, TLorentzVector vis_vec, int decayType);
  inline double MnuProbability(double mnu, double binsize);
  inline double MnuProbability(double mnu);
  inline double ResonanceProbability(TLorentzVector vec1, TLorentzVector vec2);
  inline int TailCleanUp(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1, 
			 const int & type2, const TLorentzVector & vis2, const TLorentzVector & nu2, 
			 const double & mmc_mass, const double & vis_mass, const double & eff_mass, const double & dphiTT);

  inline double TauProbability(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1, 
			       const int & type2, const TLorentzVector & vis2, const TLorentzVector & nu2);
  inline double TauProbability(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1, 
			       const int & type2, const TLorentzVector & vis2, const TLorentzVector & nu2, const double & detmet);
  inline double TauProbabilityMatrix(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1, 
				     const int & type2, const TLorentzVector & vis2, const TLorentzVector & nu2);
  inline double TauProbabilityLFV(const int & type1, const TLorentzVector & vis1, const TLorentzVector & nu1);
  inline double MaxDelPhi(int tau_type, double Pvis);

  inline int refineSolutions (		    const double & M_nu1, const double & M_nu2,
					    const int nsol1, const int nsol2,
			      const double & Mvis, const double & Meff);

  inline double mT(const TLorentzVector & vec,const TVector2 & met_vec);
  
  
  inline void handleSolutions();

  inline double MassScale(int method, double mass, const int & tau_type1, const int & tau_type2);

  
  // factor out parameter space walking and probablity computing
  inline  int DitauMassCalculatorV9walk();
  

  // Calculates mass of lep+tau system in LFV X->lep+tau decays
  // It is based on DitauMassCalculatorV9, not optimized for speed yet, simple phase-space scan   
  inline int DitauMassCalculatorV9lfv();


  
  // only compute probability
  inline int probCalculatorV9fast(
				  const double & phi1, const double & phi2, 
				  const double & M_nu1, const double & M_nu2);
  
  // initialize the walker
  inline  void SpaceWalkerInit();
  
  //walk the walker
  inline bool SpaceWalkerWalk();
  
  //update tau kinematics if tau energy scan 
  inline void updateTauKine();


  inline bool precomputeCache();
  

  inline bool updateDouble  (const double in, double & out) ;
  inline bool checkMEtInRange  () ;
  inline bool checkAllParamInRange  () ;
  inline double fixPhiRange (const double & phi);
// fast (but approximate) calculation of sin and cos
  inline void fastSinCos (const double & phi, double & sinPhi, double & cosPhi);
  

  //----------------------------------------------
  //
  // ---------   Public methods ------------------ 
  //
  //______________________________________________

public:

  ~MissingMassCalculator() ;

  MissingMassCalculator() ;


  int RunMissingMassCalculator();
  
  //-------- Set Input Parameters
  void SetCalibrationSet(const MMCCalibrationSet::e aset) { m_mmcCalibrationSet=aset; }
  void SetSearchMode(const int val) { m_SearchMode=val; }
  void SetUseVerbose(const int val) { m_fUseVerbose=val; }
  void SetSpeedStudy(const int val) { m_fSpeedStudy=val; }
  void SetAlgorithmVersion(const int val) { m_AlgorithmVersion=val; }
  void SetUseTauProbability(const int val) { m_fUseTauProbability=val; }
  void SetUseMnuProbability(const int val) { m_fUseMnuProbability=val; }
  void SetUseDefaults(const int val) { m_fUseDefaults=val; }
  void SetUseEfficiencyRecovery(const int val) { m_fUseEfficiencyRecovery=val; }

  void SetNiterFit1(const int val) { m_niter_fit1=val; } // number of iterations per loop in dPhi loop
  void SetNiterFit2(const int val) { m_niter_fit2=val; } // number of iterations per loop in MET loop
  void SetNiterFit3(const int val) { m_niter_fit3=val; } // number of iterations per loop in Mnu loop
  void SetNiterRandom(const int val) { m_NiterRandom=val; } // number of random iterations
  void SetNsucStop(const int val) { m_NsucStop=val; } // Arrest criteria for Nsuccesses
  void SetRMSStop(const int val) { m_RMSStop=val;}
  void SetMeanbinStop(const double val) {m_meanbinStop=val;}
  void SetRndmSeedAltering(const int val) { m_RndmSeedAltering=val; } // number of iterations per loop in Mnu loop
  void SetMaxDRtau(const double val) { m_dRmax_tau=val; } // max value of dR
  void SetNsigmaMETscan(const double val) { m_nsigma_METscan=val; } // number of sigma's for MET-scan
  void SetNsigmaMETscan_ll(const double val) { m_nsigma_METscan_ll=val; } // number of sigma's for MET-scan in ll events
  void SetNsigmaMETscan_lh(const double val) { m_nsigma_METscan_lh=val; } // number of sigma's for MET-scan in lh events
  void SetNsigmaMETscan_hh(const double val) { m_nsigma_METscan_hh=val; } // number of sigma's for MET-scan in hh events
  void SetBeamEnergy(const double val) { m_beamEnergy=val; } // beam energy
  void SetdTheta3d_binMax(const double val) { m_dTheta3d_binMax=val; } // maximum step size for dTheta3D
  void SetdTheta3d_binMin(const double val) { m_dTheta3d_binMin=val; } // minimal step size for dTheta3D

  void SetMetVec(const TVector2 & vec);
  void SetVisTauVec(int i, const TLorentzVector & vec);
  void SetVisTauType(int i, int tautype);
  void SetNprong(int i, int nprong);
  void SetSumEt(double sumEt);
  void SetIsData(int val);
  void SetMetCovariance(double varX, double varY, double varXY); // MET covariance matrix
  void SetMetScanParams(double phi, double sigmaP, double sigmaL); // MET ellipse (deduced from MetCovariance)
  void SetMetScanParamsUE(double sumEt, double phi_scan=0.0, int data_code=0);
  void SetMetScanParamsJets(std::vector<TLorentzVector> jets);
  void SetJERsyst(const int val) { m_fJERsyst=val; }
  void SetMETresSyst(const int val) { m_METresSyst=val; } // MET resolution systematics: +/-1 sigma
  void SetApplyMassScale(const int val) { m_fApplyMassScale=val; } 
  void SetUseTailCleanup(const int val) { m_fUseTailCleanup=val; }
  void SetNjet25(int val);
  void SetAllowUseHT(const bool allowUseHT) { m_rawInput.allowUseHT=allowUseHT;}
  void SetMETScanScheme(const int val) { m_METScanScheme=val; }

  void SetMHtSigma1(const double val) { m_rawInput.m_MHtSigma1=val; }
  void SetMHtSigma2(const double val) { m_rawInput.m_MHtSigma2=val; } 
  void SetMHtGaussFr(const double val) { m_rawInput.MHtGaussFr=val; }
  void SetMnuScanRange(const double val) { m_MnuScanRange=val; }

  void SetProposalTryMEt(const double val) {m_proposalTryMEt=val; }
  void SetProposalTryPhi(const double val) {m_ProposalTryPhi=val;}
  void SetProposalTryMnu(const double val) {m_ProposalTryMnu=val;}
  void SetProposalTryEtau(const double val) {m_ProposalTryEtau=val;}

  // added by Tomas Davidek
  void SetUseDphiLL(const bool val) {m_fUseDphiLL = val;}

  void SetLFVmode(const int val) { m_LFVmode=val; }
  MMCCalibrationSet::e GetCalibrationSet() const { return m_mmcCalibrationSet; }
  int GetNiterFit1() const { return m_niter_fit1; } // number of iterations per loop in dPhi loop
  int GetNiterFit2() const { return m_niter_fit2; } // number of iterations per loop in MET loop
  int GetNiterFit3() const { return m_niter_fit3; } // number of iterations per loop in Mnu loop
  int GetNiterRandom() const { return m_niterRandomLocal; } // number of random iterations

  int GetNsucStop() const { return m_NsucStop; } // Arrest criteria for NSuc
  int GetRMSStop() const { return m_RMSStop; }
  double GetMeanbinStop() const { return m_meanbinStop;}
  int GetRndmSeedAltering() const { return m_RndmSeedAltering; } // number of iterations per loop in Mnu loop

  bool GetUseHT() const { return m_OutputInfo.UseHT; } // if use HT

  int GetMarkovCountDuplicate() const { return m_markovCountDuplicate; }
  int GetMarkovNRejectNoSol() const { return m_markovNRejectNoSol;}
  int GetMarkovNRejectMetropolis() const {return m_markovNRejectMetropolis;}
  int GetMarkovNAccept() const { return m_markovNAccept; }
  int GetMarkovNFullscan() const { return m_markovNFullScan;}
  double GetProposalTryMEt() const {return m_proposalTryMEt;}
  double GetProposalTryPhi() const {return m_ProposalTryPhi;}
  double GetProposalTryMnu() const {return m_ProposalTryMnu;}
  double GetProposalTryEtau() const {return m_ProposalTryEtau;}


  double GetmMaxError() const {return m_printmMaxError;}
  double GetmMeanError() const { return m_printmMeanError;}
  double GetmInvWidth2Error() const {return m_printmInvWidth2Error;}


  //-------- Get results;
  int GetFitStatus(); // return fit status
  double GetFittedMass(int fitcode); // returns fitted Mass
  std::shared_ptr<TH1F> GetMassHistogram(); // return mass histogram
  std::shared_ptr<TH1F> GetMassHistogramNoWeight(); // return mass histogram without weights
  int GetNTrials();// total number of point scanned
  int GetNSuccesses();// total number of point with at least 1 solutions
  int GetNSolutions();// total number of solutions 
  double GetSumW(); // sum of weights
  double GetAveSolRMS(); // ave RMS of solutions (for one event)

  double GetRms2Mpv(); // returns RMS/MPV according to histogram method
  TLorentzVector GetNeutrino4vec(int fitcode, int ind); // returns neutrino 4-vec
  double GetFitSignificance(int fitcode); // returns fit significance
  TLorentzVector GetTau4vec(int fitcode, int ind); // returns full tau 4-vec
  TLorentzVector GetResonanceVec(int fitcode); // returns 4-vec for resonance 
  TVector2 GetFittedMetVec(int fitcode); // returns 2-vec for fitted MET
  const AllPointsOutputInfo & GetAllPointsOutputInfo() { return m_aPOI;};
  int StandardCollApprox(const TLorentzVector & tau_vec1, const TLorentzVector & tau_vec2, const TVector2 & met_vec, double &Mrec); // standard collinear approximation
  // compute maximum from histo
  //  double maxFromHist(TH1F * theHist, double & prob, double & h_integral, double & h_chi2, double & h_discri, double & h_tantheta, double & h_tanthetaw, double & h_fitlength, double & h_rms, double & h_rmsvsdiscri, const MaxHistStrategy::e maxHistStrategy=MaxHistStrategy::FIT,const int winHalfWidth=2,bool debug=false);
  Double_t maxFitting(Double_t *x, Double_t *par);
  
  double maxFromHist(TH1F *theHist, std::vector<double> & histInfo, const MaxHistStrategy::e maxHistStrategy=MaxHistStrategy::FIT,const int winHalfWidth=2,bool debug=false);
  double maxFromHist(const std::shared_ptr<TH1F>& theHist, std::vector<double> & histInfo, const MaxHistStrategy::e maxHistStrategy=MaxHistStrategy::FIT,const int winHalfWidth=2,bool debug=false) {
    return maxFromHist(theHist.get(), histInfo, maxHistStrategy, winHalfWidth, debug);
  }

    // missing removed interfaces to turn on LFV code
    TVector2 metvec_tmp;
    inline double dTheta3DLimit(const int & tau_type, const int & limit_code,const double & P_tau);

};

#endif
