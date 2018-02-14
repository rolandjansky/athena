/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHitCellMapping_h
#define TFCSHitCellMapping_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"

class ICaloGeometry;

class TFCSHitCellMapping:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSHitCellMapping(const char* name=0, const char* title=0, ICaloGeometry* geo=0);
  
  void set_geometry(ICaloGeometry* geo) {m_geo=geo;};
  ICaloGeometry* get_geometry() {return m_geo;};

  // simulated one hit position with weight that should be put into simulstate
  // sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  // someone also needs to map all hits into cells
  virtual void simulate_hit(t_hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option) const;

private:
  ICaloGeometry* m_geo; //! do not persistify

  ClassDef(TFCSHitCellMapping,1)  //TFCSHitCellMapping
};

#if defined(__MAKECINT__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSHitCellMapping+;
#endif

#endif
