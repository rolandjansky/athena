/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeParametrizationHitBase_h
#define TFCSLateralShapeParametrizationHitBase_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrization.h"

class CaloGeometry;

class TFCSLateralShapeParametrizationHitBase:public TFCSLateralShapeParametrization {
public:
  TFCSLateralShapeParametrizationHitBase(const char* name=nullptr, const char* title=nullptr);

  // Call get_number_of_hits() only once per shower simulation, as it could be build with random numbers and give different results each time
  virtual int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const;

  class Hit
  {
    public:
    Hit():m_eta(0),m_phi(0),m_E(0) {}; // for hits with the same energy, m_E should normalized to E(layer)/nhit
    Hit(float eta, float phi, float E):m_eta(eta),m_phi(phi),m_E(E) {};

    float& eta() {return m_eta;};
    float& phi() {return m_phi;};
    float& E() {return m_E;};

    private:
    float m_eta,m_phi,m_E;
  };

  /// simulated one hit position with some energy. As last step in TFCSLateralShapeParametrizationHitChain::simulate, 
  /// the hit should be mapped into a cell and this cell recorded in simulstate. 
  /// All hits/cells should be resacled such that their final sum is simulstate->E(sample)
  virtual void simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:

  ClassDef(TFCSLateralShapeParametrizationHitBase,1)  //TFCSLateralShapeParametrizationHitBase
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSLateralShapeParametrizationHitBase+;
#endif

#endif
