/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSParametrizationEkinSelectChain_h
#define ISF_FASTCALOSIMEVENT_TFCSParametrizationEkinSelectChain_h

#include "ISF_FastCaloSimEvent/TFCSParametrizationFloatSelectChain.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

class TFCSParametrizationEkinSelectChain:public TFCSParametrizationFloatSelectChain {
public:
  TFCSParametrizationEkinSelectChain(const char* name=nullptr, const char* title=nullptr):TFCSParametrizationFloatSelectChain(name,title) {};
  TFCSParametrizationEkinSelectChain(const TFCSParametrizationEkinSelectChain& ref):TFCSParametrizationFloatSelectChain(ref) {};

  using TFCSParametrizationFloatSelectChain::push_back_in_bin;
  virtual void push_back_in_bin(TFCSParametrizationBase* param);
  //selects on truth->Ekin()
  //return -1 if outside range
  virtual int get_bin(TFCSSimulationState&,const TFCSTruthState* truth, const TFCSExtrapolationState*) const;
  virtual const std::string get_variable_text(TFCSSimulationState& simulstate,const TFCSTruthState*, const TFCSExtrapolationState*) const;
  virtual const std::string get_bin_text(int bin) const;

  static void unit_test(TFCSSimulationState* simulstate=nullptr,TFCSTruthState* truth=nullptr, const TFCSExtrapolationState* extrapol=nullptr);

protected:
  void recalc();

private:

  ClassDef(TFCSParametrizationEkinSelectChain,1)  //TFCSParametrizationEkinSelectChain
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSParametrizationEkinSelectChain+;
#endif

#endif
