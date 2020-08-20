/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZDCFitWrapper.h"

void ZDCFitWrapper::Initialize(float initialAmp, float initialT0, float ampMin, float ampMax)
{
  // If we adjusted the time range on the previous event, restore to default
  //
  if (m_adjTLimitsEvent) {
    SetT0FitLimits(m_t0Min, m_t0Max);

    m_adjTLimitsEvent = false;
    m_tminAdjust = 0;
  }

  SetAmpMinMax(ampMin, ampMax);

  DoInitialize(initialAmp, initialT0, ampMin, ampMax);
}

void ZDCFitWrapper::Initialize(float initialAmp, float initialT0, float ampMin, float ampMax, float fitTmin, float fitTmax, float fitTRef)
{
  m_adjTLimitsEvent = true;

  m_tminAdjust = fitTRef;

  float newT0Min = std::max(m_t0Min, fitTmin);
  float newT0Max = std::min(m_t0Max, fitTmax);

  SetAmpMinMax(ampMin, ampMax);
  SetT0FitLimits(newT0Min, newT0Max);

  DoInitialize(initialAmp, initialT0, ampMin, ampMax);
}

ZDCFitExpFermiVariableTaus::ZDCFitExpFermiVariableTaus(std::string tag, float tmin, float tmax, bool fixTau1, bool fixTau2, float tau1, float tau2) :
  ZDCFitWrapper(std::make_shared<TF1>(("ExpFermiVariableTaus" + tag).c_str(), ZDCFermiExpFit, tmin, tmax, 5 - 1)),
  m_fixTau1(fixTau1), m_fixTau2(fixTau2), m_tau1(tau1), m_tau2(tau2)
{
  std::shared_ptr<TF1> theTF1 = ZDCFitWrapper::GetWrapperTF1();

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "#tau_{1}");
  theTF1->SetParName(3, "#tau_{2}");

  // BAC, parameter 0 limits now is set in DoInitialize
  theTF1->SetParLimits(1, tmin, tmax);
  theTF1->SetParLimits(2, 2.5, 6);
  theTF1->SetParLimits(3, 10, 60);
  //theTF1->SetParLimits(4, -0.05, 0.05);

  if (m_fixTau1) theTF1->FixParameter(2, m_tau1);
  if (m_fixTau2) theTF1->FixParameter(3, m_tau2);
}

void ZDCFitExpFermiVariableTaus::DoInitialize(float initialAmp, float initialT0, float ampMin, float ampMax)
{
  std::shared_ptr<TF1> theTF1 = ZDCFitWrapper::GetWrapperTF1();

  theTF1->SetParameter(0, initialAmp);
  theTF1->SetParameter(1, initialT0);

  theTF1->SetParLimits(0, ampMin, ampMax);

  if (!m_fixTau1) theTF1->SetParameter(2, m_tau1);
  if (!m_fixTau2) theTF1->SetParameter(3, m_tau2);
}

void ZDCFitExpFermiVariableTaus::SetT0FitLimits(float t0Min, float t0Max)
{
  // Set the parameter limits accordingly on the TF1
  //
  std::shared_ptr<TF1> theTF1 = ZDCFitWrapper::GetWrapperTF1();
  theTF1->SetParLimits(1, t0Min, t0Max);
}

ZDCFitExpFermiFixedTaus::ZDCFitExpFermiFixedTaus(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCFitWrapper(std::make_shared<TF1>(("ExpFermiFixedTaus" + tag).c_str(), this, tmin, tmax, 3 - 1)),
  m_tau1(tau1), m_tau2(tau2)
{
  std::shared_ptr<TF1> theTF1 = ZDCFitWrapper::GetWrapperTF1();

  // BAC, parameter 0 limits now is set in DoInitialize
  theTF1->SetParLimits(1, tmin, tmax);

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");

  // Now create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiFixedTausRefFunc" + tag;
  m_expFermiFunc.reset(new TF1(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 5));

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);
  m_expFermiFunc->SetParameter(4, 0);

  m_norm = 1. / m_expFermiFunc->GetMaximum();
  m_timeCorr = m_tau1 * std::log(m_tau2 / m_tau1 - 1.0);
}

void ZDCFitExpFermiFixedTaus::DoInitialize(float initialAmp, float initialT0, float ampMin, float ampMax)
{
  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);

  GetWrapperTF1()->SetParLimits(0, ampMin, ampMax);
}

void ZDCFitExpFermiFixedTaus::SetT0FitLimits(float t0Min, float t0Max)
{
  GetWrapperTF1()->SetParLimits(1, t0Min, t0Max);
}

