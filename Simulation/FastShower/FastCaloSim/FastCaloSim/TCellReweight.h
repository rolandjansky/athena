/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TCellReweight_h
#define TCellReweight_h

#include "FastCaloSim/TLateralShapeCorrection.h"

//////////////////////////////////////////
// HitCell reweight class
//////////////////////////////////////////

class THitCellReweight:public TLateralShapeCorrection {
protected:
  float m_factor;
  
  virtual std::string prefix() const {return "HitCellReweight";};
public:
  THitCellReweight();
  
  virtual std::string str() const;
  
  void set_factor(float factor) {m_factor=factor;SetStandardName();};
  float factor() const {return m_factor;};

  virtual float cellfactor(int deta,int dphi) const;

  ClassDef(THitCellReweight,1)  //THitCellReweight
};

#endif
