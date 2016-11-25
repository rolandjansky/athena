/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZDCFitWrapper.h"

ZDCFitExpFermiVariableTaus::ZDCFitExpFermiVariableTaus(std::string tag, float tmin, float tmax, bool fixTau1, bool fixTau2, float tau1, float tau2) :
  ZDCFitWrapper(new TF1(("ExpFermiVariableTaus" + tag).c_str(), ZDCFermiExpFit, tmin, tmax, 5)),
  _fixTau1(fixTau1), _fixTau2(fixTau2), _tau1(tau1), _tau2(tau2)
{
  TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "#tau_{1}");
  theTF1->SetParName(3, "#tau_{2}");
  theTF1->SetParName(4, "s_{b}");

  theTF1->SetParLimits(0, 0, 1024);
  theTF1->SetParLimits(1, tmin, tmax);
  theTF1->SetParLimits(2, 2.5, 6);
  theTF1->SetParLimits(3, 10, 60);
  theTF1->SetParLimits(4, -0.05, 0.05);

  if (_fixTau1) theTF1->FixParameter(2, _tau1);
  if (_fixTau2) theTF1->FixParameter(3, _tau2);
}

void ZDCFitExpFermiVariableTaus::Initialize(float initialAmp, float initialT0)
{
  TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

  theTF1->SetParameter(0, initialAmp);
  theTF1->SetParameter(1, initialT0);

  if (!_fixTau1) theTF1->SetParameter(2, _tau1);
  if (!_fixTau2) theTF1->SetParameter(3, _tau2);

  theTF1->SetParameter(4, 0);
}

ZDCFitExpFermiFixedTaus::ZDCFitExpFermiFixedTaus(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCFitWrapper(new TF1(("ExpFermiFixedTaus" + tag).c_str(), this, tmin, tmax, 3)),
  _tau1(tau1), _tau2(tau2)
{
  TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

  theTF1->SetParLimits(0, 0, 1024);
  theTF1->SetParLimits(1, tmin, tmax);
  theTF1->SetParLimits(2, -0.05, 0.05);

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "s_{b}");

  // Now create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiFixedTausRefFunc" + tag;
  
  _expFermiFunc = new TF1(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 5);

  _expFermiFunc->SetParameter(0, 1);
  _expFermiFunc->SetParameter(1, 0);
  _expFermiFunc->SetParameter(2, _tau1);
  _expFermiFunc->SetParameter(3, _tau2);
  _expFermiFunc->SetParameter(4, 0);

  _norm = 1./_expFermiFunc->GetMaximum();
  _timeCorr = 0.5*_tau1*std::log(_tau2/_tau1 - 1.0);
}

void ZDCFitExpFermiFixedTaus::Initialize(float initialAmp, float initialT0)
{
  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);
  GetWrapperTF1()->SetParameter(2, 0);
}

ZDCFitExpFermiPrePulse::ZDCFitExpFermiPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCPrePulseFitWrapper(new TF1(("ExpFermiPrePulse" + tag).c_str(), this, tmin, tmax, 5)),
  _tau1(tau1), _tau2(tau2)
{
  // Create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiPerPulseRefFunc" + tag;
  
  _expFermiFunc = new TF1(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 5);

  _expFermiFunc->SetParameter(0, 1);
  _expFermiFunc->SetParameter(1, 0);
  _expFermiFunc->SetParameter(2, _tau1);
  _expFermiFunc->SetParameter(3, _tau2);
  _expFermiFunc->SetParameter(4, 0);

  _norm = 1./_expFermiFunc->GetMaximum();
  _timeCorr = 0.5*_tau1*std::log(_tau2/_tau1 - 1.0);

  // Now set up the actual TF1
  //
  TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

  theTF1->SetParLimits(0, 0, 1024);
  theTF1->SetParLimits(1, tmin, tmax);
  theTF1->SetParLimits(2, 0, 1024);
  theTF1->SetParLimits(3, -20, 10);
  theTF1->SetParLimits(4, -0.05, 0.05);

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "Amp_{pre}");
  theTF1->SetParName(3, "T0_{pre}");
  theTF1->SetParName(4, "s_{b}");
}

