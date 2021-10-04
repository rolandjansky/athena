/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_RtScaleFunction_h
#define MuonCalib_RtScaleFunction_h

namespace MuonCalib {

    class IRtRelation;

    float RtScalePolynomial(const float &r);

    float RtScaleFunction(const float &t, const bool ml2, const IRtRelation &rtrelation);

}  // namespace MuonCalib

#endif
