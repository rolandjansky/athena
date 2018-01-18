/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHitCellMappingWiggleEMB_h
#define TFCSHitCellMappingWiggleEMB_h

#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrizationHitBase.h"

class CaloGeometry;

class TFCSHitCellMappingWiggleEMB:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSHitCellMappingWiggleEMB(const char* name=0, const char* title=0, CaloGeometry* geo=0);
  
  virtual void set_geometry(CaloGeometry* geo) {m_geo=geo;};
  CaloGeometry* get_geometry() {return m_geo;};

  // simulated one hit position with weight that should be put into simulstate
  // sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  // someone also needs to map all hits into cells
  virtual void simulate_hit(t_hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option) const;

private:
  CaloGeometry* m_geo; //! do not persistify
  
  //** Array for the hit-to-cell assignment accordion structure fix (wiggle)  **//
  //** To be moved to the conditions database at some point **//
  double m_wiggleLayer1[50];
  double m_wiggleLayer2[50];
  double m_wiggleLayer3[50];

  double doWiggle();

  ClassDef(TFCSHitCellMappingWiggleEMB,1)  //TFCSHitCellMappingWiggleEMB
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSHitCellMappingWiggleEMB+;
#endif

#endif
