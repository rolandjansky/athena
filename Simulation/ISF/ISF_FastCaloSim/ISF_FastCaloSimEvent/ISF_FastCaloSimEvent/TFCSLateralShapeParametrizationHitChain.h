/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeParametrizationHitChain_h
#define TFCSLateralShapeParametrizationHitChain_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
#include <vector>

class TFCSLateralShapeParametrizationHitChain:public TFCSLateralShapeParametrization {
public:
  TFCSLateralShapeParametrizationHitChain(const char* name=nullptr, const char* title=nullptr);
  TFCSLateralShapeParametrizationHitChain(TFCSLateralShapeParametrizationHitBase* hitsim);

  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  virtual void set_geometry(ICaloGeometry* geo);
  
  typedef std::vector< TFCSLateralShapeParametrizationHitBase* > Chain_t;
  Chain_t::size_type size() const {return m_chain.size();};
  const Chain_t& chain() const {return m_chain;};
  Chain_t& chain() {return m_chain;};
  void push_back( const Chain_t::value_type& value ) {m_chain.push_back(value);};
  
  virtual void set_number_of_hits_simul(TFCSLateralShapeParametrizationHitBase* sim) {m_number_of_hits_simul=sim;};
  
  /// Call get_number_of_hits() only once, as it could contain a random number
  virtual int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const;

  void Print(Option_t *option = "") const;
private:
  Chain_t m_chain;
  TFCSLateralShapeParametrizationHitBase* m_number_of_hits_simul;
  ClassDef(TFCSLateralShapeParametrizationHitChain,1)  //TFCSLateralShapeParametrizationHitChain
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSLateralShapeParametrizationHitChain+;
#endif

#endif
