/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZDCFitWrapper.h"

ZDCFitExpFermiVariableTaus::ZDCFitExpFermiVariableTaus(std::string tag, float tmin, float tmax, bool fixTau1, bool fixTau2, float tau1, float tau2) :
  ZDCFitWrapper(new TF1(("ExpFermiVariableTaus" + tag).c_str(), ZDCFermiExpFit, tmin, tmax, 5)),
  m_fixTau1(fixTau1), m_fixTau2(fixTau2), m_tau1(tau1), m_tau2(tau2)
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

  if (m_fixTau1) theTF1->FixParameter(2, m_tau1);
  if (m_fixTau2) theTF1->FixParameter(3, m_tau2);
}

void ZDCFitExpFermiVariableTaus::Initialize(float initialAmp, float initialT0)
{
  TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

  theTF1->SetParameter(0, initialAmp);
  theTF1->SetParameter(1, initialT0);

  if (!m_fixTau1) theTF1->SetParameter(2, m_tau1);
  if (!m_fixTau2) theTF1->SetParameter(3, m_tau2);

  theTF1->SetParameter(4, 0);
}

ZDCFitExpFermiFixedTaus::ZDCFitExpFermiFixedTaus(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCFitWrapper(new TF1(("ExpFermiFixedTaus" + tag).c_str(), this, tmin, tmax, 3)),
  m_tau1(tau1), m_tau2(tau2)
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
  
  m_expFermiFunc = new TF1(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 5);

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);
  m_expFermiFunc->SetParameter(4, 0);

  m_norm = 1./m_expFermiFunc->GetMaximum();
  m_timeCorr = 0.5*m_tau1*std::log(m_tau2/m_tau1 - 1.0);
}

void ZDCFitExpFermiFixedTaus::Initialize(float initialAmp, float initialT0)
{
  GetWrapperTF1()->SetParameter(0, initialAmp);
  GetWrapperTF1()->SetParameter(1, initialT0);
  GetWrapperTF1()->SetParameter(2, 0);
}

ZDCFitExpFermiPrePulse::ZDCFitExpFermiPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2) :
  ZDCPrePulseFitWrapper(new TF1(("ExpFermiPrePulse" + tag).c_str(), this, tmin, tmax, 5)),
  m_tau1(tau1), m_tau2(tau2)
{
  // Create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiPerPulseRefFunc" + tag;
  
  m_expFermiFunc = new TF1(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 5);

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);
  m_expFermiFunc->SetParameter(4, 0);

  m_norm = 1./m_expFermiFunc->GetMaximum();
  m_timeCorr = 0.5*m_tau1*std::log(m_tau2/m_tau1 - 1.0);

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
  ZDCFitWrapper(0), m_tau1(tau1), m_tau2(tau2)
{
  // Create the reference function that we use to evaluate ExpFermiFit more efficiently
  //
  std::string funcNameRefFunc = "ExpFermiPulseSequenceRefFunc" + tag;
  
  m_expFermiFunc = new TF1(funcNameRefFunc.c_str(), ZDCFermiExpFit, -50, 100, 5);

  m_expFermiFunc->SetParameter(0, 1);
  m_expFermiFunc->SetParameter(1, 0);
  m_expFermiFunc->SetParameter(2, m_tau1);
  m_expFermiFunc->SetParameter(3, m_tau2);
  m_expFermiFunc->SetParameter(4, 0);

  m_norm = 1./m_expFermiFunc->GetMaximum();
  m_timeCorr = 0.5*m_tau1*std::log(m_tau2/m_tau1 - 1.0);

  // Calculate how many pulses we have to analyze
  //
  size_t numPrePulse = std::ceil((nominalT0 - tmin)/deltaT) + 1;
  for (size_t ipre = 0; ipre <  numPrePulse; ipre++) {
    m_pulseDeltaT.push_back(((float) ipre) * -deltaT);
  }

  size_t numPostPulse = std::floor((tmax - nominalT0)/deltaT);
  for (size_t ipost = 0; ipost <  numPrePulse; ipost++) {
    m_pulseDeltaT.push_back(((float) ipost) * deltaT);
  }

  m_numPulses = 1 + numPrePulse + numPostPulse;

  // Now set up the actual TF1
  //
  
  m_fitFunc = new TF1(("ExpFermiPulseSequence" + tag).c_str(), this, tmin, tmax, m_numPulses + 1),

  m_fitFunc->SetParName(0, "Amp");
  m_fitFunc->SetParName(1, "T0");

  m_fitFunc->SetParLimits(0, 0, 1024);
  m_fitFunc->SetParLimits(1, nominalT0 - deltaT/2, nominalT0 + deltaT/2);

  for (size_t ipulse = 0; ipulse < m_numPulses - 1; ipulse++) {
    std::string name = "Amp" + std::to_string(ipulse + 1);
    m_fitFunc->SetParName(ipulse + 2, name.c_str());
    m_fitFunc->SetParLimits(ipulse + 2, 0, 1024);
  }
}

void ZDCFitExpFermiPulseSequence::Initialize(float initialAmp, float initialT0)
{
  m_fitFunc->SetParameter(0, initialAmp);
  m_fitFunc->SetParameter(1, initialT0);

  for (size_t ipulse = 0; ipulse < m_numPulses - 1; ipulse++) {
    m_fitFunc->SetParameter(ipulse + 2, 0);
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
