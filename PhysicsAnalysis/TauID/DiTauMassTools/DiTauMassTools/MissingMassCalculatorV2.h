/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
------ MissingMassCalculator
------ Author: Aliaksandr Pranko (appranko@lbl.gov)
------ Code developers: David Rousseau (rousseau@lal.in2p3.fr), Dimitris Varouchas (Dimitris.Varouchas@cern.ch)
MissingMassCalculator is designed to reconstruct mass in
events where two particles decay into states with missing ET.
*/
#ifndef MissingMassCalculatorV2_h
#define MissingMassCalculatorV2_h



#if !defined (__CINT__) || defined (__MAKECINT__)

#include <TRandom2.h>
#include <TH1.h>
#include <TGraph.h>
#include <TF1.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <vector>
#include <TObject.h>
#include <TDirectory.h>

#include <memory>
#include <string>

#include "DiTauMassTools/MissingMassInput.h"
#include "DiTauMassTools/MissingMassOutput.h"
#include "DiTauMassTools/MissingMassProb.h"
#include "DiTauMassTools/HelperFunctions.h"

#include "xAODMissingET/MissingET.h"

#endif


namespace DiTauMassTools{

class MissingMassCalculatorV2 {

 public:

 private:

  //---------------- structures
  struct DitauStuff {
    double Mditau_best; // best fitted M(ditau)
    double Sign_best;   // best significance of M(ditau) fit
    TLorentzVector nutau1;  // fitted 4-vec for neutrino from tau-1
    TLorentzVector nutau2;  // fitted 4-vec for neutrino from tau-2
    TLorentzVector vistau1;  // fitted 4-vec for visible tau-1
    TLorentzVector vistau2;  // fitted 4-vec for visible tau-2
    double RMSoverMPV;
  };

  TRandom2 m_randomGen;
  
  MMCCalibrationSetV2::e m_mmcCalibrationSet;

  bool m_fUseEfficiencyRecovery; // switch to turn ON/OFF re-fit in order to recover efficiency
  bool m_fUseFloatStopping; // switch to turn ON/OFF floating stopping criterion

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

  TLorentzVector m_tautau_tmp;

  bool m_debugThisIteration;
  
  int m_nCallprobCalculatorV9fast;
  
  double m_nsigma_METscan,m_nsigma_METscan2,m_nsigma_METscan_ll,m_nsigma_METscan_lh,m_nsigma_METscan_hh; // number of sigmas for MET-scan

  int m_iter1,m_iter2,m_iter3,m_iter4,m_iter5,m_iang1low,m_iang1high,m_iang2low,m_iang2high;
  int m_iterTheta3d;
  
  double m_prob_tmp;
  
  double m_totalProbSum;
  double m_mtautauSum;

  int m_eventNumber;
  int m_seed;
  // data member for the spaceWalker approach

  int m_iter0;
  int m_iterNuPV3;
  int m_testptn1;
  int m_testptn2;
  int m_testdiscri1;
  int m_testdiscri2;
  int m_nosol1;
  int m_nosol2;
  int m_iterNsuc;
  bool m_switch1;
  bool m_switch2;

  bool m_meanbinToBeEvaluated;
  
  int m_markovCountDuplicate;
  int m_markovNFullScan;
  int m_markovNRejectNoSol;
  int m_markovNRejectMetropolis;
  int m_markovNAccept;

  double m_PrintmMaxError;
  double m_PrintmMeanError;
  double m_PrintmInvWidth2Error;

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
  double m_metCovPhiCos,m_metCovPhiSin;
  double m_eTau1Proposal,m_eTau2Proposal;

  double m_eTau1Min,m_eTau1Max,m_eTau1Range;
  double m_eTau2Min,m_eTau2Max,m_eTau2Range;
  bool m_fullParamSpaceScan;
  bool m_Mnu1Exclude;
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


