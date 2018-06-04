/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHitCellMappingWiggle_h
#define TFCSHitCellMappingWiggle_h

#include "ISF_FastCaloSimEvent/TFCSHitCellMapping.h"

class TFCS1DFunction;
class TH1;

class TFCSHitCellMappingWiggle:public TFCSHitCellMapping {
public:
  TFCSHitCellMappingWiggle(const char* name=nullptr, const char* title=nullptr, ICaloGeometry* geo=nullptr);
  ~TFCSHitCellMappingWiggle();
  
  void initialize(TFCS1DFunction* func);

  void initialize(TH1* histogram);

  /// modify one hit position to emulate the LAr accordeon shape
  /// and then fills all hits into calorimeter cells
  virtual void simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:
  //** Function for the hit-to-cell assignment accordion structure fix (wiggle)  **//
  //** To be moved to the conditions database at some point **//
  TFCS1DFunction* m_function{nullptr};

  ClassDef(TFCSHitCellMappingWiggle,1)  //TFCSHitCellMappingWiggle
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSHitCellMappingWiggle+;
#endif

#endif
