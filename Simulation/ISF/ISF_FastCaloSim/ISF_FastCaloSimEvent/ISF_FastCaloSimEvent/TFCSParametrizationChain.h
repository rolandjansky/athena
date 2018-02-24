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
  virtual unsigned int size() const {return m_chain.size();};
  virtual const TFCSParametrizationBase* operator[](unsigned int ind) const {return m_chain[ind];};
  virtual TFCSParametrizationBase* operator[](unsigned int ind) {return m_chain[ind];};
  const Chain_t& chain() const {return m_chain;};
  Chain_t& chain() {return m_chain;};
  void push_back(const Chain_t::value_type& param) {m_chain.push_back(param);recalc();};

  virtual bool is_match_Ekin_bin(int Ekin_bin) const;
  virtual bool is_match_calosample(int calosample) const;

  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option = "") const;
protected:
  void recalc_pdgid_intersect();
  void recalc_pdgid_union();

  void recalc_Ekin_intersect();
  void recalc_eta_intersect();
  void recalc_Ekin_eta_intersect();

  void recalc_Ekin_union();
  void recalc_eta_union();
  void recalc_Ekin_eta_union();
  
  ///Default is to call recalc_pdgid_intersect() and recalc_Ekin_eta_intersect()
  virtual void recalc();
private:  
  Chain_t m_chain;

  ClassDef(TFCSParametrizationChain,1)  //TFCSParametrizationChain
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSParametrizationChain+;
#endif

#endif
