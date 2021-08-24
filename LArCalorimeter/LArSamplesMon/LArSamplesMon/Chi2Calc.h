/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamplesHistory
   @brief Class to calculate chi^2s
*/

#ifndef LArSamples_Chi2Calc_H
#define LArSamples_Chi2Calc_H

#include "LArSamplesMon/IndexRange.h"
#include "TArrayI.h"
#include "TVectorD.h"
#include "LArCafJobs/AbsShape.h"
#include "LArCafJobs/Definitions.h"

namespace LArSamples {
  
  class OFC;
  class ScaledErrorData;
  
  enum Chi2Params { DefaultChi2 = 0, NoCorrs = 1, BasicChi2 = 2, OFCChi2 = 4 };
  
  class Chi2Calc : public IndexRange {
  
   public:
   
    /** @brief Constructor (takes ownership of LArCellInfo object) */
    Chi2Calc(int pars = 0) 
      : m_pars(pars), m_lwb(-1), m_upb(-1) { }

    virtual ~Chi2Calc() { }

    TVectorD deltas(const AbsShape& data, const AbsShape& reference, CovMatrix& errors, const ScaledErrorData* shapeError = 0, 
                    int lwb = -1, int upb = -1, bool noDataError = false) const;

    double chi2(const AbsShape& data, const AbsShape& reference, const ScaledErrorData* shapeError = 0, 
                int lwb = -1, int upb = -1) const;

    // From AbsShape
    double scalarProduct(const TVectorD& values1, const TVectorD& values2, const CovMatrix& invCovMat) const;
        
    int lwb() const { return m_lwb; }
    int upb() const { return m_upb; }
    unsigned int nDof() const { return upb() - lwb() + 1; }  
    
    bool bestRescale(const AbsShape& data, const AbsShape& reference, double& k, double& chi2, double deltaT = 0, 
                     const ScaledErrorData* sed = 0, unsigned int minNDof = 0) const;
    
  private:
    
    bool useCorrs() const { return !(m_pars & NoCorrs); }
    
    int m_pars;
    
    mutable int m_lwb, m_upb;
  };
}
#endif
