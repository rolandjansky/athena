/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
   @class LArAbsShape
   @brief Liquid Argon base class for shape information
*/

#ifndef LArSamples_AbsShape_H
#define LArSamples_AbsShape_H

#include "TArrayI.h"
#include "TVectorD.h"
#include "TMatrixD.h"

#include "LArCafJobs/Definitions.h"

class TH1D;
class TGraphErrors;

namespace LArSamples {

  class SimpleShape;
  
  class AbsShape  {
  
    public:
      
      virtual ~AbsShape() { }

      /** @return data points */
      virtual unsigned int nPoints() const = 0;
      virtual double value(unsigned int i) const = 0;
      virtual double covariance(unsigned int i, unsigned int j) const = 0;
      virtual double time(unsigned int i) const = 0;
      
      virtual double error(unsigned int i) const;
      TVectorD values(int lwb, int upb) const;

      int findTimeInterval(double time) const;
      int interpolate(double time, double& value, double& error) const;
      int interpolateDiff(double time, double& diff) const;
      bool interpolate(const AbsShape& other, TVectorD& values, CovMatrix& errors, int lwb = -1, int upb = -1) const;
      bool interpolateDiff(const AbsShape& other, TVectorD& diffs, int lwb = -1, int upb = -1) const;

      // From AbsShape
      CovMatrix covarianceMatrix(int lwb = -1, int upb = -1,
                                 const CovMatrix& refErr = CovMatrix(),
                                 bool withCorrs = true) const;
    
      CovMatrix invCovarianceMatrix(int lwb = -1, int upb = -1,
                                    const CovMatrix& refErr = CovMatrix(),
                                    bool withCorrs = true) const;
    
      CovMatrix covarianceMatrix(unsigned int nPoints, bool withCorrs = true) const;
      CovMatrix invCovarianceMatrix(unsigned int nPoints, bool withCorrs = true) const;
      
      /** @return sample max parameters */
      double maxValue(bool withErrors = false) const;
      double minValue(bool withErrors = false) const;
      int maxPosition() const;
      int minPosition() const;

      TGraphErrors* graph(bool timeInUnitOfSamples = false) const;
      SimpleShape* resample(unsigned int nPts) const;
      
    protected:
      
      AbsShape() { }
  };
}
#endif

