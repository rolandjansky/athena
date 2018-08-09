/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSHitCellMappingFCal_h
#define TFCSHitCellMappingFCal_h

#include "ISF_FastCaloSimEvent/TFCSHitCellMapping.h"

class ICaloGeometry;

class TFCSHitCellMappingFCal:public TFCSHitCellMapping {
public:
  TFCSHitCellMappingFCal(const char* name=nullptr, const char* title=nullptr, ICaloGeometry* geo=nullptr):TFCSHitCellMapping(name,title,geo){}
  
  virtual void simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

private:
  ClassDef(TFCSHitCellMappingFCal,1)  //TFCSHitCellMapping
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSHitCellMappingFCal+;
#endif

#endif
