/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/CombinedShapeErrorGetter.h"

#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


ShapeErrorData* CombinedShapeErrorGetter::shapeErrorData(unsigned int hash, CaloGain::CaloGain gain, const Residual* /*toExclude*/) const
{
  TVectorD offsets(32);
  CovMatrix errors(32);
  ShapeErrorData* sed = 0;
  for (std::vector<const AbsShapeErrorGetter*>::const_iterator getter = m_getters.begin();
       getter != m_getters.end(); getter++) {
    const ShapeErrorData* other = (*getter)->shapeErrorData(hash, gain);
    if (!other) continue;
    if (!sed) 
      sed = new ShapeErrorData(*other);
    else {
      ShapeErrorData* newSed = sed->add(*other);
      delete sed;
      sed = newSed;
      if (!sed) return 0;
    }
    delete other;
  }
  return sed;
}
