/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::DigitMonitor
   @brief storage of the time histories of all the cells
 */

#ifndef LArSamples_DigitMonitor_H
#define LArSamples_DigitMonitor_H

#include "TArrayI.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMatrixD.h"
#include "TString.h"

#include "LArSamplesMon/MonitorBase.h"
#include "LArSamplesMon/Chi2Calc.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/CaloId.h"
#include "CaloIdentifier/CaloGain.h"

class TF1;

namespace LArSamples {
  
  class SimpleShape;
  class Residuals;
  
  class DigitMonitor : public MonitorBase  
  {
  
    public:
      
      /** @brief Constructor  */
      DigitMonitor(const Interface& interface) : MonitorBase(interface) { } 
    
      TH1D* chi2Dist(const TString& name, int nBins, double max, double kFactor = 0, double fitMax = -1, 
                     int lwb = -1, int upb = -1, unsigned int chi2Pars = DefaultChi2, ShapeErrorType shapeErrorType = BestShapeError,
                     unsigned int nDof = 0) const;

      static TF1* chi2Func(const char* name, double xMin, double xMax);
      static TF1* fitChi2(TH1D& h, const char* name, double xMin, double xMax, double nDof = -1, 
			  double fitMin = Definitions::none, double fitMax = Definitions::none);

      TH1D* bestChi2Dist(const TString& name, int nBins, double max, 
                         double refErrMin, double refErrMax, unsigned int refErrNBins,
                         int lwb = -1, int upb = -1, unsigned int chi2Pars = DefaultChi2,
                         unsigned int nDof = 0) const;
        
      TH1D* gainDist(const TString& name) const;
      TH1D* layerDist(const TString& name) const;
      
      TH1D* energyDist(const TString& name, int nBins, double eMax = 10000) const;
      TH1D* timeDist(const TString& name, int nBins, double tMin = -25, double tMax = 25) const;
      TH1D* adcMaxDist(const TString& name, int nBins, double aMin = 0, double aMax = 4096) const;
      TH1D* noiseDist(const TString& name, int nBins, double max) const;
    
      TH2D* maxValueMap(TString name, PartitionId partition) const;
      TH2D* minValueMap(TString name, PartitionId partition) const;
      
      TH1D* residualDist(unsigned int k, const TString& name, int nBins, double rMin, double rMax, bool norm = false) const;
      
      bool residualPlots(CaloId calo, unsigned int layer, CaloGain::CaloGain gain = CaloGain::LARHIGHGAIN, bool xip = false, bool ring = false, 
                         double lo = -0.05, double hi = 0.05, const TString& fileName = "residuals");
      bool residualPlotsRingComp(CaloId calo, unsigned int layer, CaloGain::CaloGain gain = CaloGain::LARHIGHGAIN, bool xip = false,
                         double lo = -0.05, double hi = 0.05, const TString& fileName = "residuals");
      bool residualPlotsGainComp(CaloId calo, unsigned int layer, bool ring = false, bool xip = false,
                         double lo = -0.05, double hi = 0.05, const TString& fileName = "residuals");

      TH1D* shapeErrorDist(unsigned int k, const TString& name, int nBins, double rMin, double rMax, double mean = 0) const;
      double residualCorr(unsigned int k1, unsigned int k2) const;
      
      bool makeResidualCorrections(const TString& outputFile, short resTrunc = -1, short timeTrunc = -1, 
                                   double absResTrunc = -1, unsigned int minSize = 0, 
                                   bool weigh = false, bool adjust = false, bool zeroTime = false) const;
      Residuals* getResiduals(unsigned int hash, CaloGain::CaloGain gain, double absResTrunc = -1, bool adjust = false, bool zeroTime = false) const;
     
      bool residualParams(int lwb, int upb, CovMatrix& k, TVectorD& means) const;
      CovMatrix kMatrix(int lwb, int upb) { CovMatrix k; TVectorD means; residualParams(lwb, upb, k, means); return k; }
      TVectorD means(int lwb, int upb) { CovMatrix k; TVectorD means; residualParams(lwb, upb, k, means); return means; }
                  
      int combine(SimpleShape*& shape, SimpleShape*& ref, const TString& selection = "", bool timeAligned = true) const;
  };
}
  
#endif
