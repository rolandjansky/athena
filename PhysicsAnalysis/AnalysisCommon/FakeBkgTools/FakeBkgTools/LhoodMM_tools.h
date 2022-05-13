/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Lhood_MM_tools_h
#define Lhood_MM_tools_h

#include "Rtypes.h"
#include "TMatrixT.h"
#include "FakeBkgTools/BaseFakeBkgTool.h"
#include "FakeBkgTools/LhoodMMEvent.h"
#include "FakeBkgTools/LhoodMMFitInfo.h"

#include <vector>

class TH1;
class TH2;
class TH3;
class TMinuit_LHMM;
class TDirectory;

namespace CP
{

class LhoodMM_tools : public BaseFakeBkgTool
#ifndef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
  , public FakeBkgTools::ExtraPropertyManager<LhoodMM_tools, BaseFakeBkgTool>
#endif
{

  ASG_TOOL_CLASS(LhoodMM_tools, IFakeBkgTool)

 protected:

  virtual void reset();

  // implement base class methods
  virtual StatusCode addEventCustom() override;

 public:

  LhoodMM_tools(const std::string& name); 
  ~LhoodMM_tools();
  
  virtual StatusCode initialize() override;

  // implement base class methods

  virtual StatusCode getTotalYield(float& yield, float& statErrUp, float& statErrDown) override final;
  virtual StatusCode register1DHistogram(TH1* h1, const float *val) override;
  virtual StatusCode register2DHistogram(TH2* h2, const float *xval, const float *yval) override;
  virtual StatusCode register3DHistogram(TH3* h3, const float *xval, const float *yval, const float *zval) override;

  double nfakes(Double_t *poserr, Double_t *negerr); 
 
  double nfakes_std(double *error);
  double nfakes_std_perEventWeight(double *error);

  StatusCode setFitType(std::string ft);

  Int_t getFitStatus() {return m_fitStatus;}
  
  void setPrintLevel(Int_t printLevel) { m_printLevel = printLevel;}

  void set_do_std_perEventWeight(bool val) {m_do_std_perEventWeight = val;}
  
  virtual StatusCode saveProgress(TDirectory* dir) override;

  StatusCode mergeSubJobs();

  bool perfectFit() { return m_perfectFit;}
 protected:
  /// This indicates which type of efficiencies/fake factor need to be filled
  virtual FakeBkgTools::Client clientForDB() override final;

 private:

  // pointer to instance of LhoodMM_tools to use in a given fit
  static LhoodMM_tools* m_current_lhoodMM_tool;
  
  LhoodMMFitInfo m_global_fitInfo;
  bool m_prevSave;
  bool m_perfectFit;

  const LhoodMMFitInfo *m_current_fitInfo;

 #ifndef __CLING__
  std::vector<std::unique_ptr<FakeBkgTools::FinalState> > m_fsvec;
 #else
  std::vector<int> m_fsvec;
 #endif
  int m_curr_nlep;
  int m_minnlep, m_maxnlep, m_maxnlep_loose;
  int m_theta_tot_start_index;
  std::vector<std::vector<int>> m_real_indices, m_fake_indices;
  bool m_requireSS, m_requireOS;

  bool m_needToResize;

  bool m_doFakeFactor;

  bool m_fixNormalization;

  float m_maxWeight;

  bool m_alreadyMerged;

  inline static const int s_nLepMax = 6;
  inline static const int s_maxRank =  64; // i.e. 2^nLepMax


  std::vector < std::vector < std::vector < double > > > m_coeffs;
  
  std::map<TH1*, std::vector< LhoodMMFitInfo > > m_fitInfo_1dhisto_map;
  std::map<TH2*, std::vector< LhoodMMFitInfo > > m_fitInfo_2dhisto_map;
  std::map<TH3*, std::vector< LhoodMMFitInfo > > m_fitInfo_3dhisto_map;

  double m_dilep_SSfrac_num, m_dilep_SSfrac_denom;
  std::vector< std::vector < double > > m_OSfrac;

  double m_nfakes_std, m_nfakes_std_err;

  double m_nfakes_std_perEventWeight, m_nfakes_std_err_perEventWeight;

  bool m_do_std_perEventWeight;

  Int_t m_fitStatus;

  Int_t m_printLevel;

  std::vector<std::shared_ptr<TMatrixT<double>> > m_nrf_mat_vec;
  std::vector<std::shared_ptr<TMatrixT<double>> > m_MMmatrix_vec;
  std::vector<std::shared_ptr<TMatrixT<double>>> m_ntlpred_vec;

  unsigned m_lastSaveIndex;

  //mathematical constants
  inline static const double s_piover2 = 1.57079632679489661923;
  inline static const double s_piover4 = 0.785398163397448309616;

  StatusCode setup();

  static double logPoisson(double obs, double pred);

  StatusCode addEventCustom(const std::vector<bool>& isTight_vals,
       const std::vector<FakeBkgTools::Efficiency>& realEff_vals,
       const std::vector<FakeBkgTools::Efficiency>& fakeEff_vals,
       const std::vector<int>& charges,
       float weight);

  StatusCode incrementMatrices(const LhoodMMEvent& mmevt);
  StatusCode incrementOneMatrixSet(LhoodMMFitInfo& fitInfo,
				   const LhoodMMEvent& mmevt);

  static void fcn_nlep(Int_t &npar , Double_t *gin , Double_t &f, Double_t *par, Int_t iflag );
  static void fcn_minnlep_maxnlep(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

  void get_init_pars(std::vector<double> &init_pars, int nlep);
  void get_analytic(std::vector<double>& nrf, const int nlep);

  double fixPosErr(double n_fake_fit, TMinuit_LHMM* lhoodFit);
  double fixNegErr(double n_fake_fit, TMinuit_LHMM* lhoodFit);
  void mapLhood(TMinuit_LHMM* lhoodFit, int nlep, double min, double max);

  StatusCode fillHistograms();
  StatusCode fillHisto_internal(const std::vector< LhoodMMFitInfo >& fitInfo_vec, TH1* h);
};

}

#endif
