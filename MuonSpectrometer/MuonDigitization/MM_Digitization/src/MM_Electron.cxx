/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_Electron.h"

using namespace std;

MM_Electron::MM_Electron() : time(-99999), charge(-99999) {}

MM_Electron::MM_Electron(float _x, float _y) : initialPosition(_x,_y), time(-99999), charge(-99999) {

}

MM_Electron::MM_Electron(const MM_Electron& /* _MM_Electron */) : time(-99999), charge(-99999) {

}

void MM_Electron::diffuseElectron(float LongitudinalSigma, float TransverseSigma, TRandom3* rndm) {

  LongitudinalDiffusionFunction->SetParameters(1.0, initialPosition.Y(), initialPosition.Y()*LongitudinalSigma);

  if (LongitudinalSigma == 0 || TransverseSigma == 0) {
    TransverseDiffusionFunction->SetParameters(initialPosition.Y()*TransverseSigma, 0.0);
  } else {
    TransverseDiffusionFunction->SetParameters(initialPosition.Y()*TransverseSigma, 1.0);
  }
  gRandom = rndm;
  offsetPosition.Set(TransverseDiffusionFunction->GetRandom(), LongitudinalDiffusionFunction->GetRandom());

}
void MM_Electron::setOffsetPosition(float x, float y) { offsetPosition.Set(x, y);}

void MM_Electron::propagateElectron(float lorentzAngle, float driftVel) {

  float tanLorentzAngle = TMath::Tan(lorentzAngle);
  if (tanLorentzAngle == tanLorentzAngle) // checking that it's not NAN
    offsetPosition.Set(offsetPosition.X() + tanLorentzAngle * offsetPosition.Y(), offsetPosition.Y());

  if (driftVel > 0.)
    time = offsetPosition.Mod()/driftVel;
  else
    time = -1.;

}

void MM_Electron::avalancheElectron(float gain, TRandom3* rndm) {

  PolyaFunction->SetParameters(2.3, 1.0);
  gRandom = rndm;
  charge = gain*PolyaFunction->GetRandom();

}

void MM_Electron::setTime(float Time) {time = Time;}
void MM_Electron::setCharge(float Charge) {charge = Charge;}

float MM_Electron::getCharge() const { return charge; }
float MM_Electron::getTime() const { return time; }
float MM_Electron::getX() const { return offsetPosition.X() + initialPosition.X(); }
float MM_Electron::getY() const { return offsetPosition.Y() + initialPosition.Y(); }
float MM_Electron::getInitialX() const { return initialPosition.X(); }
float MM_Electron::getInitialY() const { return initialPosition.Y(); }
