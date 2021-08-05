/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamplesOFC
   @brief Liquid Argon class for standalone storage of cell shape information
*/

#ifndef LArSamples_OFC_H
#define LArSamples_OFC_H

#include "LArSamplesMon/IndexRange.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TArrayI.h"
#include <vector>

#include "LArCafJobs/Definitions.h"

namespace LArSamples {

  class AbsShape;
  class ShapeErrorData;
  
  class OFC : public IndexRange {
  
   public:
   
    /** @brief Constructor  */
    OFC(const AbsShape& shape, const AbsShape& data, int lwb = -1, int upb = -1, const ShapeErrorData* sed = 0, bool withAutoCorr = true);
    
    OFC(const AbsShape& shape, const AbsShape& data, const CovMatrix& refErr, 
        int lwb = -1, int upb = -1, const ShapeErrorData* sed = 0, bool withAutoCorr = true);
    
    bool initGValues(const AbsShape& shape, const AbsShape& data, const ShapeErrorData* sed);
    bool initOFCs(const AbsShape& data, const CovMatrix& refErr, bool useCorrs);
    bool initRVectors();

    OFC(const OFC& other) :
	IndexRange(),
        m_g(other.m_g), m_gp(other.m_gp), m_a(other.m_a), m_b(other.m_b),
        m_G(other.m_G), m_Gp(other.m_Gp), m_Gpp(other.m_Gpp),
        m_invGamma(other.m_invGamma) { }

    virtual ~OFC();

    unsigned int nSamples() const { return m_g.GetNoElements(); }

    int lwb() const { return g().GetLwb(); }
    int upb() const { return g().GetUpb(); }

    double g(unsigned int i) const { return m_g[i]; }
    double gp(unsigned int i) const { return m_gp[i]; }
    
    const TVectorD& g() const { return m_g; }
    const TVectorD& gp() const { return m_gp; }
    
    double G() const { return m_G; }
    double Gp() const { return m_Gp; }
    double Gpp() const { return m_Gpp; }

    double a(unsigned int i) const { return m_a[i]; }
    double b(unsigned int i) const { return m_b[i]; }
    
    const TVectorD& a() const { return m_a; }
    const TVectorD& b() const { return m_b; }

    const TVectorD& r(unsigned int i) const { return m_r[i]; }
    unsigned int rIdx(unsigned int i) const { return m_rIdx[i]; }
    
    const TMatrixD& resProj() const { return m_resProj; }

    double A(const AbsShape& data) const;
    double B(const AbsShape& data) const;
    double time(const AbsShape& data) const { return B(data)/A(data); }

    const CovMatrix& invGamma() const { return m_invGamma; }
    const CovMatrix& Gamma() const { return m_Gamma; }
    
    double dot(const TVectorD& form, const TVectorD& v) const;
    TVectorD residual(const TVectorD& v) const;
        
   private:

    TVectorD m_g, m_gp, m_a, m_b;
    double m_G = 0.0, m_Gp = 0.0, m_Gpp = 0.0;
    CovMatrix m_invGamma, m_Gamma;
    TMatrixD m_resProj;
    std::vector<TVectorD> m_r;
    std::vector<unsigned int> m_rIdx;
  };
}

#endif

