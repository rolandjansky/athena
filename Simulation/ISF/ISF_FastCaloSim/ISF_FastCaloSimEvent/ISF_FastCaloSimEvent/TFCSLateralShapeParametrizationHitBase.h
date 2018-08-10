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

  ///Call get_number_of_hits() only once per shower simulation, as it could be calculated with random numbers and give different results each time
  virtual int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const;

  class Hit
  {
    public:
    Hit():m_eta_x(0.),m_phi_y(0.),m_z(0.),m_E(0.),m_useXYZ(false) {}; // for hits with the same energy, m_E should normalized to E(layer)/nhit
    Hit(float eta, float phi, float E):m_eta_x(eta),m_phi_y(phi),m_E(E),m_useXYZ(false) {};
    Hit(float x, float y, float z, float E):m_eta_x(x),m_phi_y(y),m_z(z),m_E(E),m_useXYZ(true) {};
    
    inline void setEtaPhiZE(float eta,float phi,float z, float E){
      m_eta_x=eta;
      m_phi_y=phi;
      m_z=z;
      m_E=E;
      m_useXYZ=false;
    }
    inline void setXYZE(float x,float y,float z, float E){
      m_eta_x=x;
      m_phi_y=y;
      m_z=z;
      m_E=E;
      m_useXYZ=true;
    }

    inline void reset(){
      m_eta_x=0.;
      m_phi_y=0.;
      m_z=0.;
      m_E=0.;
      m_useXYZ=false;
    }

    inline float& eta() {return m_eta_x;};
    inline float& phi() {return m_phi_y;};
    inline float& x() {return m_eta_x;};
    inline float& y() {return m_phi_y;};
    inline float& E() {return m_E;};
    inline float& z() {return m_z;}
    inline float r() {
      if(m_useXYZ) return sqrt(m_eta_x*m_eta_x + m_phi_y*m_phi_y);
      else return m_z/asinh(m_eta_x);
    }

    private:
    float m_eta_x; // eta for barrel and end-cap, x for FCal
    float m_phi_y; // phi for barrel and end-cap, y for FCal
    float m_z;
    float m_E;
    bool m_useXYZ;
    
  };

  /// simulated one hit position with some energy. As last step in TFCSLateralShapeParametrizationHitChain::simulate, 
  /// the hit should be mapped into a cell and this cell recorded in simulstate. 
  /// All hits/cells should be resacled such that their final sum is simulstate->E(sample)
  virtual FCSReturnCode simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:

  ClassDefOverride(TFCSLateralShapeParametrizationHitBase,1)  //TFCSLateralShapeParametrizationHitBase
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSLateralShapeParametrizationHitBase+;
#endif

#endif
