/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MuonTriggerSFTool_H
#define MuonTriggerSFTool_H

#include "TObject.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TAxis.h"
#include "TFile.h"
#include <cmath>
#include <iostream>
#include <utility>

class MuonTriggerSFTool : public TObject
{
public:
  MuonTriggerSFTool();
  ~MuonTriggerSFTool();

  void initialize(std::string filename1 = "data/trigger/SF_for_EPS.root",
		  std::string filename2 = "data/trigger/SF_barrel_ptdep.root"
		  );

  void SetMu18OR40(const bool& v = true) { m_18or40 = v; }
  void SetVerbose(const bool& v = true) { m_verbose = v; }

  float GetScaleFactor(float eta, float phi, float pt /* GeV */);

private:

  bool m_isloaded;
  bool m_18or40;
  bool m_verbose;

  TFile* m_file1;
  TFile* m_file2;

  TH1F * h_sf_endcap;
  TH2F * h_sf_barrel;
  TH1F * h_corr_barrel;

  TAxis* m_ax;
  TAxis* m_ay;

  #ifndef SUSY_ATHENA
  ClassDef(MuonTriggerSFTool,2);
  #endif
};

#endif // not MuonTriggerSFTool_H

