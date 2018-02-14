/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHitCellMappingWiggleEMB_h
#define TFCSHitCellMappingWiggleEMB_h

#include "ISF_FastCaloSimEvent/TFCSHitCellMapping.h"

class TFCSHitCellMappingWiggleEMB:public TFCSHitCellMapping {
public:
  TFCSHitCellMappingWiggleEMB(const char* name=0, const char* title=0, ICaloGeometry* geo=0);

  // simulated one hit position with weight that should be put into simulstate
  // sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  // someone also needs to map all hits into cells
  virtual void simulate_hit(t_hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:
  //** Array for the hit-to-cell assignment accordion structure fix (wiggle)  **//
  //** To be moved to the conditions database at some point **//
  double m_wiggleLayer1[50];
  double m_wiggleLayer2[50];
  double m_wiggleLayer3[50];

  double doWiggle();

  ClassDef(TFCSHitCellMappingWiggleEMB,1)  //TFCSHitCellMappingWiggleEMB
};

#if defined(__MAKECINT__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSHitCellMappingWiggleEMB+;
#endif

#endif
