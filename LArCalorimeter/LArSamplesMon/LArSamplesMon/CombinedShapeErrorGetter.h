/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class CombinedShapeErrorGetter
   @brief Liquid Argon base class for shape information
*/

#ifndef LArSamples_CombinedShapeErrorGetter_H
#define LArSamples_CombinedShapeErrorGetter_H

#include "LArSamplesMon/AbsShapeErrorGetter.h"
#include <vector>

namespace LArSamples {

  class CombinedShapeErrorGetter : public AbsShapeErrorGetter {
  
    public:
      
      CombinedShapeErrorGetter() { }
      virtual ~CombinedShapeErrorGetter() { }
      
      void add(const AbsShapeErrorGetter& getter) { m_getters.push_back(&getter); }
      
      ShapeErrorData* shapeErrorData(unsigned int hash, CaloGain::CaloGain gain, const Residual* toExclude = 0) const;
      ShapeErrorData* phiSymShapeErrorData(short /*ring*/, CaloGain::CaloGain /*gain*/, const Residual* /*toExclude*/) const { return 0; }
      
    private:
      
      std::vector<const AbsShapeErrorGetter*> m_getters;
  };
}
#endif
