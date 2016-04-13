/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeParametrization_h
#define TFCSLateralShapeParametrization_h

#include "ISF_FastCaloSimParametrization/TFCSParametrization.h"

class TFCSLateralShapeParametrization:public TFCSParametrization {
public:
  TFCSLateralShapeParametrization(const char* name=0, const char* title=0);

  bool is_match_Ekin_bin(int Ekin_bin) const {return Ekin_bin==m_Ekin_bin;};
  bool is_match_calosample(int calosample) const {return calosample==m_calosample;};
  
  int Ekin_bin() const {return m_Ekin_bin;};
  void set_Ekin_bin(int bin);

  int calosample() const {return m_calosample;};
  void set_calosample(int cs);

  // simulated one hit position with weight that should be put into simulstate
  // sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  // someone also needs to map all hits into cells
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option = "") const;
private:
  int m_Ekin_bin;
  int m_calosample;

  ClassDef(TFCSLateralShapeParametrization,1)  //TFCSLateralShapeParametrization
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSLateralShapeParametrization;
#endif

#endif
