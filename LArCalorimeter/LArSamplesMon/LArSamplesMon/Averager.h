/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::Averager
   @brief storage of the time histories of all the cells
 */

#ifndef LArSamples_Averager_H
#define LArSamples_Averager_H

#include "LArSamplesMon/IndexRange.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TMath.h"

#include "LArCafJobs/Definitions.h"

namespace LArSamples {
  
  class Averager : public IndexRange
  {
  
    public:
      
      /** @brief Constructor  */
      Averager(unsigned int n = 0);
      Averager(unsigned int lwb, unsigned int upb);

      virtual ~Averager() { }

      bool add(double xMin = -DBL_MAX, double xMax = DBL_MAX);
      bool fill(const TVectorD& values, double w = 1);
      bool append(const Averager& other);

      double mean(unsigned int i) const;
      double meanError(unsigned int i) const;
      
      double rms(unsigned int i) const;
      double covariance(unsigned int i, unsigned j) const;
      double covarianceError(unsigned int i, unsigned j) const;

      TVectorD means() const;
      TVectorD meanErrors() const;

      CovMatrix meanErrorMatrix() const;
      
      CovMatrix covarianceMatrix() const;
      CovMatrix covarianceMatrixErrors() const;

      unsigned int nVariables() const { return m_xMin.size(); }
      int lwb() const { return m_lwb; }
      int upb() const { return m_lwb + nVariables() - 1; }

      unsigned int nEntries() const { return m_n; }
      double totalWeight() const { return m_wTot; }

      const TVectorD& sum1() const { return m_sum1; }
      const TVectorD& sum2() const { return m_sum2; }
      const TMatrixD& sum11Matrix() const { return m_sum11Matrix; }
      const TMatrixD& sum21Matrix() const { return m_sum21Matrix; }
      const TMatrixD& sum22Matrix() const { return m_sum22Matrix; }

      Averager& operator=(const Averager& other);
      static double safeSqrt(double x); 

    private:

      bool prepare();

      unsigned int m_n;
      double m_wTot;
      int m_lwb;
      
      std::vector<double> m_xMin, m_xMax;
      TVectorD m_sum1, m_sum2;
      TMatrixD m_sum11Matrix, m_sum21Matrix, m_sum22Matrix;
  };
}
  
#endif
