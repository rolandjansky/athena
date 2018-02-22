/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSParametrizationChain_h
#define ISF_FASTCALOSIMEVENT_TFCSParametrizationChain_h

#include "ISF_FastCaloSimEvent/TFCSParametrization.h"

class TFCSParametrizationChain:public TFCSParametrization {
public:
  TFCSParametrizationChain(const char* name=nullptr, const char* title=nullptr):TFCSParametrization(name,title) {};
  TFCSParametrizationChain(const TFCSParametrizationChain& ref):TFCSParametrization(ref.GetName(),ref.GetTitle()),m_chain(ref.chain()) {};

  typedef std::vector< TFCSParametrizationBase* > Chain_t;
  Chain_t::size_type size() const {return m_chain.size();};
  const Chain_t& chain() const {return m_chain;};
  Chain_t& chain() {return m_chain;};
  void push_back(const Chain_t::value_type& param) {m_chain.push_back(param);recalc();};

  virtual void set_geometry(ICaloGeometry* geo);
  
  virtual bool is_match_Ekin_bin(int Ekin_bin) const;
  virtual bool is_match_calosample(int calosample) const;

  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option = "") const;
private:
  Chain_t m_chain;
  
  void recalc();

  ClassDef(TFCSParametrizationChain,1)  //TFCSParametrizationChain
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSParametrizationChain+;
#endif

#endif
