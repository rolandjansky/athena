/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class ScaledShiftedShape
   @brief Liquid Argon class for standalone storage of cell shape information
*/

#ifndef LArSamples_ScaledShiftedShape_H
#define LArSamples_ScaledShiftedShape_H

#include "LArCafJobs/AbsShape.h"

namespace LArSamples {
  
  class ScaledShiftedShape : public AbsShape {
  
   public:
   
    /** @brief Constructor  */
    ScaledShiftedShape(const AbsShape& base, double scaling = 1, double shift = 0)
      : m_base(&base), m_scaling(scaling), m_shift(shift) { }
  
    ScaledShiftedShape(const ScaledShiftedShape& other) 
      : AbsShape(), m_base(&other.base()), m_scaling(other.scaling()), m_shift(other.shift()) { }
  
    virtual ~ScaledShiftedShape() { }
    
    const AbsShape& base() const { return *m_base; }
    double scaling() const { return m_scaling; } 
    double shift() const { return m_shift; }

    unsigned int nPoints()  const { return base().nPoints(); }  
    double value(unsigned int i) const { return scaling()*base().value(i); }
    double error(unsigned int i) const { return scaling()*base().error(i); }
    double time(unsigned int i) const { return base().time(i) + shift(); }
    double covariance(unsigned int i, unsigned int j) const { return base().covariance(i, j)*scaling()*scaling(); }
    
  private:

    const AbsShape* m_base;
    double m_scaling, m_shift;
  };
}

#endif

