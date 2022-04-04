/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TMinuit_LHMM_h
#define TMinuit_LHMM_h

#include "TMinuit.h"

// minimal class designed to hide annoying warning messages from TMinuit

class TMinuit_LHMM : public TMinuit {

public:

  TMinuit_LHMM() { }
 TMinuit_LHMM(Int_t maxpar) : TMinuit(maxpar) { }

 void   mnrazz(Double_t ynew, Double_t *pnew, Double_t *y, Int_t &jh, Int_t &jl) override;

 void mnmnos() override;

 void mncuve() override;

#ifdef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
 ClassDefOverride(TMinuit_LHMM,0)
#endif
};
#endif
