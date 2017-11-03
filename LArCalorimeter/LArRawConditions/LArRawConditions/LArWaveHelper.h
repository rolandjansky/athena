/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARWAVEHELPER_H
#define LARWAVEHELPER_H

#include "LArWave.h"
#include "LArWaveCumul.h"
#include "LArWaveDerivedQuantitiesP.h"
#include <math.h>


class LArWaveHelper {

 public:

  LArWave      translate  (const LArWave& theWave, int nShift, double baseline = 0.) const;
  LArWave      Dtranslate  (const LArWave& theWave, double tShift, double baseline = 0.) const;
    /// return index of minimum sample
  unsigned int getMin     (const LArWave& theWave) const;     
    /// return index of maximum sample
  unsigned int getMax     (const LArWave& theWave) const;     
   ///  return amplitude aproximation from poly2 fit around maxima, and it's time
  double       getDMax    (const LArWave& theWave, double &tMax)const;
    /// crude derivative
  LArWave      derive     (const LArWave& theWave) const;          
    /// smoothed derivative
  LArWave      derive_smooth(const LArWave& theWave) const;
    /// autocorrelation function (not normalized)
  LArWave      autocorr   (const LArWave& theWave) const; 	

  LArWave  subSample(const LArWave& theWave,unsigned Nfirst,unsigned deltaN) const;
  double   getBaseline(const LArWave& theWave,unsigned nBase) const;
  double   getSumTail(const LArWave& theWave,unsigned iFirst) const;
  double   getSumRegion(const LArWave& theWave,unsigned iFirst,unsigned iLast) const;
  double   getSumSquareTail(const LArWave& theWave,unsigned iFirst) const;
  double   getSumSquareRegion(const LArWave& theWave,unsigned iFirst,unsigned iLast) const;
  double   getArea(const LArWave& theWave)const;
  double   getWidth(const LArWave& theWave)const;
  double   getMaxAmp(const LArWave& theWave)const;
  double   getT0(const LArWave& theWave)const;
  double   getT5(const LArWave& theWave)const;
  unsigned getStart(const LArWave& theWave)const;
  unsigned getZeroCross(const LArWave& theWave)const;
  double   getJitter(const LArWaveCumul& theWave)const;

  LArWaveDerivedQuantitiesP getDerivedQuantities(const LArWaveCumul& theWave)const;

  std::vector<double> linfit(const LArWave& theWave,unsigned iFirst,unsigned iLast,
	                double& rho) const;
  std::vector<double> expfit(const LArWave& theWave,unsigned iFirst,unsigned iLast,
			double& rho) const;
  std::vector<double> polyfit(const LArWave& theWave,unsigned iFirst,unsigned iLast,
                         unsigned Ndeg) const;

  // the following returns a linear master waveform and a "dac0" intercept
  // [0] is dac0, [1] is master waveform
  std::vector<LArWave> linearMasterWave(const std::vector<const LArWave*>& vWaves,const std::vector<double> vAmpli) const;

 private:

  unsigned fact(unsigned N) const { return ( N<2 ) ? 1 : N*fact(N-1) ; };

  std::vector<double> linfit(const std::vector<double>& X,const std::vector<double>& Y,
			double& rho) const;
  std::vector<double> expfit(const std::vector<double>& X,const std::vector<double>& Y,
			double& rho) const;
  std::vector<double> polyfit(const std::vector<double>& X,const std::vector<double>& Y,
			 unsigned Ndeg) const;
  unsigned get_fit_vectors(const LArWave theWave,unsigned iFirst,unsigned iLast,
		  std::vector<double>& X,std::vector<double>& Y) const;

} ;


#endif // LARWAVEHELPER_H