ZDCFitExpFermiPrePulse::ZDCFitExpFermiPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCPrePulseFitWrapper(std::make_shared<TF1>(("ExpFermiPrePulse" + tag).c_str(), this, tmin, tmax, 5 - 1)),
  m_tau1(tau1), m_tau2(tau2)
{
  // Create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiPerPulseRefFunc" + tag;

  m_expFermiFunc.reset(new TF1(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 4));

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);

  m_norm = 1. / m_expFermiFunc->GetMaximum();
  m_timeCorr = m_tau1 * std::log(m_tau2 / m_tau1 - 1.0);

  // Now set up the actual TF1
  //
   std::shared_ptr<TF1> theTF1 = ZDCFitWrapper::GetWrapperTF1();

  // BAC, parameter 0 limits now is set in DoInitialize
  theTF1->SetParLimits(1, tmin, tmax);
  theTF1->SetParLimits(2, 1, 4096); // Increase the upper range to 4 times of ADC range to deal with large exponential tail case of pre-pulse.
  theTF1->SetParLimits(3, -20, 10);

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "Amp_{pre}");
  theTF1->SetParName(3, "T0_{pre}");
}

void ZDCFitExpFermiPrePulse::SetPrePulseT0Range(float tmin, float tmax)
{
  if (tmin > GetTMin()) {
    GetWrapperTF1()->ReleaseParameter(3);
    GetWrapperTF1()->SetParLimits(3, tmin, tmax);
  }
  else {
    GetWrapperTF1()->FixParameter(3, tmin * 1.01);
  }
}

void ZDCFitExpFermiPrePulse::DoInitialize(float initialAmp, float initialT0, float ampMin, float ampMax)
{
  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);
  GetWrapperTF1()->SetParameter(2, 5);
  GetWrapperTF1()->SetParameter(3, 0);

  GetWrapperTF1()->SetParLimits(0, ampMin, ampMax);
}

void ZDCFitExpFermiPrePulse::SetT0FitLimits(float t0Min, float t0Max)
{
   std::shared_ptr<TF1> theTF1 = GetWrapperTF1();
  theTF1->SetParLimits(1, t0Min, t0Max);
}

double ZDCFermiExpFit(double* xvec, double* pvec)
{
  static const float offsetScale = 0;

  double t = xvec[0];

  double amp = pvec[0];
  double t0 = pvec[1];
  double tau1 = pvec[2];
  double tau2 = pvec[3];
  //double linSlope = pvec[4];

  double tauRatio = tau2 / tau1;
  double tauRatioMinunsOne = tauRatio - 1;

  double norm = ( std::exp(-offsetScale / tauRatio) * pow(1. / tauRatioMinunsOne, 1. / (1 + tauRatio)) /
                  ( 1 + pow(1. / tauRatioMinunsOne, 1. / (1 + 1 / tauRatio)))                         );

  double deltaT = t - (t0 - offsetScale * tau1);
  if (deltaT < 0) deltaT = 0;

  double expTerm = std::exp(-deltaT / tau2);
  double fermiTerm = 1. / (1. + std::exp(-(t - t0) / tau1));

  return amp * expTerm * fermiTerm / norm; // + linSlope*t;
}

// --------------------------------------------------------------------------------------------------------------------------------------------
//
ZDCFitExpFermiLinearFixedTaus::ZDCFitExpFermiLinearFixedTaus(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCFitWrapper(std::make_shared<TF1>(("ExpFermiFixedTaus" + tag).c_str(), this, tmin, tmax, 4)),
  m_tau1(tau1), m_tau2(tau2)
{
  std::shared_ptr<TF1> theTF1 = GetWrapperTF1();

  // BAC, parameter 0 limits now is set in DoInitialize
  theTF1->SetParLimits(1, tmin, tmax);

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "s_{b}");
  theTF1->SetParName(3, "c_{b}");

  // Now create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiFixedTausRefFunc" + tag;

  m_expFermiFunc = std::make_shared<TF1>(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 4);

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);

  m_norm = 1. / m_expFermiFunc->GetMaximum();
  m_timeCorr = m_tau1 * std::log(m_tau2 / m_tau1 - 1.0);
}

void ZDCFitExpFermiLinearFixedTaus::DoInitialize(float initialAmp, float initialT0, float ampMin, float ampMax)
{
  float slope     = std::abs(0.1 * initialAmp / initialT0);
  float intercept = std::abs(0.1 * initialAmp);
  GetWrapperTF1()->SetParLimits(2, -slope    , slope    );
  GetWrapperTF1()->SetParLimits(3, -intercept, intercept);

  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);
  GetWrapperTF1()->SetParameter(2, 0);
  GetWrapperTF1()->SetParameter(3, 0);

  GetWrapperTF1()->SetParLimits(0, ampMin, ampMax);
}

void ZDCFitExpFermiLinearFixedTaus::SetT0FitLimits(float t0Min, float t0Max)
{
  std::shared_ptr<TF1> theTF1 = GetWrapperTF1();
  theTF1->SetParLimits(1, t0Min, t0Max);
}

