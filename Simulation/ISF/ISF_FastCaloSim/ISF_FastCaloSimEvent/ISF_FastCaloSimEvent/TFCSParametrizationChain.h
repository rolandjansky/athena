/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSParametrizationChain_h
#define ISF_FASTCALOSIMEVENT_TFCSParametrizationChain_h

#include "ISF_FastCaloSimEvent/TFCSParametrization.h"

class TFCSParametrizationChain:public TFCSParametrization {
public:
  TFCSParametrizationChain(const char* name=0, const char* title=0):TFCSParametrization(name,title) {};
  TFCSParametrizationChain(const TFCSParametrizationChain& ref):TFCSParametrization(ref.GetName(),ref.GetTitle()),m_chain(ref.chain()) {};

  const std::vector< TFCSParametrizationBase* >& chain() const {return m_chain;};
  void push_back(TFCSParametrizationBase* param) {m_chain.push_back(param);recalc();};

  virtual void set_geometry(ICaloGeometry* geo);
  
  virtual bool is_match_Ekin_bin(int Ekin_bin) const;
  virtual bool is_match_calosample(int calosample) const;

  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option = "") const;
private:
  std::vector< TFCSParametrizationBase* > m_chain;
  
  void recalc();

  ClassDef(TFCSParametrizationChain,1)  //TFCSParametrizationChain
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSParametrizationChain+;
#endif

#endif
