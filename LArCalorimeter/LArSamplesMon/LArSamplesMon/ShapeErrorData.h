/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class ShapeErrorData
   @brief Liquid Argon class for standalone storage of cell shape information
*/

#ifndef LArSamples_ShapeErrorData_H
#define LArSamples_ShapeErrorData_H

#include "LArSamplesMon/IndexRange.h"
#include "TVectorD.h"
#include "LArCafJobs/Definitions.h"

namespace LArSamples {

  class ShapeErrorData : public IndexRange {
  
   public:
   
    /** @brief Constructor  */
    ShapeErrorData(const TVectorD& xi = TVectorD(), const TVectorD& xip = TVectorD(),
                   const CovMatrix& xiErr = CovMatrix(), 
                   const CovMatrix& xipErr = CovMatrix(),
                   double tbar = Definitions::none, int n = -1)
      : m_xi(xi), m_xip(xip), m_xiErr(xiErr), m_xipErr(xipErr), m_tbar(tbar), m_n(n) { }
    
    ShapeErrorData(const ShapeErrorData& other) :
      IndexRange(),
      m_xi(other.m_xi), m_xip(other.m_xip), m_xiErr(other.m_xiErr), m_xipErr(other.m_xipErr), 
      m_tbar(other.m_tbar), m_n(other.m_n) { }

    virtual ~ShapeErrorData() { }
    
    /** @return size */
    unsigned int nSamples() const { return m_xi.GetNrows(); }

    const TVectorD&  xi()     const { return m_xi; }
    const TVectorD&  xip()    const { return m_xip; }
    const CovMatrix& xiErr()  const { return m_xiErr; }
    const CovMatrix& xipErr() const { return m_xipErr; }

    int lwb() const { return m_xi.GetLwb(); }
    int upb() const { return m_xi.GetUpb(); }
    
    const TVectorD  xi    (int first, int last) const;
    const TVectorD  xip   (int first, int last) const;
    const CovMatrix xiErr (int first, int last) const;
    const CovMatrix xipErr(int first, int last) const;

    double tbar() const { return m_tbar; }
    int    n()    const { return m_n; }
    
    ShapeErrorData* add(const ShapeErrorData& other) const;
  
    ShapeErrorType shapeErrorType() const { return m_shapeErrorType; }
    void setShapeErrorType(ShapeErrorType type) const { m_shapeErrorType = type; }
    
    private:

    TVectorD m_xi, m_xip;
    CovMatrix m_xiErr, m_xipErr;
    double m_tbar;
    int m_n;
    mutable ShapeErrorType m_shapeErrorType;
  };
}

#endif