// --------------------------------------------------------------------------------------------------------------------------------------------
//
ZDCFitExpFermiLinearPrePulse::ZDCFitExpFermiLinearPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCPrePulseFitWrapper(std::make_shared<TF1>(("ExpFermiPrePulse" + tag).c_str(), this, tmin, tmax, 6)),
  m_tau1(tau1), m_tau2(tau2)
{
  // Create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiPerPulseRefFunc" + tag;

  m_expFermiFunc = std::make_shared<TF1>(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 4);

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);

  m_norm = 1. / m_expFermiFunc->GetMaximum();
  m_timeCorr = m_tau1 * std::log(m_tau2 / m_tau1 - 1.0);

  // Now set up the actual TF1
  //
  std::shared_ptr<TF1> theTF1 = ZDCFitWrapper::GetWrapperTF1();

  // BAC, parameter 0 limits now is set in DoInitialize
  theTF1->SetParLimits(1, tmin, tmax);
  theTF1->SetParLimits(2, 1, 4096); // Increase the upper range to 4 times of ADC range to deal with large exponential tail case of pre-pulse.
  theTF1->SetParLimits(3, -20, 10);

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "Amp_{pre}");
  theTF1->SetParName(3, "T0_{pre}");
  theTF1->SetParName(4, "s_{b}");
  theTF1->SetParName(5, "c_{b}");
}

void ZDCFitExpFermiLinearPrePulse::SetPrePulseT0Range(float tmin, float tmax)
{
  if (tmin > GetTMin()) {
    GetWrapperTF1()->ReleaseParameter(3);
    GetWrapperTF1()->SetParLimits(3, tmin, tmax);
  }
  else {
    GetWrapperTF1()->FixParameter(3, tmin * 1.01);
  }
}

void ZDCFitExpFermiLinearPrePulse::DoInitialize(float initialAmp, float initialT0, float ampMin, float ampMax)
{
  float slope     = std::abs(initialAmp / initialT0);  // to be studied more ??? limit 0.1 0.05
  float intercept = std::abs(0.5 * initialAmp);
  GetWrapperTF1()->SetParLimits(4, -slope    , slope    );
  GetWrapperTF1()->SetParLimits(5, -intercept, intercept);

  GetWrapperTF1()->SetParLimits(0, ampMin, ampMax);

  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);
  GetWrapperTF1()->SetParameter(2, 5);
  GetWrapperTF1()->SetParameter(3, 0);
  GetWrapperTF1()->SetParameter(4, 0);
  GetWrapperTF1()->SetParameter(5, 0);
}

void ZDCFitExpFermiLinearPrePulse::SetT0FitLimits(float t0Min, float t0Max)
{
  GetWrapperTF1()->SetParLimits(1, t0Min, t0Max);
}


// --------------------------------------------------------------------------------------------------------------------------------------------
//
ZDCFitComplexPrePulse::ZDCFitComplexPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCPrePulseFitWrapper(std::make_shared<TF1>(("ExpFermiPrePulse" + tag).c_str(), this, tmin, tmax, 7)),
  m_tau1(tau1), m_tau2(tau2)
{
  // Create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiPerPulseRefFunc" + tag;

  m_expFermiFunc = std::make_shared<TF1>(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 4);

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);

  m_norm     = 1. / m_expFermiFunc->GetMaximum();
  m_timeCorr = m_tau1 * std::log(m_tau2 / m_tau1 - 1.0);

  // Now set up the actual TF1
  //
  std::shared_ptr<TF1> theTF1 = GetWrapperTF1();

  // BAC, parameter 0 limits now is set in DoInitialize
  theTF1->SetParLimits(1, tmin, tmax);
  theTF1->SetParLimits(2,    0, 2048);  // Pre-pulse upper bound should not be greater 2 times of ADC range with overflow constrains.
  theTF1->SetParLimits(3,    0,   40);
  theTF1->SetParLimits(6,    0, 4096);  // Increase the upper range to 4 times of ADC range to deal with large exponential tail case of pre-pulse.

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "Amp_{pre}");
  theTF1->SetParName(3, "T0_{pre}");
  theTF1->SetParName(4, "s_{b}");
  theTF1->SetParName(5, "c_{b}");
  theTF1->SetParName(6, "Amp_{exp}");
}

void ZDCFitComplexPrePulse::SetPrePulseT0Range(float tmin, float tmax)
{
  if (tmin > GetTMin()) {
    GetWrapperTF1()->ReleaseParameter(3);
    GetWrapperTF1()->SetParLimits(3, tmin, tmax);
  }
  else {
    GetWrapperTF1()->SetParLimits(3, 0, tmax);
  }
}

