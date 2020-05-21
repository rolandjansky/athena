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

  float m_tmin;
  float m_tmax;

  float m_t0Min;
  float m_t0Max;

  bool  m_adjTLimitsEvent;
  float m_tminAdjust;

public:
  ZDCFitWrapper(TF1* wrapperTF1) : m_wrapperTF1(wrapperTF1), //m_t0Min(t0Min), m_t0Max(t0Max)
    m_adjTLimitsEvent(true)  // true here forces a setting of T0 par limits on first event
  {
    m_tmin = m_wrapperTF1->GetXmin();
    m_tmax = m_wrapperTF1->GetXmax();

    m_t0Min = m_tmin;
    m_t0Max = m_tmax;
  }

  virtual ~ZDCFitWrapper() {delete m_wrapperTF1;}

  void Initialize(float initialAmp, float initialT0);
  void Initialize(float initialAmp, float initialT0, float fitTmin, float fitTmax);

  // Performs the class-specific event initialization
  //
  virtual void DoInitialize(float initialAmp, float initialT0) = 0;

  // Actually sets the t0 parameter limits in the fit function
  //
  virtual void SetT0FitLimits(float tMin, float tMax) = 0;

  void SetT0Range(float t0Min, float t0Max)
  {
    m_t0Min = t0Min;
    m_t0Max = t0Max;

    SetT0FitLimits(t0Min, t0Max);
  }

  virtual float GetAmplitude() const = 0;
  virtual float GetAmpError() const = 0;
  virtual float GetTime() const = 0;
  virtual float GetTau1() const = 0;
  virtual float GetTau2() const = 0;

  float GetTMin() const {return m_tmin;}
  float GetTMax() const {return m_tmax;}

  float GetT0Min() const {return m_t0Min;}
  float GetT0Max() const {return m_t0Max;}

  float GetTMinAdjust() const {return m_tminAdjust;}

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

  virtual void SetInitialPrePulse(float amp, float t0, float expamp, bool fixPrePulseToZero) = 0; // bill

  virtual void SetPrePulseT0Range(float tmin, float tmax) = 0;
  virtual void SetPostPulseT0Range(float tmin, float tmax, float initialPostT0) = 0; // bill

  virtual unsigned int GetPreT0ParIndex() const = 0;

  virtual float GetPreT0()  const = 0;  // bill
  virtual float GetPreAmp() const = 0;  // bill

  virtual float GetPostT0()  const = 0;  // bill
  virtual float GetPostAmp() const = 0;  // bill

  virtual float GetExpAmp()  const = 0;  // bill
};

class ZDCFitExpFermiVariableTaus : public ZDCFitWrapper
{
  bool m_fixTau1;
  bool m_fixTau2;

  float m_tau1;
  float m_tau2;

public:

  ZDCFitExpFermiVariableTaus(std::string tag, float tmin, float tmax, bool fixTau1, bool fixTau2, float tau1, float tau2);

