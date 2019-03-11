/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_RESPONSE_MDT_RESPONSE_H
#define MDT_RESPONSE_MDT_RESPONSE_H

#include <vector>

#include "MDT_Response/Amplifier.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
// Add support for CLHEP 1.9 by replacing forward declarations by inclusion
// of equivalent header file.
#include "CLHEP/Random/RandomEngine.h"


class MDT_Response {

 public:
  typedef std::vector<double> clusterVec;

  MDT_Response();
  MDT_Response(double binsize, double window); // time window in which to look for threshold
  ~MDT_Response();

  // set segment
  void   SetSegment(double r, double x);
 
  // set parameters
  void SetTubeRadius(double radius);
  void SetClusterDensity(double dens);
  void SetAttLength(double len);
  void SetRtMode(unsigned int mode);
  void SetDifRMS(double rms);
  void SetTriggerElectron(double el);
  void SetIntegrationWindow(double win);
  void SetRtParameters(int npar,double *par);
  
  // get functions
  bool   GetSignal(CLHEP::HepRandomEngine *rndmEngine);        // processes hit, returns true if amplifier passed threshold
  bool   GetSignal(double ParticleCharge,double ParticleGamma,CLHEP::HepRandomEngine *rndmEngine);         // processes hit, returns true if amplifier passed threshold
 
  // acces to amplifier
  double DriftTime() const;
  double TThreshold() const;
  double TFirst() const;
  double T0() const;
  double AdcResponse() const; 
  double Charge() const;
  double V_r(double r);
  double DoStep(CLHEP::HepRandomEngine *rndmEngine) const;
  double DampingFactor(double x);
  double PropagationDelay(double x);
  double RtoT(double r);
  double Diffusion(double r, CLHEP::HepRandomEngine *rndmEngine) const;
  double SigmaDiffusion(double r) const;
  int    GenerateQ(CLHEP::HepRandomEngine *rndmEngine) const;

  const double* RtParameters() const;
 private:
  void   InitTubeParameters();
  void   InitClusters(double timewindow, double binsize);
  void   InitRt();
  void   InitdEdxTable(); 
  void   DoStepping(CLHEP::HepRandomEngine *rndmEngine);
  void   DoStepping(double ParticleCharge,double ParticleGamma, CLHEP::HepRandomEngine *rndmEngine); 
  void   Reset();

  double     m_radius;              // radius of the tube

  double     m_rhit;                // radius of the current hit
  double     m_xhit;                // position along the tube of the current hit
  double     m_pathLength;          // path length of particle in tube
 
  double m_clusterDensity;          // clusters per mm
  std::vector<double> m_gammaFactorVec; // gamma	
  std::vector<double> m_numberOfClustersPerCmVec; // clusters per cm 

  double m_attLength;               // attenuation length of tube
  double m_signalSpeed;             // propagation speed along wire
 
  int     m_rtMode;                  // choose rt mode
  unsigned int m_rtNpar;
  double* m_rtParameters;

  double m_difSmearing;             // width of gaussian smearing 

  double m_triggerElectron;
  double m_integrationWindow;
  double m_binsize;
  double m_timeWindow; 
  int m_offset;
  int m_bins;

  Amplifier  m_amplifier;           // amplifier
  clusterVec m_clusters;            // produced clusters

  double m_t0;
};

inline   
void MDT_Response::SetSegment(double r, double x)
{
  m_rhit = r;
  m_xhit = x;
  m_pathLength = sqrt(m_radius*m_radius - r*r);
  Reset();
}

inline
double MDT_Response::DoStep(CLHEP::HepRandomEngine *rndmEngine) const
{
  return (-1./m_clusterDensity)*log( CLHEP::RandFlat::shoot(rndmEngine) );
}

inline   
int MDT_Response::GenerateQ(CLHEP::HepRandomEngine *rndmEngine) const
{
  double v = CLHEP::RandFlat::shoot(rndmEngine);
  double p;
  if(v<0.08){
    p = CLHEP::RandPoisson::shoot(rndmEngine,13.);
  }else if(v<0.28){
    double v1 = CLHEP::RandFlat::shoot(rndmEngine);
    p = (1./(v1+1.e-7));
  }else{
    p = 1+CLHEP::RandPoisson::shoot(rndmEngine,0.05);
  }
  return (int)p;
}
 
inline   
double MDT_Response::DampingFactor(double x) { return exp(-1.*x/m_attLength); }
   
inline   
double MDT_Response::PropagationDelay(double x){ return x/m_signalSpeed; }
  
inline   
double MDT_Response::RtoT(double r)
{
  int j = m_rtNpar-1;
  double time = m_rtParameters[j];
  while(j>0) { --j; time = time*r + m_rtParameters[j]; }
  return time;
} 

inline   
double MDT_Response::V_r(double r)
{
  int j = m_rtNpar-1;
  double p = m_rtParameters[j];
  double dp = 0.;
  while(j>0) {dp = dp*r + p;--j;p=p*r+m_rtParameters[j]; }
  return 1./dp;
} 

inline   
double MDT_Response::Diffusion(double r, CLHEP::HepRandomEngine *rndmEngine) const
{
  // Smearing with maximum of three sigma 
  double sigma = SigmaDiffusion(r);
  double t = CLHEP::RandGaussZiggurat::shoot(rndmEngine,0.,sigma); 
  if(fabs(t) > 3*sigma) {
    t = Diffusion(r, rndmEngine);
  }
  return t;
}

inline   
double MDT_Response::SigmaDiffusion(double r) const
{
  return m_difSmearing*r/m_radius;
}

// access to amplifier response
inline double MDT_Response::DriftTime() const
{ return m_t0 + m_amplifier.TThreshold() - m_offset*m_binsize; }

inline double MDT_Response::TThreshold() const 
{ return m_amplifier.TThreshold() - m_offset*m_binsize; }

inline double MDT_Response::TFirst() const 
{ return m_amplifier.TFirst(); }

inline double MDT_Response::T0() const 
{ return m_t0; }

inline double MDT_Response::AdcResponse() const 
{ return m_amplifier.AdcResponse(); } 

inline double MDT_Response::Charge() const 
{ return m_amplifier.Charge(); }

inline void MDT_Response::SetTubeRadius(double radius)
{ m_radius = radius ; }    


inline void MDT_Response::SetClusterDensity(double dens) 
{ m_clusterDensity = dens ; }    

inline void MDT_Response::SetAttLength(double len) 
{ m_attLength = len ; }

inline void MDT_Response::SetRtMode(unsigned int mode) 
{ m_rtMode = mode;   InitRt(); }

inline void MDT_Response::SetDifRMS(double rms) 
{ m_difSmearing = rms; InitClusters(m_timeWindow,m_binsize); }

inline void MDT_Response::SetTriggerElectron(double el) 
{  m_triggerElectron = el; m_amplifier.SetTriggerElectron(el); }

inline void MDT_Response::SetIntegrationWindow(double win) 
{ m_integrationWindow = win; m_amplifier.SetIntegrationWindow(win); }

inline void MDT_Response::SetRtParameters(int npar,double *par)
{
  m_rtNpar = npar;
  if(m_rtParameters) delete m_rtParameters;
  m_rtParameters = new double[m_rtNpar];
  for(unsigned int i=0;i<m_rtNpar;++i)
    m_rtParameters[i] = par[i]; 
}
inline const double* MDT_Response::RtParameters() const
{
  return m_rtParameters;
}

#endif