void ZDCFitComplexPrePulse::DoInitialize(float initialAmp, float initialT0, float ampMin, float ampMax)
{
  float slope     = std::abs(initialAmp / initialT0);  // to be studied more ??? limit 0.1 0.05
  float intercept = std::abs(0.1 * initialAmp);       // reduce from 0.25 to 0.1 fix some fail issue
  GetWrapperTF1()->SetParLimits(4, -slope    , slope    );  // if the lower limit is set to 0, there will be some fit fail issue...
  GetWrapperTF1()->SetParLimits(5, -intercept, intercept);

  GetWrapperTF1()->SetParLimits(0, ampMin, ampMax);

  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);
  GetWrapperTF1()->SetParameter(2,  5);
  GetWrapperTF1()->SetParameter(3, 10);
  GetWrapperTF1()->SetParameter(4,  0);
  GetWrapperTF1()->SetParameter(5,  0);
  GetWrapperTF1()->SetParameter(6,  1);
}

void ZDCFitComplexPrePulse::SetT0FitLimits(float t0Min, float t0Max)
{
  GetWrapperTF1()->SetParLimits(1, t0Min, t0Max);
}



// --------------------------------------------------------------------------------------------------------------------------------------------
//
ZDCFitGeneralPulse::ZDCFitGeneralPulse(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCPrePulseFitWrapper(std::make_shared<TF1>(("ExpFermiPrePulse" + tag).c_str(), this, tmin, tmax, 9)),
  m_tau1(tau1), m_tau2(tau2)
{
  // Create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiPerPulseRefFunc" + tag;

  m_expFermiFunc = std::make_shared<TF1>(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 4);

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);

  m_norm     = 1. / m_expFermiFunc->GetMaximum();
  m_timeCorr = m_tau1 * std::log(m_tau2 / m_tau1 - 1.0);

  // Now set up the actual TF1
  //
  std::shared_ptr<TF1> theTF1 = GetWrapperTF1();

  // BAC, parameter 0 limits now is set in DoInitialize
  theTF1->SetParLimits(1, tmin, tmax);
  theTF1->SetParLimits(2,    0, 2048);  // Pre-pulse upper bound should not be greater 2 times of ADC range with overflow constrains.
  theTF1->SetParLimits(3,    0,   40);
  theTF1->SetParLimits(6,    0, 4096);  // Increase the upper range to 4 times of ADC range to deal with large exponential tail case of pre-pulse.
  theTF1->SetParLimits(7,    0, 2048);  // Post-pulse upper bound should not be greater 2 times of ADC range with overflow constrains.
  theTF1->SetParLimits(8,  100,  163);

  theTF1->SetParName(0, "Amp");
  theTF1->SetParName(1, "T0");
  theTF1->SetParName(2, "Amp_{pre}");
  theTF1->SetParName(3, "T0_{pre}");
  theTF1->SetParName(4, "s_{b}");
  theTF1->SetParName(5, "c_{b}");
  theTF1->SetParName(6, "Amp_{exp}");
  theTF1->SetParName(7, "Amp_{post}");
  theTF1->SetParName(8, "T0_{post}");
}

void ZDCFitGeneralPulse::SetPrePulseT0Range(float tmin, float tmax)
{
  if (tmin > GetTMin()) {
    if (tmin < 0) tmin = 0;
    GetWrapperTF1()->SetParLimits(3, tmin, tmax);
  }
  else {
    GetWrapperTF1()->SetParLimits(3, 0, tmax);
  }
}

void ZDCFitGeneralPulse::SetPostPulseT0Range(float tmin, float tmax, float initialPostT0)
{
  GetWrapperTF1()->SetParLimits(8, tmin, tmax);
  float iniPostT0 = initialPostT0;
  GetWrapperTF1()->SetParameter(8, iniPostT0);
}

void ZDCFitGeneralPulse::DoInitialize(float initialAmp, float initialT0, float ampMin, float ampMax)
{
  float slope     = std::abs(initialAmp / initialT0);  // to be studied more ??? limit 0.1 0.05
  float intercept = std::abs(0.1 * initialAmp);       // reduce from 0.25 to 0.1 fix some fail issue
  GetWrapperTF1()->SetParLimits(4, -slope    , slope    );  // if the lower limit is set to 0, there will be some fit fail issue...
  GetWrapperTF1()->SetParLimits(5, -intercept, intercept);

  GetWrapperTF1()->SetParLimits(0, ampMin, ampMax);

  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);
  GetWrapperTF1()->SetParameter(4,   0);
  GetWrapperTF1()->SetParameter(5,   0);
  GetWrapperTF1()->SetParameter(7,   5);
}

void ZDCFitGeneralPulse::SetT0FitLimits(float t0Min, float t0Max)
{
  GetWrapperTF1()->SetParLimits(1, t0Min, t0Max);
}