  virtual void DoInitialize(float initialAmp, float initialT0);
  virtual void SetT0FitLimits(float tMin, float tMax);

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return GetWrapperTF1()->GetParameter(2);}
  virtual float GetTau2() const {return GetWrapperTF1()->GetParameter(3);}

  virtual float GetTime() const {
    const TF1* theTF1 = GetWrapperTF1();

    float fitT0 =  theTF1->GetParameter(1);

    float tau1 = theTF1->GetParameter(2);
    float tau2 = theTF1->GetParameter(3);

    // Correct the time to the maximum
    //
    if (tau2 > tau1) fitT0 += tau1 * std::log(tau2 / tau1 - 1.0);
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

    double background = slope * GetTime();
    return background / amp;
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

  virtual void DoInitialize(float initialAmp, float initialT0);
  virtual void SetT0FitLimits(float tMin, float tMax);

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum
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

    double background = slope * GetTime();
    return background / amp;
  }

  virtual double operator() (double *x, double *p)
  {
    double amp = p[0];
    double t0 = p[1];
    double deltaT = x[0] - t0;

    // double bckgd = p[2]*x[0];

    double expFermi =  amp * m_norm * m_expFermiFunc->operator()(deltaT);

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

  virtual void DoInitialize(float initialAmp, float initialT0);
  virtual void SetT0FitLimits(float tMin, float tMax);

  virtual void SetInitialPrePulse(float amp, float t0, float expamp, bool fixPrePulseToZero) {
    GetWrapperTF1()->SetParameter(2, std::max(amp, (float) 1.5)); //1.5 here ensures that we're above lower limit
    GetWrapperTF1()->SetParameter(3, t0);
    expamp = 0;
    fixPrePulseToZero = false;
  }

  virtual void SetPrePulseT0Range(float tmin, float tmax);
  virtual void SetPostPulseT0Range(float tmin, float tmax, float initialPostT0) {
    tmin = tmin;
    tmax = tmax;
    initialPostT0 = initialPostT0;
    return;
  }

  unsigned int GetPreT0ParIndex() const {return 3;}

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetPreT0()  const {return 0;}
  virtual float GetPreAmp() const {return 0;}

  virtual float GetPostT0()  const {return 0;}
  virtual float GetPostAmp() const {return 0;}

  virtual float GetExpAmp() const {return 0;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum
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

    double background = slope * maxTime + preAmp * m_norm * (m_expFermiFunc->operator()(deltaTPre) -
                        m_expFermiFunc->operator()(-preT0));

    return background / amp;
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

    // We subtract off the  value of the pre-pulse at the minimum time (nominally 0,
    //   but can change if we exclude early samples) to account for the subtraction of the pre-sample
    //
    double deltaPresamp = GetTMinAdjust() - preT0;

    // double bckgd = linSlope*t;

    double pulse1 =  amp * m_norm * m_expFermiFunc->operator()(deltaT);
    double pulse2 =  preAmp * m_norm * (m_expFermiFunc->operator()(deltaTPre) -
                                        m_expFermiFunc->operator()(deltaPresamp));

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

  virtual void DoInitialize(float initialAmp, float initialT0);
  virtual void SetT0FitLimits(float tMin, float tMax);

  virtual float GetAmplitude() const {return m_fitFunc->GetParameter(0); }
  virtual float GetAmpError() const {return m_fitFunc->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  unsigned int GetPreT0ParIndex() const {return 3;}

  virtual float GetTime() const {
    float fitT0 =  m_fitFunc->GetParameter(1);

    // Correct the time to the maximum
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
    double funcValue =  mainAmp * m_norm * m_expFermiFunc->operator()(mainDeltaT);

    for (size_t ipulse = 0; ipulse < m_numPulses - 1; ipulse++) {
      double deltaT = mainDeltaT - m_pulseDeltaT[ipulse];
      double amp = p[2 + ipulse];

      double pulse = amp * m_norm * m_expFermiFunc->operator()(deltaT);
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

  virtual void DoInitialize(float initialAmp, float initialT0);
  virtual void SetT0FitLimits(float tMin, float tMax);

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum
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

    double background = slope * GetTime();
    return background / amp;
  }

  virtual double operator() (double *x, double *p)
  {
    double amp    = p[0];
    double t0     = p[1];
    double deltaT = x[0] - t0;

    double bckgd = p[2] * x[0] + p[3];

    double expFermi =  amp * m_norm * m_expFermiFunc->operator()(deltaT);

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

  virtual void DoInitialize(float initialAmp, float initialT0);
  virtual void SetT0FitLimits(float tMin, float tMax);

  virtual void SetInitialPrePulse(float amp, float t0, float expamp, bool fixPrePulseToZero) {
    GetWrapperTF1()->SetParameter(2, std::max(amp, (float) 1.5)); //1.5 here ensures that we're above lower limit
    GetWrapperTF1()->SetParameter(3, t0);
    expamp = 0;
    fixPrePulseToZero = false;
  }

  virtual void SetPrePulseT0Range(float tmin, float tmax);
  virtual void SetPostPulseT0Range(float tmin, float tmax, float initialPostT0) {
    tmin = tmin;
    tmax = tmax;
    initialPostT0 = initialPostT0;
    return;
  }

  unsigned int GetPreT0ParIndex() const {return 3;}

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetPreT0()  const {
    float fitPreT0 =  GetWrapperTF1()->GetParameter(3);

    // Correct the time to the maximum
    //
    fitPreT0 += m_timeCorr;

    return fitPreT0;
  }
  virtual float GetPreAmp() const {return GetWrapperTF1()->GetParameter(2);}

  virtual float GetPostT0()  const {return 0;}
  virtual float GetPostAmp() const {return 0;}

  virtual float GetExpAmp() const {return 0;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum
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

    double background = slope * maxTime + preAmp * m_norm * (m_expFermiFunc->operator()(deltaTPre) -
                        m_expFermiFunc->operator()(-preT0));

    return background / amp;
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

    double bckgd = linSlope * t + p[5];

    // We subtract off the  value of the pre-pulse at the minimum time (nominally 0,
    //   but can change if we exclude early samples) to account for the subtraction of the pre-sample
    //
    double deltaPresamp = GetTMinAdjust() - preT0;

    double pulse1 =  amp * m_norm * m_expFermiFunc->operator()(deltaT);
    double pulse2 =  preAmp * m_norm * (m_expFermiFunc->operator()(deltaTPre) -
                                        m_expFermiFunc->operator()(deltaPresamp));

    return pulse1 + pulse2 + bckgd;
  }
};



// ----------------------------------------------------------------------
class ZDCFitComplexPrePulse : public ZDCPrePulseFitWrapper
{
  float m_tau1;
  float m_tau2;
  float m_norm;
  float m_timeCorr;
  TF1* m_expFermiFunc;

public:
  ZDCFitComplexPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2);

  virtual void DoInitialize(float initialAmp, float initialT0);
  virtual void SetT0FitLimits(float tMin, float tMax);

  virtual void SetInitialPrePulse(float amp, float t0, float expamp, bool fixPrePulseToZero) {
    GetWrapperTF1()->SetParameter(2, std::max(amp, (float) 1.5)); //1.5 here ensures that we're above lower limit
    GetWrapperTF1()->SetParameter(3, t0);
    GetWrapperTF1()->SetParameter(6, std::max(abs(expamp), (float) 1.5));
    fixPrePulseToZero = false;
    // GetWrapperTF1()->FixParameter(2, 0.001);
    // std::cout << "-----> expamp = " << expamp << "   amp = " << amp << "   t0 = " << t0 << std::endl;
  }

  virtual void SetPrePulseT0Range(float tmin, float tmax);
  virtual void SetPostPulseT0Range(float tmin, float tmax, float initialPostT0) {
    tmin = tmin;
    tmax = tmax;
    initialPostT0 = initialPostT0;
    return;
  }

  unsigned int GetPreT0ParIndex() const {return 3;}

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetPreT0()  const {
    float fitPreT0 =  GetWrapperTF1()->GetParameter(3);

    // Correct the time to the maximum
    //
    fitPreT0 += m_timeCorr;

    return fitPreT0;
  }
  virtual float GetPreAmp() const {return GetWrapperTF1()->GetParameter(2);}

  virtual float GetPostT0()  const {return 0;}
  virtual float GetPostAmp() const {return 0;}

  virtual float GetExpAmp() const {return GetWrapperTF1()->GetParameter(6);}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum
    //
    fitT0 += m_timeCorr;
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if      (index == 0) return m_tau1;
    else if (index == 1) return m_tau2;
    else if (index <  5) return GetWrapperTF1()->GetParameter(index);
    else throw;
  }

  virtual float GetBkgdMaxFraction() const
  {
    const TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

    double maxTime = GetTime();

    double amp    = theTF1->GetParameter(0);
    double preAmp = theTF1->GetParameter(2);
    double preT0  = theTF1->GetParameter(3);
    double slope  = theTF1->GetParameter(4);

    double deltaTPre = maxTime - preT0;

    double background = slope * maxTime + preAmp * m_norm * (m_expFermiFunc->operator()(deltaTPre) -
                        m_expFermiFunc->operator()(-preT0));

    return background / amp;
  }

  virtual double operator() (double *x, double *p)
  {
    double t = x[0];

    double amp      = p[0];
    double t0       = p[1];
    double preAmp   = p[2];
    double preT0    = p[3];
    double linSlope = p[4];
    double linConst = p[5];
    double expamp   = p[6];

    double deltaT    = t - t0;
    double deltaTPre = t - preT0;

    // We subtract off the  value of the pre-pulse at the minimum time (nominally 0,
    //   but can change if we exclude early samples) to account for the subtraction of the pre-sample
    //
    double deltaPresamp = GetTMinAdjust() - preT0;

    double pulse1 =  amp    * m_norm *  m_expFermiFunc->operator()(deltaT);
    double pulse2 =  preAmp * m_norm * (m_expFermiFunc->operator()(deltaTPre) - m_expFermiFunc->operator()(deltaPresamp));

    double linBG = linSlope * t + linConst;
    double expBG = expamp * std::exp(-(t) / m_tau2) - expamp;  // deltaPresamp

    return pulse1 + pulse2 + linBG + expBG;
  }
};


// ----------------------------------------------------------------------
class ZDCFitGeneralPulse : public ZDCPrePulseFitWrapper
{
  float m_tau1;
  float m_tau2;
  float m_norm;
  float m_timeCorr;
  TF1* m_expFermiFunc;

public:
  ZDCFitGeneralPulse(std::string tag, float tmin, float tmax, float tau1, float tau2);

  virtual void DoInitialize(float initialAmp, float initialT0);
  virtual void SetT0FitLimits(float tMin, float tMax);

  virtual void SetInitialPrePulse(float amp, float t0, float expamp, bool fixPrePulseToZero) {
    GetWrapperTF1()->ReleaseParameter(2);
    GetWrapperTF1()->ReleaseParameter(3);
    GetWrapperTF1()->SetParameter(2, std::max(amp, (float) 1.5)); //1.5 here ensures that we're above lower limit
    GetWrapperTF1()->SetParameter(3, std::max(t0, (float) 20.0));
    GetWrapperTF1()->SetParameter(6, std::max(abs(expamp), (float) 1.5));

    if (fixPrePulseToZero) {
      GetWrapperTF1()->FixParameter(2, 0. );
      GetWrapperTF1()->FixParameter(3, 20.);
    }
  }

  virtual void SetPrePulseT0Range(float tmin, float tmax);
  virtual void SetPostPulseT0Range(float tmin, float tmax, float initialPostT0);

  unsigned int GetPreT0ParIndex() const {return 3;}

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError()  const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return m_tau1;}
  virtual float GetTau2() const {return m_tau2;}

  virtual float GetPreT0()  const {
    float fitPreT0 =  GetWrapperTF1()->GetParameter(3);

    // Correct the time to the maximum
    //
    fitPreT0 += m_timeCorr;

    return fitPreT0;
  }
  virtual float GetPreAmp() const {return GetWrapperTF1()->GetParameter(2);}

  virtual float GetPostT0()  const {
    float fitPostT0 =  GetWrapperTF1()->GetParameter(8);

    // Correct the time to the maximum
    //
    fitPostT0 += m_timeCorr;

    return fitPostT0;
  }
  virtual float GetPostAmp() const {return GetWrapperTF1()->GetParameter(7);}

  virtual float GetExpAmp() const {return GetWrapperTF1()->GetParameter(6);}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum
    //
    fitT0 += m_timeCorr;
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if      (index == 0) return m_tau1;
    else if (index == 1) return m_tau2;
    else if (index <  5) return GetWrapperTF1()->GetParameter(index);
    else throw;
  }

  virtual float GetBkgdMaxFraction() const
  {
    const TF1* theTF1 = ZDCFitWrapper::GetWrapperTF1();

    double maxTime = GetTime();

    double amp    = theTF1->GetParameter(0);
    double preAmp = theTF1->GetParameter(2);
    double preT0  = theTF1->GetParameter(3);
    double slope  = theTF1->GetParameter(4);

    double deltaTPre = maxTime - preT0;

    double background = slope * maxTime + preAmp * m_norm * (m_expFermiFunc->operator()(deltaTPre) -
                        m_expFermiFunc->operator()(-preT0));

    return background / amp;
  }

  virtual double operator() (double *x, double *p)
  {
    double t = x[0];

    double amp      = p[0];
    double t0       = p[1];
    double preAmp   = p[2];
    double preT0    = p[3];
    double linSlope = p[4];
    double linConst = p[5];
    double expamp   = p[6];
    double postAmp  = p[7];
    double postT0   = p[8];

    double deltaT     = t - t0;
    double deltaTPre  = t - preT0;
    double deltaTPost = postT0 - t;

    // We subtract off the  value of the pre-pulse at the minimum time (nominally 0,
    //   but can change if we exclude early samples) to account for the subtraction of the pre-sample
    //
    double deltaPresamp = GetTMinAdjust() - preT0;

    double pulse1 =  amp     * m_norm *  m_expFermiFunc->operator()(deltaT);
    double pulse2 =  preAmp  * m_norm * (m_expFermiFunc->operator()(deltaTPre) - m_expFermiFunc->operator()(deltaPresamp));
    double pulse3 =  postAmp * m_norm *  m_expFermiFunc->operator()(deltaTPost);

    double linBG = linSlope * t + linConst;
    double expBG = expamp * std::exp(-(t) / m_tau2) - expamp;  // deltaPresamp

    return pulse1 + pulse2 + pulse3 + linBG + expBG;
  }
};
#endif
