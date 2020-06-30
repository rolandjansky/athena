/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserAnalysis_H
#define UserAnalysis_H

#include "baseAnalysis.h"

class TH1D;

class UserAnalysis: public baseAnalysis
{
 public:

  UserAnalysis();
  virtual ~UserAnalysis();

  virtual int Init(double tr_max_eta, double tr_min_pt);
  virtual int Process(HepMC::GenEvent* hepmcevt);
  
 private:
  
  TH1D *m_h_njets;
  TH1D *m_Ptjet;

};

#endif
