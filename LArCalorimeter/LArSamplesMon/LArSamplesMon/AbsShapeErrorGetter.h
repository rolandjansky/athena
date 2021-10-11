/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AbsShapeErrorGetter
   @brief Liquid Argon base class for shape information
*/

#ifndef LArSamples_AbsShapeErrorGetter_H
#define LArSamples_AbsShapeErrorGetter_H

#include "LArSamplesMon/ShapeErrorData.h"
#include "LArCafJobs/CaloId.h"
#include "CaloIdentifier/CaloGain.h"

class TH1D;

namespace LArSamples {

  class Residual;
  
  class AbsShapeErrorGetter {
  
    public:

      virtual ~AbsShapeErrorGetter() { }
      
      virtual ShapeErrorData* shapeErrorData(unsigned int hash, CaloGain::CaloGain gain, const Residual* toExclude = 0) const = 0;
      virtual ShapeErrorData* phiSymShapeErrorData(short ring, CaloGain::CaloGain gain, const Residual* toExclude = 0) const = 0;
  };
}
#endif
