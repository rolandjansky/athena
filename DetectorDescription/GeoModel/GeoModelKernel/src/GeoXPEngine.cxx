/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoXPEngine.h"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/FixedConstant.hh"
#include "CLHEP/GenericFunctions/RKIntegrator.hh"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"


GeoXPEngine::GeoXPEngine (const Genfun::AbsFunction & Bx,
			  const Genfun::AbsFunction & By,
			  const Genfun::AbsFunction & Bz,
			  const HepGeom::Point3D<double>          & x0,
			  const CLHEP::HepLorentzVector    & p0,
			  double                    q   )
{


  // Peform a Runge-Kutte Integration of the trajectory of the charged
  // particle in a magnetic field.  Physics input is the Lorentz force
  // law; note, relativistic expression applies.

  double m = p0.m();
  double E = p0.e();
  Genfun::Variable t;


  if (m<100*CLHEP::keV) { 
    _x= (Genfun::FixedConstant(x0.x()) + (CLHEP::c_light*p0.x()/p0.vect().mag())*t).clone();
    _y= (Genfun::FixedConstant(x0.y()) + (CLHEP::c_light*p0.y()/p0.vect().mag())*t).clone();
    _z= (Genfun::FixedConstant(x0.z()) + (CLHEP::c_light*p0.z()/p0.vect().mag())*t).clone();
    _px  = Genfun::FixedConstant(p0.x()).clone();
    _py  = Genfun::FixedConstant(p0.y()).clone();
    _pz  = Genfun::FixedConstant(p0.z()).clone();
    

  }
  else {

    Genfun::RKIntegrator rkIntegrator;
    Genfun::Variable Px(0,6),Py(1,6), Pz(2,6), X(3,6), Y(4,6), Z(5,6);
    Genfun::FixedConstant I(1.0);
    
    Genfun::GENFUNCTION DPxDt  = (q/E)*(Py*(I%I%I%Bz) -Pz*(I%I%I%By))*CLHEP::c_light;
    Genfun::GENFUNCTION DPyDt  = (q/E)*(Pz*(I%I%I%Bx) -Px*(I%I%I%Bz))*CLHEP::c_light;
    Genfun::GENFUNCTION DPzDt  = (q/E)*(Px*(I%I%I%By) -Py*(I%I%I%Bx))*CLHEP::c_light;
    Genfun::GENFUNCTION DxDt   = Px/E;
    Genfun::GENFUNCTION DyDt   = Py/E;
    Genfun::GENFUNCTION DzDt   = Pz/E;
    
    rkIntegrator.addDiffEquation(&DPxDt, "Px",p0.x(), p0.x(), p0.x());
    rkIntegrator.addDiffEquation(&DPyDt, "Py",p0.y(), p0.y(), p0.y());
    rkIntegrator.addDiffEquation(&DPzDt, "Pz",p0.z(), p0.z(), p0.z());
    rkIntegrator.addDiffEquation(&DxDt,  "x", x0.x(), x0.x(), x0.x());
    rkIntegrator.addDiffEquation(&DyDt,  "y", x0.y(), x0.y(), x0.y());
    rkIntegrator.addDiffEquation(&DzDt,  "z", x0.z(), x0.z(), x0.z());
    
   

    _px = (*rkIntegrator.getFunction(0))(CLHEP::c_light*t).clone();
    _py = (*rkIntegrator.getFunction(1))(CLHEP::c_light*t).clone();
    _pz = (*rkIntegrator.getFunction(2))(CLHEP::c_light*t).clone();
    _x  = (*rkIntegrator.getFunction(3))(CLHEP::c_light*t).clone();
    _y  = (*rkIntegrator.getFunction(4))(CLHEP::c_light*t).clone();
    _z  = (*rkIntegrator.getFunction(5))(CLHEP::c_light*t).clone();
  }
}

GeoXPEngine::~GeoXPEngine() {
  delete _x;
  delete _y;
  delete _z;
  delete _px;
  delete _py;
  delete _pz;
}

const Genfun::AbsFunction &  GeoXPEngine::x() const {
  return *_x;
}
const Genfun::AbsFunction &  GeoXPEngine::y() const {
  return *_y;
}
const Genfun::AbsFunction &  GeoXPEngine::z() const {
  return *_z;
}
const Genfun::AbsFunction &  GeoXPEngine::px() const {
  return *_px;
}
const Genfun::AbsFunction &  GeoXPEngine::py() const {
  return *_py;
}
const Genfun::AbsFunction &  GeoXPEngine::pz() const {
  return *_pz;
}