  double m_Mnu1ExcludeMin,m_Mnu1ExcludeMax,m_Mnu1ExcludeRange;
  double m_Mnu1XMin,m_Mnu1XMax, m_Mnu1XRange;
  double m_walkWeight;
  double m_cosPhi1, m_cosPhi2, m_sinPhi1, m_sinPhi2;
  
  bool m_scanMnu1,m_scanMnu2;

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
  double m_Mvis,m_Meff;
  bool m_reRunWithBestMET;
  
  //--- define histograms for histogram method
  //--- upper limits need to be revisied in the future!!! It may be not enough for some analyses
  std::shared_ptr<TH1F> m_fMfit_all;
  std::shared_ptr<TH1F> m_fMEtP_all;
  std::shared_ptr<TH1F> m_fMEtL_all;
  std::shared_ptr<TH1F> m_fMnu1_all;
  std::shared_ptr<TH1F> m_fMnu2_all;
  std::shared_ptr<TH1F> m_fPhi1_all;
  std::shared_ptr<TH1F> m_fPhi2_all;
  std::shared_ptr<TGraph> m_fMfit_allGraph;
  std::shared_ptr<TH1F> m_fMfit_allNoWeight;
  
  std::shared_ptr<TH1F> m_fPXfit1;
  std::shared_ptr<TH1F> m_fPYfit1;
  std::shared_ptr<TH1F> m_fPZfit1;
  std::shared_ptr<TH1F> m_fPXfit2;
  std::shared_ptr<TH1F> m_fPYfit2;
  std::shared_ptr<TH1F> m_fPZfit2;

  // these histograms are used for the floating stopping criterion
  std::shared_ptr<TH1F> m_fMmass_split1;
  std::shared_ptr<TH1F> m_fMEtP_split1;
  std::shared_ptr<TH1F> m_fMEtL_split1;
  std::shared_ptr<TH1F> m_fMnu1_split1;
  std::shared_ptr<TH1F> m_fMnu2_split1;
  std::shared_ptr<TH1F> m_fPhi1_split1;
  std::shared_ptr<TH1F> m_fPhi2_split1;
  std::shared_ptr<TH1F> m_fMmass_split2;
  std::shared_ptr<TH1F> m_fMEtP_split2;
  std::shared_ptr<TH1F> m_fMEtL_split2;
  std::shared_ptr<TH1F> m_fMnu1_split2;
  std::shared_ptr<TH1F> m_fMnu2_split2;
  std::shared_ptr<TH1F> m_fPhi1_split2;
  std::shared_ptr<TH1F> m_fPhi2_split2;

  TF1 *m_fFitting;

  TH1F* m_fPhi1;
  TH1F* m_fPhi2;
  TH1F* m_fMnu1;
  TH1F* m_fMnu2;
  TH1F* m_fMetx;
  TH1F* m_fMety;
  TH1F* m_fTheta3D;
  TH1F* m_fTauProb;

  // for intermediate calc
  TLorentzVector m_TLVdummy;

  //---------------- protected variables
  DitauStuff m_fDitauStuffFit; // results based on fit method
  DitauStuff m_fDitauStuffHisto; // results based on histo method

  int m_fApplyMassScale; // switch to apply mass scale correction

  int m_niter_fit1; // number of iterations for dR-dPhi scan 
  int m_niter_fit2; // number of iterations for MET-scan 
  int m_niter_fit3; // number of iterations for Mnu-scan 
  int m_NiterRandom; // number of random iterations (for lh, multiply or divide by 10 for ll and hh)
  int m_NsucStop;
  int m_RMSStop;
  int m_RndmSeedAltering; // reset seed (not necessary by default)


  int m_fJERsyst; // switch for JER systematics
  double m_dTheta3d_binMin; // minimal step size for dTheta3D
  double m_dTheta3d_binMax; // maximum step size for dTheta3D
  double m_dRmax_tau; // maximum dR(nu-visTau)

