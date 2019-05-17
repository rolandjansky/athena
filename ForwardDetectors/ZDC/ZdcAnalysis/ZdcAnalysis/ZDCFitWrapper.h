/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ZDCFitWrapper_h
#define _ZDCFitWrapper_h

// Base class that defines the interface
//
#include <TF1.h>

double ZDCFermiExpFit(double* xvec, double* pvec);

class ZDCFitWrapper
{
  TF1* m_wrapperTF1;

public:
  ZDCFitWrapper(TF1* wrapperTF1) : m_wrapperTF1(wrapperTF1) {}

  virtual ~ZDCFitWrapper() {delete m_wrapperTF1;}

  virtual void Initialize(float initialAmp, float initialT0) = 0;

  virtual void SetFitT0Max(float t0Min, float t0Max) = 0;

  virtual float GetAmplitude() const = 0;
  virtual float GetAmpError() const = 0;
  virtual float GetTime() const = 0;
  virtual float GetTau1() const = 0;
  virtual float GetTau2() const = 0;

  virtual float GetBkgdMaxFraction() const = 0;

  virtual float GetShapeParameter(size_t index) const = 0;

  virtual double operator()(double *x, double *p) = 0;

  virtual TF1* GetWrapperTF1() {return m_wrapperTF1;}
  virtual const TF1* GetWrapperTF1() const {return m_wrapperTF1;}
};

class ZDCPrePulseFitWrapper : public ZDCFitWrapper
{
public:
  ZDCPrePulseFitWrapper(TF1* wrapperTF1) : ZDCFitWrapper(wrapperTF1) {}

  virtual void SetInitialPrePulseT0(float t0) = 0;
};

class ZDCFitExpFermiVariableTaus : public ZDCFitWrapper
{
  bool m_fixTau1;
  bool m_fixTau2;

  float m_tau1;
  float m_tau2;

public:

  ZDCFitExpFermiVariableTaus(std::string tag, float tmin, float tmax, bool fixTau1, bool fixTau2, float tau1, float tau2);

  virtual void Initialize(float initialAmp, float initialT0);

  virtual void SetFitT0Max(float t0Min, float t0Max);

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return GetWrapperTF1()->GetParameter(2);}
  virtual float GetTau2() const {return GetWrapperTF1()->GetParameter(3);}

  virtual float GetTime() const {
    const TF1* theTF1 = GetWrapperTF1();

    float fitT0 =  theTF1->GetParameter(1);

    float tau1 = theTF1->GetParameter(2);
    float tau2 = theTF1->GetParameter(3);

    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    if (tau2 > tau1) fitT0 += tau1*std::log(tau2/tau1 - 1.0);
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return GetWrapperTF1()->GetParameter(2);
    else if (index == 1) return GetWrapperTF1()->GetParameter(3);
    else throw;
  }

  virtual float GetBkgdMaxFraction() const
  {
    const TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();
    double amp = theTF1->GetParameter(0);
    double slope = theTF1->GetParameter(4);

    double background = slope*GetTime();
    return background/amp;
  }

  //  virtual float GetNDOF() const {return _fitFunc->GetNDF(); }

  virtual double operator()(double *x, double *p) {
    return ZDCFermiExpFit(x, p);
  }

};

class ZDCFitExpFermiFixedTaus : public ZDCFitWrapper
{
  float m_tau1;
  float m_tau2;

  float m_norm;
  float m_timeCorr;

  TF1* m_expFermiFunc;

public:

  ZDCFitExpFermiFixedTaus(std::string tag, float tmin, float tmax, float tau1, float tau2);

  ~ZDCFitExpFermiFixedTaus() {
    delete m_expFermiFunc;
  }

  virtual void Initialize(float initialAmp, float initialT0);

