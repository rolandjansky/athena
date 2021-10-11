/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class ScaledErrorData
   @brief Liquid Argon class for standalone storage of cell shape information
*/

#ifndef LArSamples_ScaledErrorData_H
#define LArSamples_ScaledErrorData_H

#include "LArSamplesMon/ShapeErrorData.h"

namespace LArSamples {

  class ScaledErrorData : public ShapeErrorData {
  
   public:
   
    /** @brief Constructor  */
    ScaledErrorData(const ShapeErrorData& sed, double scale = 1, double time = 0)
      : ShapeErrorData(sed), m_scale(scale), m_time(time) { }
    
    ScaledErrorData(const ScaledErrorData& other) :
      ShapeErrorData(other), m_scale(other.m_scale), m_time(other.m_time) { }

    virtual ~ScaledErrorData() { }
        
    double scale() const { return m_scale; }
    double time() const { return m_time; }
        
    const TVectorD  offsets(int first = -1, int last = -1) const;
    const CovMatrix errors(int first = -1, int last = -1) const;
  
    private:

    double m_scale, m_time;
  };
}

#endif