  double m_MnuScanRange; // range of M(nunu) scan; M(nunu) range can be affected by selection cuts
  

  //---------------- protected functions
  void ClearDitauStuff(DitauStuff &fStuff);
  void DoOutputInfo();
  void PrintOtherInput();
  void PrintResults();
  int NuPsolution(TVector2 met_vec, double theta1, double phi1, 
		  double theta2, double phi2, double &P1, double &P2); // keep this version for simple tests


  inline int NuPsolutionV3(const double & mNu1, const double & mNu2, const double & phi1, const double & phi2, 
			   int & nsol1, int & nsol2);

  inline int NuPsolutionLFV(const TVector2 & met_vec, const TLorentzVector & tau, 
			    const double & m_nu, std::vector<TLorentzVector> &nu_vec);

  
 protected:
  inline int CheckSolutions(TLorentzVector nu_vec, TLorentzVector vis_vec, int decayType);
  inline int TailCleanUp(const TLorentzVector & vis1, const TLorentzVector & nu1, 
			 const TLorentzVector & vis2, const TLorentzVector & nu2, 
			 const double & mmc_mass, const double & vis_mass, const double & eff_mass, const double & dphiTT);


  inline int refineSolutions (		    const double & M_nu1, const double & M_nu2,
					    const int nsol1, const int nsol2,
			      const double & Mvis, const double & Meff);

  
  
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

  inline bool precomputeCache();
  

  inline bool checkMEtInRange  () ;
  inline bool checkAllParamInRange  () ;

  //----------------------------------------------
  //
  // ------------   Public methods ---------------
  //
  //______________________________________________

public:

  ~MissingMassCalculatorV2() ;

  MissingMassCalculatorV2(MMCCalibrationSetV2::e aset, std::string m_paramFilePath) ;

  MissingMassInput preparedInput;
  MissingMassOutput OutputInfo;
  MissingMassProb* Prob;

  int RunMissingMassCalculator( const xAOD::IParticle* part1, const xAOD::IParticle* part2, const xAOD::MissingET* met, const int& njets );

  //-------- Set Input Parameters
  void FinalizeSettings(const xAOD::IParticle* part1, const xAOD::IParticle* part2, const xAOD::MissingET* met, const int& njets );
  void SetNiterFit1(const int val) { m_niter_fit1=val; } // number of iterations per loop in dPhi loop
  void SetNiterFit2(const int val) { m_niter_fit2=val; } // number of iterations per loop in MET loop
  void SetNiterFit3(const int val) { m_niter_fit3=val; } // number of iterations per loop in Mnu loop
  void SetNiterRandom(const int val) { m_NiterRandom=val; } // number of random iterations
  void SetNsucStop(const int val) { m_NsucStop=val; } // Arrest criteria for Nsuccesses
  void SetRMSStop(const int val) { m_RMSStop=val;}
  void SetMeanbinStop(const double val) {m_meanbinStop=val;}
  void SetRndmSeedAltering(const int val) { m_RndmSeedAltering=val; } // number of iterations per loop in Mnu loop
  void SetdTheta3d_binMax(const double val) { m_dTheta3d_binMax=val; } // maximum step size for dTheta3D
  void SetdTheta3d_binMin(const double val) { m_dTheta3d_binMin=val; } // minimal step size for dTheta3D
  void SetEventNumber(const int eventNumber) { m_eventNumber = eventNumber; }

  void SetJERsyst(const int val) { m_fJERsyst=val; }
  void SetApplyMassScale(const int val) { m_fApplyMassScale=val; } 

  void SetMnuScanRange(const double val) { m_MnuScanRange=val; }

  void SetProposalTryMEt(const double val) {m_proposalTryMEt=val; }
  void SetProposalTryPhi(const double val) {m_ProposalTryPhi=val;}
  void SetProposalTryMnu(const double val) {m_ProposalTryMnu=val;}
  void SetProposalTryEtau(const double val) {m_ProposalTryEtau=val;}

