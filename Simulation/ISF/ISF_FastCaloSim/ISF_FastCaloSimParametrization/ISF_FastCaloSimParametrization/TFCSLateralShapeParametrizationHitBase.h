/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeParametrizationHitBase_h
#define TFCSLateralShapeParametrizationHitBase_h

#include "ISF_FastCaloSimEvent/TFCSParametrization.h"

class CaloGeometry;

class TFCSLateralShapeParametrizationHitBase:public TFCSParametrization {
public:
  TFCSLateralShapeParametrizationHitBase(const char* name=0, const char* title=0);

  bool is_match_Ekin_bin(int Ekin_bin) const {return Ekin_bin==m_Ekin_bin;};
  bool is_match_calosample(int calosample) const {return calosample==m_calosample;};
  
  int Ekin_bin() const {return m_Ekin_bin;};
  void set_Ekin_bin(int bin);

  int calosample() const {return m_calosample;};
  void set_calosample(int cs);

  virtual void set_geometry(CaloGeometry* /*geo*/) {};

  // Call get_number_of_hits() only once per shower simulation, as it could be build with random numbers and give different results each time
  virtual int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const;

  class t_hit
  {
    public:
    t_hit():m_eta(0),m_phi(0),m_E(0) {}; // for hits with the same energy, m_E should normalized to E(layer)/nhit
    t_hit(float eta, float phi, float E):m_eta(eta),m_phi(phi),m_E(E) {};

    float& eta() {return m_eta;};
    float& phi() {return m_phi;};
    float& E() {return m_E;};

    private:
    float m_eta,m_phi,m_E;
  };

  // simulated one hit position with some energy. As last step in TFCSLateralShapeParametrizationHitChain::simulate, 
  // the hit should be mapped into a cell and this cell recorded in simulstate. 
  // All hits/cells should be resacled such that their final sum is simulstate->E(sample)
  virtual void simulate_hit(t_hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option = "") const;
private:
  int m_Ekin_bin;
  int m_calosample;

  ClassDef(TFCSLateralShapeParametrizationHitBase,1)  //TFCSLateralShapeParametrizationHitBase
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSLateralShapeParametrizationHitBase+;
#endif

#endif
