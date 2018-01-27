/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_Electron.h"

using namespace std;

MM_Electron::MM_Electron() : m_time(-99999), m_charge(-99999) {}

MM_Electron::MM_Electron(float x, float y) : m_initialPosition(x,y), m_time(-99999), m_charge(-99999) {
  
  //  m_PolyaFunction = unique_ptr<TF1>(new TF1("polya","(1./[1])*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x,[0])*TMath::Exp(-([0]+1)*x)", 0., 4.));  
  // The typical widths are 0.036 and 0.019, so [-1, 1] should be a safe interval.
  //  m_LongitudinalDiffusionFunction = unique_ptr<TF1>(new TF1("longdiff","gaus", -1., 10.));
  //  m_TransverseDiffusionFunction = unique_ptr<TF1>(new TF1("transdiff", "1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.));
    
}  

MM_Electron::MM_Electron(const MM_Electron& /* other */) : m_time(-99999), m_charge(-99999) {

  //  m_PolyaFunction = unique_ptr<TF1>((TF1*) _MM_Electron.m_PolyaFunction->Clone());
  //  m_LongitudinalDiffusionFunction = unique_ptr<TF1>((TF1*) _MM_Electron.m_LongitudinalDiffusionFunction->Clone());
  //  m_TransverseDiffusionFunction = unique_ptr<TF1>((TF1*) _MM_Electron.m_TransverseDiffusionFunction->Clone());

}

void MM_Electron::diffuseElectron(float LongitudinalSigma, float TransverseSigma, TRandom3* rndm) {
  
  m_LongitudinalDiffusionFunction->SetParameters(1.0, m_initialPosition.Y(), m_initialPosition.Y()*LongitudinalSigma);
  
  if (LongitudinalSigma == 0 || TransverseSigma == 0) {
    m_TransverseDiffusionFunction->SetParameters(m_initialPosition.Y()*TransverseSigma, 0.0);
  } else {
    m_TransverseDiffusionFunction->SetParameters(m_initialPosition.Y()*TransverseSigma, 1.0);
  }
  gRandom = rndm;
  m_offsetPosition.Set(m_TransverseDiffusionFunction->GetRandom(), m_LongitudinalDiffusionFunction->GetRandom());

}
void MM_Electron::setOffsetPosition(float x, float y) { m_offsetPosition.Set(x, y);}
    
void MM_Electron::propagateElectron(float driftVelx, float driftVely, float driftVel) {

  if (driftVely != 0.)
    m_offsetPosition.Set(m_offsetPosition.X() + driftVelx*m_offsetPosition.Y()/driftVely, m_offsetPosition.Y());
  if (driftVel > 0.)
    m_time = m_offsetPosition.Mod()/driftVel;
  else
    m_time = -1.;
  
}

void MM_Electron::avalancheElectron(float gain, TRandom3* rndm) {
  
  m_PolyaFunction->SetParameters(2.3, 1.0);
  gRandom = rndm;
  m_charge = gain*m_PolyaFunction->GetRandom();
  
}

void MM_Electron::setTime(float Time) {m_time = Time;}
void MM_Electron::setCharge(float Charge) {m_charge = Charge;}

float MM_Electron::getCharge() const { return m_charge; }
float MM_Electron::getTime() const { return m_time; }
float MM_Electron::getX() const { return m_offsetPosition.X() + m_initialPosition.X(); }
float MM_Electron::getInitialX() const { return m_initialPosition.X(); }
float MM_Electron::getInitialY() const { return m_initialPosition.Y(); }