  void SetUseEfficiencyRecovery(const  bool val) { m_fUseEfficiencyRecovery=val; }
  bool GetUseEfficiencyRecovery() const { return m_fUseEfficiencyRecovery; }

  int GetNiterFit1() const { return m_niter_fit1; } // number of iterations per loop in dPhi loop
  int GetNiterFit2() const { return m_niter_fit2; } // number of iterations per loop in MET loop
  int GetNiterFit3() const { return m_niter_fit3; } // number of iterations per loop in Mnu loop
  int GetNiterRandom() const { return m_niterRandomLocal; } // number of random iterations

  int GetNsucStop() const { return m_NsucStop; } // Arrest criteria for NSuc
  int GetRMSStop() const { return m_RMSStop; }
  double GetMeanbinStop() const { return m_meanbinStop;}
  int GetRndmSeedAltering() const { return m_RndmSeedAltering; } // number of iterations per loop in Mnu loop

  int GetMarkovCountDuplicate() const { return m_markovCountDuplicate; }
  int GetMarkovNRejectNoSol() const { return m_markovNRejectNoSol;}
  int GetMarkovNRejectMetropolis() const {return m_markovNRejectMetropolis;}
  int GetMarkovNAccept() const { return m_markovNAccept; }
  int GetMarkovNFullscan() const { return m_markovNFullScan;}
  double GetProposalTryMEt() const {return m_proposalTryMEt;}
  double GetProposalTryPhi() const {return m_ProposalTryPhi;}
  double GetProposalTryMnu() const {return m_ProposalTryMnu;}
  double GetProposalTryEtau() const {return m_ProposalTryEtau;}

  void SetNsigmaMETscan_ll(const double val) { m_nsigma_METscan_ll=val; } // number of sigma's for MET-scan in ll events
  void SetNsigmaMETscan_lh(const double val) { m_nsigma_METscan_lh=val; } // number of sigma's for MET-scan in lh events
  void SetNsigmaMETscan_hh(const double val) { m_nsigma_METscan_hh=val; } // number of sigma's for MET-scan in hh events
  void SetNsigmaMETscan(const double val) { m_nsigma_METscan=val; } // number of sigma's for MET-scan

  void SetUseFloatStopping(const bool val) { m_fUseFloatStopping=val; } // switch for floating stopping criterion

  double GetmMaxError() const {return m_PrintmMaxError;}
  double GetmMeanError() const { return m_PrintmMeanError;}
  double GetmInvWidth2Error() const {return m_PrintmInvWidth2Error;}

  int GetNNoSol() const {return m_markovNRejectNoSol;}
  int GetNMetroReject() const {return m_markovNRejectMetropolis;}
  int GetNSol() const {return m_markovNAccept;}


  //-------- Get results;
  int StandardCollApprox(const TLorentzVector & tau_vec1, const TLorentzVector & tau_vec2, const TVector2 & met_vec, double &Mrec); // standard collinear approximation
  Double_t maxFitting(Double_t *x, Double_t *par);
  
  // compute maximum from histo
  double maxFromHist(TH1F *theHist, std::vector<double> & histInfo, const MaxHistStrategyV2::e maxHistStrategy=MaxHistStrategyV2::FIT,const int winHalfWidth=2,bool debug=false);
  double maxFromHist(const std::shared_ptr<TH1F>& theHist, std::vector<double> & histInfo, const MaxHistStrategyV2::e maxHistStrategy=MaxHistStrategyV2::FIT,const int winHalfWidth=2,bool debug=false) {
    return maxFromHist(theHist.get(), histInfo, maxHistStrategy, winHalfWidth, debug);
  }

  TVector2 metvec_tmp;
  inline double dTheta3DLimit(const int & tau_type, const int & limit_code,const double & P_tau);

};
} // namespace DiTauMassTools

#endif
