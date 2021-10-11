/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/UniformShapeErrorGetter.h"

#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;

ShapeErrorData* UniformShapeErrorGetter::shapeErrorData(unsigned int /*hash*/, CaloGain::CaloGain /*gain*/, const Residual* /*toExclude*/) const
{
  TVectorD xi(32);
  TVectorD xip(32);
  CovMatrix xiErr(32);
  CovMatrix xipErr(32);

  for (unsigned int i = 0; i < 32; i++)
    xiErr(i, i) = TMath::Power(m_k, 2); 

  return new ShapeErrorData(xi, xip, xiErr, xipErr);
}
