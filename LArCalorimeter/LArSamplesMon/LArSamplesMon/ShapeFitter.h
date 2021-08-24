/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamplesHistory
   @brief Class to fit shapes
 */


#ifndef LArSamples_ShapeFitter_H
#define LArSamples_ShapeFitter_H

#include "LArCafJobs/AbsShape.h"
#include "LArSamplesMon/Chi2Calc.h"

namespace LArSamples {
  
  class History;
  class Data;
  class ScaledErrorData;
  
  class ShapeFitter  {
  
    public:
   
      ShapeFitter(Chi2Params chi2Params = DefaultChi2) : m_c2c(chi2Params) { }
      
      bool fit(const LArSamples::AbsShape& data, const AbsShape& reference, 
               double& k, double& deltaT, double& chi2,
               const ScaledErrorData* sed = 0) const;      

      static void adjusted_reference(Int_t& nPar, Double_t* grad, Double_t& f, Double_t* par, Int_t iflag);
      
      const Chi2Calc& chi2Calc() const { return m_c2c; }
      
    private:
      
      Chi2Calc m_c2c;      
  };
}
#endif
