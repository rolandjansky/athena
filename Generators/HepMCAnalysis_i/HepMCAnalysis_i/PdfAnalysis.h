/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PdfAnalysis_H
#define PdfAnalysis_H

#include "baseAnalysis.h"

class TH1D;

class PdfAnalysis: public baseAnalysis
{
 public:

  PdfAnalysis();
  virtual ~PdfAnalysis();

  virtual int Init(double tr_max_eta, double tr_min_pt);
  virtual int Process(HepMC::GenEvent* hepmcevt);

  
 private:

  void InitPlots();

  TH1D* m_Qscale;

  TH1D* m_id_1pos_x;
  TH1D* m_id_2pos_x;
  TH1D* m_id_3pos_x;
  TH1D* m_id_4pos_x;
  TH1D* m_id_5pos_x;

  TH1D* m_id_1neg_x;
  TH1D* m_id_2neg_x;
  TH1D* m_id_3neg_x;
  TH1D* m_id_4neg_x;
  TH1D* m_id_5neg_x;

  TH1D* m_id_21_x;

 
};

#endif
