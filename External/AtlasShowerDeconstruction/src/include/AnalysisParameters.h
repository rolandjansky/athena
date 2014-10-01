#ifndef ANALYSISPARAMETERS_H
#define ANALYSISPARAMETERS_H

#include "Parameters.h"

struct Parameter {
  int m_idx;
  std::string m_key;

  Parameter(int idx, std::string key) : m_idx(idx), m_key(key) {}
};

#define STR(x) #x
#define CREATE_PARAMETER(x) Parameter(p_ ## x, STR(x))

enum ParameterList {
  p_R = 0,
  p_ptmin_jet,
  p_n_of_jets,
  p_rapmax_jet,
  p_eta_cell,
  p_phi_cell,
  p_cell_ptcut,
  p_ptcut_microjets,
  p_masscut_microjets,
  p_lambda_mu_ext,
  p_lambda_theta_ext,
  p_cut_n_sub,
  p_fractionISR_of_hardInt_PT,
  p_noISR,
  p_topmass,
  p_topwindow,
  p_wmass,
  p_wwindow,
  p_Higgsmass,
  p_delta_Higgsmass,
  p_Rsmall,
  p_m_min_jet,
  p_br_wqq,
  p_useBtag,
  p_tagprob,
  p_fakeprob,
  p_nparam
};



class AnalysisParameters : public Deconstruction::Parameters {

  public:
    AnalysisParameters();
    AnalysisParameters(const std::string &input);

  private:
    void init();
};

#endif

