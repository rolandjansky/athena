/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LINEARITY_TOOL_H 
#define LINEARITY_TOOL_H 

#include "MissingETData.h"

#include <string>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TFile.h"

class LinearityTool {

public:

  LinearityTool();
  ~LinearityTool() {}
  void initialize(TFile *);

  void execute(MissingETData *data);
  void linearity(MissingETData *data);
  std::string m_folderName;

  float linplot_x;
  float linplot_y;
  std::string linplot_text;

  //histograms
  TH1D *hl_tot;
  TH1D *hl2_tot;
  TH1D *hl3_tot;
  TH1D *hl4_tot;
  TH1D *hl5_tot;
  TH1D *hl6_tot;
  TH1D *hl7_tot;

  TH1D *hletmi;
  TH1D *hl2etmi;
  TH1D *hl3etmi;
  TH1D *hl4etmi;
  TH1D *hl5etmi;
  TH1D *hl6etmi;
  TH1D *hl7etmi;

  TH1D *h_RefEle_METup;
  TH1D *h_RefJet_METup;
  TH1D *h_CellOut_METup;
  TH1D *h_refjetpoutup; 
  TH1D *h_RefEle_METlow;
  TH1D *h_RefJet_METlow;
  TH1D *h_CellOut_METlow;
  TH1D *h_refjetpoutlow; 

  TH1D *hcount_threshold;

  //donatella curvalin
  TGraphErrors *grlin;
  std::vector<TH1F *> hl_;
  std::vector<TH1F *> hl2_;
  std::vector<TH1F *> hl3_;
  std::vector<TH1F *> hl4_;
  std::vector<TH1F *> hl5_;
  std::vector<TH1F *> hl6_;
  std::vector<TH1F *> hl7_;
  TH2F *he_linvset;

  int m_lnbini;
  int m_lnumptt;
  float m_lstartpt;
  float m_lbinpt;
  float m_fitrangel;
  float m_curvaslin;
  float m_curvasETmi;
  float m_percerr;
  int m_entries;
  float m_fitr;
  int m_linorm;
  float m_bin_rp3;
  float m_suptmi;
  float m_etmi_threshold;
};

#endif // LINEARITY_TOOL_H 