  virtual void SetFitT0Max(float t0Min, float t0Max);

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    fitT0 += m_timeCorr;
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return m_tau1;
    else if (index == 1) return m_tau2;
    else throw;
  }

  virtual float GetBkgdMaxFraction() const
  {
    const TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();
    double amp = theTF1->GetParameter(0);
    double slope = theTF1->GetParameter(2);

    double background = slope*GetTime();
    return background/amp;
  }

  virtual double operator() (double *x, double *p)
  {
    double amp = p[0];
    double t0 = p[1];
    double deltaT = x[0] - t0;

    // double bckgd = p[2]*x[0];

    double expFermi =  amp*m_norm*m_expFermiFunc->operator()(deltaT);

    return expFermi; // + bckgd;
  }
};

class ZDCFitExpFermiPrePulse : public ZDCPrePulseFitWrapper
{
  float m_tau1;
  float m_tau2;
  float m_norm;
  float m_timeCorr;
  TF1* m_expFermiFunc;

public:
  ZDCFitExpFermiPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2);

  virtual void Initialize(float initialAmp, float initialT0);
  //  void InitializePrePulseT0(float initPreT0) {  GetWrapperTF1()->SetParameter(3, initPreT0);}

  virtual void SetFitT0Max(float t0Min, float t0Max);

  virtual void SetInitialPrePulseT0(float t0) {GetWrapperTF1()->SetParameter(3, t0);}

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    fitT0 += m_timeCorr;
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return m_tau1;
    else if (index == 1) return m_tau2;
    else if (index < 5) return GetWrapperTF1()->GetParameter(index);
    else throw;
  }

  virtual float GetBkgdMaxFraction() const
  {
    const TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

    double maxTime = GetTime();

    double amp = theTF1->GetParameter(0);
    double preAmp = theTF1->GetParameter(2);
    double preT0 = theTF1->GetParameter(3);
    double slope = theTF1->GetParameter(4);

    double deltaTPre = maxTime - preT0;

    double background = slope*maxTime + preAmp*m_norm*(m_expFermiFunc->operator()(deltaTPre) -
							 m_expFermiFunc->operator()(-preT0));

    return background/amp;
  }

  virtual double operator() (double *x, double *p)
  {
    double t = x[0];

    double amp = p[0];
    double t0 = p[1];
    double preAmp = p[2];
    double preT0 = p[3];
    //    double linSlope = p[4];

    double deltaT = t - t0;
    double deltaTPre = t - preT0;

    // double bckgd = linSlope*t;

    double pulse1 =  amp*m_norm*m_expFermiFunc->operator()(deltaT);
    double pulse2 =  preAmp*m_norm*(m_expFermiFunc->operator()(deltaTPre) -
				   m_expFermiFunc->operator()(-preT0));

    return pulse1 + pulse2;// + bckgd;
  }
};

class ZDCFitExpFermiPulseSequence : public ZDCFitWrapper
{
  float m_tau1;
  float m_tau2;
  float m_norm;
  float m_timeCorr;

  size_t m_numPulses;
  std::vector<float> m_pulseDeltaT;

  TF1* m_fitFunc;
  TF1* m_expFermiFunc;


public:
  ZDCFitExpFermiPulseSequence(std::string tag, float tmin, float tmax, float nominalT0, float deltaT,  float tau1, float tau2);

  ~ZDCFitExpFermiPulseSequence()
  {
    delete m_fitFunc;
    delete m_expFermiFunc;
  }

  virtual TF1* GetWrapperTF1() {return m_fitFunc;}
  virtual const TF1* GetWrapperTF1() const {return m_fitFunc;}

  virtual void Initialize(float initialAmp, float initialT0);
  //  void InitializePrePulseT0(float initPreT0) {  GetWrapperTF1()->SetParameter(3, initPreT0);}

  virtual void SetFitT0Max(float t0Min, float t0Max);

