/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class UniformShapeErrorGetter
   @brief Liquid Argon base class for shape information
*/

#ifndef LArSamples_UniformShapeErrorGetter_H
#define LArSamples_UniformShapeErrorGetter_H

#include "LArSamplesMon/AbsShapeErrorGetter.h"

namespace LArSamples {

  class UniformShapeErrorGetter : public AbsShapeErrorGetter {
  
    public:
      
      UniformShapeErrorGetter(double k) : m_k(k) { }
      virtual ~UniformShapeErrorGetter() { }

      ShapeErrorData* shapeErrorData(unsigned int hash, CaloGain::CaloGain gain, const Residual* toExclude = 0) const;
      ShapeErrorData* phiSymShapeErrorData(short /*ring*/, CaloGain::CaloGain /*gain*/, const Residual* /*toExclude*/) const { return 0; }

    private:
      
      double m_k;
  };
}
#endif
