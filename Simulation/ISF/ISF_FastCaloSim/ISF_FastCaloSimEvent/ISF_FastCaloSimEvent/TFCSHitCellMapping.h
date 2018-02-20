/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHitCellMapping_h
#define TFCSHitCellMapping_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"

class ICaloGeometry;

class TFCSHitCellMapping:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSHitCellMapping(const char* name=nullptr, const char* title=nullptr, ICaloGeometry* geo=nullptr);
  
  void set_geometry(ICaloGeometry* geo) {m_geo=geo;};
  ICaloGeometry* get_geometry() {return m_geo;};

  /// fills all hits into calorimeter cells
  virtual void simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  void Print(Option_t *option) const;

private:
  ICaloGeometry* m_geo; //! do not persistify

  ClassDef(TFCSHitCellMapping,1)  //TFCSHitCellMapping
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSHitCellMapping+;
#endif

#endif