  virtual float GetAmplitude() const {return m_fitFunc->GetParameter(0); }
  virtual float GetAmpError() const {return m_fitFunc->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetTime() const {
    float fitT0 =  m_fitFunc->GetParameter(1);

    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    fitT0 += m_timeCorr;
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return m_tau1;
    else if (index == 1) return m_tau2;
    else if (index < m_numPulses + 2) return m_fitFunc->GetParameter(index);
    else throw;
  }

  virtual float GetBkgdMaxFraction() const
  {
    return 0;
  }

  virtual double operator() (double *x, double *p)
  {
    double t = x[0];

    double mainAmp = p[0];
    double t0 = p[1];

    double mainDeltaT = t - t0;
    double funcValue =  mainAmp*m_norm*m_expFermiFunc->operator()(mainDeltaT);

    for (size_t ipulse = 0; ipulse < m_numPulses - 1; ipulse++) {
      double deltaT = mainDeltaT - m_pulseDeltaT[ipulse];
      double amp = p[2 + ipulse];

      double pulse = amp*m_norm*m_expFermiFunc->operator()(deltaT);
      funcValue += pulse;
    }

    return funcValue;
  }
};

// ----------------------------------------------------------------------
class ZDCFitExpFermiLinearFixedTaus : public ZDCFitWrapper
{
  float m_tau1;
  float m_tau2;

  float m_norm;
  float m_timeCorr;

  TF1* m_expFermiFunc;

public:

  ZDCFitExpFermiLinearFixedTaus(std::string tag, float tmin, float tmax, float tau1, float tau2);

  ~ZDCFitExpFermiLinearFixedTaus() {
    delete m_expFermiFunc;
  }

  virtual void Initialize(float initialAmp, float initialT0);

  virtual void SetFitT0Max(float t0Min, float t0Max);  // Bill 03/01

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);
    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    fitT0 += m_timeCorr;
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return m_tau1;
    else if (index == 1) return m_tau2;
    else throw;
  }

  virtual float GetBkgdMaxFraction() const
  {
    const TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();
    double amp = theTF1->GetParameter(0);
    double slope = theTF1->GetParameter(2);

    double background = slope*GetTime();
    return background/amp;
  }

  virtual double operator() (double *x, double *p)
  {
    double amp    = p[0];
    double t0     = p[1];
    double deltaT = x[0] - t0;

    double bckgd = p[2]*x[0] + p[3];

    double expFermi =  amp*m_norm*m_expFermiFunc->operator()(deltaT);

    return expFermi + bckgd;
  }
};

// ----------------------------------------------------------------------
class ZDCFitExpFermiLinearPrePulse : public ZDCPrePulseFitWrapper
{
  float m_tau1;
  float m_tau2;
  float m_norm;
  float m_timeCorr;
  TF1* m_expFermiFunc;

public:
  ZDCFitExpFermiLinearPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2);

  virtual void Initialize(float initialAmp, float initialT0);
  //  void InitializePrePulseT0(float initPreT0) {  GetWrapperTF1()->SetParameter(3, initPreT0);}

  virtual void SetInitialPrePulseT0(float t0) {GetWrapperTF1()->SetParameter(3, t0);}

  virtual void SetFitT0Max(float t0Min, float t0Max);  // Bill 03/01

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    fitT0 += m_timeCorr;
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return m_tau1;
    else if (index == 1) return m_tau2;
    else if (index < 5) return GetWrapperTF1()->GetParameter(index);
    else throw;
  }

  virtual float GetBkgdMaxFraction() const
  {
    const TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

    double maxTime = GetTime();

    double amp = theTF1->GetParameter(0);
    double preAmp = theTF1->GetParameter(2);
    double preT0 = theTF1->GetParameter(3);
    double slope = theTF1->GetParameter(4);

    double deltaTPre = maxTime - preT0;

    double background = slope*maxTime + preAmp*m_norm*(m_expFermiFunc->operator()(deltaTPre) -
               m_expFermiFunc->operator()(-preT0));

    return background/amp;
  }

  virtual double operator() (double *x, double *p)
  {
    double t = x[0];

    double amp = p[0];
    double t0 = p[1];
    double preAmp = p[2];
    double preT0 = p[3];
    double linSlope = p[4];

    double deltaT = t - t0;
    double deltaTPre = t - preT0;

    double bckgd = linSlope*t + p[5];

    double pulse1 =  amp*m_norm*m_expFermiFunc->operator()(deltaT);
    double pulse2 =  preAmp*m_norm*(m_expFermiFunc->operator()(deltaTPre) -
           m_expFermiFunc->operator()(-preT0));

    return pulse1 + pulse2 + bckgd;
  }
};
#endif
