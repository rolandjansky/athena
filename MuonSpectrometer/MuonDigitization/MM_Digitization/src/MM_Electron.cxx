/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_Electron.h"

using namespace std;

MM_Electron::MM_Electron() : time(-99999), charge(-99999) {}

MM_Electron::MM_Electron(float _x, float _y) : initialPosition(_x,_y), time(-99999), charge(-99999) {

  //  PolyaFunction = unique_ptr<TF1>(new TF1("polya","(1./[1])*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x,[0])*TMath::Exp(-([0]+1)*x)", 0., 4.));
  // The typical widths are 0.036 and 0.019, so [-1, 1] should be a safe interval.
  //  LongitudinalDiffusionFunction = unique_ptr<TF1>(new TF1("longdiff","gaus", -1., 10.));
  //  TransverseDiffusionFunction = unique_ptr<TF1>(new TF1("transdiff", "1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.));

}

MM_Electron::MM_Electron(const MM_Electron& /* _MM_Electron */) : time(-99999), charge(-99999) {

  //  PolyaFunction = unique_ptr<TF1>((TF1*) _MM_Electron.PolyaFunction->Clone());
  //  LongitudinalDiffusionFunction = unique_ptr<TF1>((TF1*) _MM_Electron.LongitudinalDiffusionFunction->Clone());
  //  TransverseDiffusionFunction = unique_ptr<TF1>((TF1*) _MM_Electron.TransverseDiffusionFunction->Clone());

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

void MM_Electron::propagateElectron(float driftVelx, float driftVely, float driftVel) {

  if (driftVely != 0.)
    offsetPosition.Set(offsetPosition.X() + driftVelx*offsetPosition.Y()/driftVely, offsetPosition.Y());
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
float MM_Electron::getInitialX() const { return initialPosition.X(); }
float MM_Electron::getInitialY() const { return initialPosition.Y(); }