void ZDCFitExpFermiPrePulse::Initialize(float initialAmp, float initialT0)
{
  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);
  GetWrapperTF1()->SetParameter(2, 5);
  GetWrapperTF1()->SetParameter(3, 0);
  GetWrapperTF1()->SetParameter(4, 0);
}

ZDCFitExpFermiPulseSequence::ZDCFitExpFermiPulseSequence(std::string tag, float tmin, float tmax, float nominalT0, float deltaT,  float tau1, float tau2) :
  ZDCFitWrapper(0), _tau1(tau1), _tau2(tau2)
{
  // Create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiPulseSequenceRefFunc" + tag;
  
  _expFermiFunc = new TF1(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 5);

  _expFermiFunc->SetParameter(0, 1);
  _expFermiFunc->SetParameter(1, 0);
  _expFermiFunc->SetParameter(2, _tau1);
  _expFermiFunc->SetParameter(3, _tau2);
  _expFermiFunc->SetParameter(4, 0);

  _norm = 1./_expFermiFunc->GetMaximum();
  _timeCorr = 0.5*_tau1*std::log(_tau2/_tau1 - 1.0);

  // Calculate how many pulses we have to analyze
  //
  size_t numPrePulse = std::ceil((nominalT0 - tmin)/deltaT) + 1;
  for (size_t ipre = 0; ipre <  numPrePulse; ipre++) {
    _pulseDeltaT.push_back(((float) ipre) * -deltaT);
  }

  size_t numPostPulse = std::floor((tmax - nominalT0)/deltaT);
  for (size_t ipost = 0; ipost <  numPrePulse; ipost++) {
    _pulseDeltaT.push_back(((float) ipost) * deltaT);
  }

  _numPulses = 1 + numPrePulse + numPostPulse;

  // Now set up the actual TF1
  //
  
  _fitFunc = new TF1(("ExpFermiPulseSequence" + tag).c_str(), this, tmin, tmax, _numPulses + 1),

  _fitFunc->SetParName(0, "Amp");
  _fitFunc->SetParName(1, "T0");

  _fitFunc->SetParLimits(0, 0, 1024);
  _fitFunc->SetParLimits(1, nominalT0 - deltaT/2, nominalT0 + deltaT/2);

  for (size_t ipulse = 0; ipulse < _numPulses - 1; ipulse++) {
    std::string name = "Amp" + std::to_string(ipulse + 1);
    _fitFunc->SetParName(ipulse + 2, name.c_str());
    _fitFunc->SetParLimits(ipulse + 2, 0, 1024);
  }
}

void ZDCFitExpFermiPulseSequence::Initialize(float initialAmp, float initialT0)
{
  _fitFunc->SetParameter(0, initialAmp);
  _fitFunc->SetParameter(1, initialT0);

  for (size_t ipulse = 0; ipulse < _numPulses - 1; ipulse++) {
    _fitFunc->SetParameter(ipulse + 2, 0);
  }
}



double ZDCFermiExpFit(double* xvec, double* pvec)
{
  static const float offsetScale = 0;

  double t = xvec[0];

  double amp = pvec[0];
  double t0 = pvec[1];
  double tau1 = pvec[2];
  double tau2 = pvec[3];
  double linSlope = pvec[4];

  double tauRatio = tau2/tau1;
  double tauRatioMinunsOne = tauRatio - 1;

  double norm = ( std::exp(-offsetScale/tauRatio)*pow(1./tauRatioMinunsOne, 1./(1 + tauRatio))/
		  ( 1 + pow(1./tauRatioMinunsOne, 1./(1 + 1/tauRatio)))                         );

  double deltaT = t - (t0 - offsetScale*tau1);
  if (deltaT < 0) deltaT = 0;

  double expTerm = std::exp(-deltaT/tau2);
  double fermiTerm = 1./(1. + std::exp(-(t - t0)/tau1));

  return amp*expTerm*fermiTerm/norm + linSlope*t;
}
