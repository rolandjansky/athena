/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeParametrizationHitChain_h
#define TFCSLateralShapeParametrizationHitChain_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
#include <vector>

class TFCSLateralShapeParametrizationHitChain:public TFCSLateralShapeParametrization {
public:
  TFCSLateralShapeParametrizationHitChain(const char* name=0, const char* title=0);
  TFCSLateralShapeParametrizationHitChain(TFCSLateralShapeParametrizationHitBase* hitsim);

  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  virtual void set_geometry(ICaloGeometry* geo);
  
  typedef std::vector< TFCSLateralShapeParametrizationHitBase* > t_chain;
  t_chain::size_type size() {return m_chain.size();};
  t_chain& chain() {return m_chain;};
  t_chain::reference operator[](t_chain::size_type i) {return m_chain[i];};
  t_chain::const_reference operator[](t_chain::size_type i) const {return m_chain[i];};
  void push_back( const t_chain::value_type& value ) {m_chain.push_back(value);};
  
  virtual void set_number_of_hits_simul(TFCSLateralShapeParametrizationHitBase* sim) {m_number_of_hits_simul=sim;};
  
  virtual int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const;

  void Print(Option_t *option = "") const;
private:
  t_chain m_chain;
  TFCSLateralShapeParametrizationHitBase* m_number_of_hits_simul;
  ClassDef(TFCSLateralShapeParametrizationHitChain,1)  //TFCSLateralShapeParametrizationHitChain
};

#if defined(__MAKECINT__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSLateralShapeParametrizationHitChain+;
#endif

#endif
