/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ZDCFitWrapper_h
#define _ZDCFitWrapper_h

// Base class that defines the interface
//
#include <TF1.h>

double ZDCFermiExpFit(double* xvec, double* pvec);

class ZDCFitWrapper
{
  TF1* _wrapperTF1;

public:
  ZDCFitWrapper(TF1* wrapperTF1) : _wrapperTF1(wrapperTF1) {}

  virtual ~ZDCFitWrapper() {delete _wrapperTF1;}

  virtual void Initialize(float initialAmp, float initialT0) = 0;

  virtual float GetAmplitude() const = 0;
  virtual float GetAmpError() const = 0;
  virtual float GetTime() const = 0;
  virtual float GetTau1() const = 0;
  virtual float GetTau2() const = 0;

  virtual float GetBkgdMaxFraction() const = 0;

  virtual float GetShapeParameter(size_t index) const = 0;

  virtual double operator()(double *x, double *p) = 0;

  virtual TF1* GetWrapperTF1() {return _wrapperTF1;}
  virtual const TF1* GetWrapperTF1() const {return _wrapperTF1;}
};

class ZDCPrePulseFitWrapper : public ZDCFitWrapper
{
public:
  ZDCPrePulseFitWrapper(TF1* wrapperTF1) : ZDCFitWrapper(wrapperTF1) {}

  virtual void SetInitialPrePulseT0(float t0) = 0;
};

class ZDCFitExpFermiVariableTaus : public ZDCFitWrapper
{
  bool _fixTau1;
  bool _fixTau2;

  float _tau1;
  float _tau2;

public:

  ZDCFitExpFermiVariableTaus(std::string tag, float tmin, float tmax, bool fixTau1, bool fixTau2, float tau1, float tau2);

  virtual void Initialize(float initialAmp, float initialT0);

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
    if (tau2 > tau1) fitT0 +=  0.5*tau1*std::log(tau2/tau1 - 1.0);
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
  float _tau1;
  float _tau2;

  float _norm;
  float _timeCorr;

  TF1* _expFermiFunc;

public:

  ZDCFitExpFermiFixedTaus(std::string tag, float tmin, float tmax, float tau1, float tau2);

  ~ZDCFitExpFermiFixedTaus() { 
    delete _expFermiFunc; 
  }

  virtual void Initialize(float initialAmp, float initialT0);

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return _tau1;}
  virtual float GetTau2() const {return _tau2;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    fitT0 += _timeCorr; 
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return _tau1;
    else if (index == 1) return _tau2;
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

    double bckgd = p[2]*x[0]; 

    double expFermi =  amp*_norm*_expFermiFunc->operator()(deltaT);

    return expFermi + bckgd;
  }
};

class ZDCFitExpFermiPrePulse : public ZDCPrePulseFitWrapper
{
  float _tau1;
  float _tau2;
  float _norm;
  float _timeCorr;
  TF1* _expFermiFunc;

public:
  ZDCFitExpFermiPrePulse(std::string tag, float tmin, float tmax, float tau1, float tau2);

  virtual void Initialize(float initialAmp, float initialT0);
  //  void InitializePrePulseT0(float initPreT0) {  GetWrapperTF1()->SetParameter(3, initPreT0);}

  virtual void SetInitialPrePulseT0(float t0) {GetWrapperTF1()->SetParameter(3, t0);}

  virtual float GetAmplitude() const {return GetWrapperTF1()->GetParameter(0); }
  virtual float GetAmpError() const {return GetWrapperTF1()->GetParError(0); }

  virtual float GetTau1() const {return _tau1;}
  virtual float GetTau2() const {return _tau2;}

  virtual float GetTime() const {
    float fitT0 =  GetWrapperTF1()->GetParameter(1);

    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    fitT0 += _timeCorr; 
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return _tau1;
    else if (index == 1) return _tau2;
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

    double background = slope*maxTime + preAmp*_norm*(_expFermiFunc->operator()(deltaTPre) - 
							 _expFermiFunc->operator()(-preT0));

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

    double bckgd = linSlope*t; 

    double pulse1 =  amp*_norm*_expFermiFunc->operator()(deltaT);
    double pulse2 =  preAmp*_norm*(_expFermiFunc->operator()(deltaTPre) - 
				   _expFermiFunc->operator()(-preT0));

    return pulse1 + pulse2 + bckgd;
  }
};

class ZDCFitExpFermiPulseSequence : public ZDCFitWrapper
{
  float _tau1;
  float _tau2;
  float _norm;
  float _timeCorr;

  size_t _numPulses;
  std::vector<float> _pulseDeltaT;

  TF1* _fitFunc;
  TF1* _expFermiFunc;


public:
  ZDCFitExpFermiPulseSequence(std::string tag, float tmin, float tmax, float nominalT0, float deltaT,  float tau1, float tau2);

  ~ZDCFitExpFermiPulseSequence() 
  {
    delete _fitFunc;
    delete _expFermiFunc;
  }

  virtual TF1* GetWrapperTF1() {return _fitFunc;}
  virtual const TF1* GetWrapperTF1() const {return _fitFunc;}

  virtual void Initialize(float initialAmp, float initialT0);
  //  void InitializePrePulseT0(float initPreT0) {  GetWrapperTF1()->SetParameter(3, initPreT0);}

  virtual float GetAmplitude() const {return _fitFunc->GetParameter(0); }
  virtual float GetAmpError() const {return _fitFunc->GetParError(0); }

  virtual float GetTau1() const {return _tau1;}
  virtual float GetTau2() const {return _tau2;}

  virtual float GetTime() const {
    float fitT0 =  _fitFunc->GetParameter(1);

    // Correct the time to the maximum (the factor of 1/2 is still not fully understood)
    //
    fitT0 += _timeCorr; 
    return fitT0;
  }

  virtual float GetShapeParameter(size_t index) const
  {
    if (index == 0) return _tau1;
    else if (index == 1) return _tau2;
    else if (index < _numPulses + 2) return _fitFunc->GetParameter(index);
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
    double funcValue =  mainAmp*_norm*_expFermiFunc->operator()(mainDeltaT);

    for (size_t ipulse = 0; ipulse < _numPulses - 1; ipulse++) {
      double deltaT = mainDeltaT - _pulseDeltaT[ipulse];
      double amp = p[2 + ipulse];

      double pulse = amp*_norm*_expFermiFunc->operator()(deltaT);
      funcValue += pulse;
    }

    return funcValue;
  }
};
#endif
