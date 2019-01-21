/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoXPEngine.h"
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
    m_x= (Genfun::FixedConstant(x0.x()) + (CLHEP::c_light*p0.x()/p0.vect().mag())*t).clone();
    m_y= (Genfun::FixedConstant(x0.y()) + (CLHEP::c_light*p0.y()/p0.vect().mag())*t).clone();
    m_z= (Genfun::FixedConstant(x0.z()) + (CLHEP::c_light*p0.z()/p0.vect().mag())*t).clone();
    m_px  = Genfun::FixedConstant(p0.x()).clone();
    m_py  = Genfun::FixedConstant(p0.y()).clone();
    m_pz  = Genfun::FixedConstant(p0.z()).clone();
    

  }
  else {

    Genfun::RKIntegrator rkIntegrator;
    Genfun::Variable Px(0,6),Py(1,6), Pz(2,6), X(3,6), Y(4,6), Z(5,6);
    Genfun::FixedConstant I(1.0);
    
    const double inv_E = 1. / E;
    Genfun::GENFUNCTION DPxDt  = (q*inv_E)*(Py*(I%I%I%Bz) -Pz*(I%I%I%By))*CLHEP::c_light;
    Genfun::GENFUNCTION DPyDt  = (q*inv_E)*(Pz*(I%I%I%Bx) -Px*(I%I%I%Bz))*CLHEP::c_light;
    Genfun::GENFUNCTION DPzDt  = (q*inv_E)*(Px*(I%I%I%By) -Py*(I%I%I%Bx))*CLHEP::c_light;
    Genfun::GENFUNCTION DxDt   = Px*inv_E;
    Genfun::GENFUNCTION DyDt   = Py*inv_E;
    Genfun::GENFUNCTION DzDt   = Pz*inv_E;
    
    rkIntegrator.addDiffEquation(&DPxDt, "Px",p0.x(), p0.x(), p0.x());
    rkIntegrator.addDiffEquation(&DPyDt, "Py",p0.y(), p0.y(), p0.y());
    rkIntegrator.addDiffEquation(&DPzDt, "Pz",p0.z(), p0.z(), p0.z());
    rkIntegrator.addDiffEquation(&DxDt,  "x", x0.x(), x0.x(), x0.x());
    rkIntegrator.addDiffEquation(&DyDt,  "y", x0.y(), x0.y(), x0.y());
    rkIntegrator.addDiffEquation(&DzDt,  "z", x0.z(), x0.z(), x0.z());
    
   

    m_px = (*rkIntegrator.getFunction(0))(CLHEP::c_light*t).clone();
    m_py = (*rkIntegrator.getFunction(1))(CLHEP::c_light*t).clone();
    m_pz = (*rkIntegrator.getFunction(2))(CLHEP::c_light*t).clone();
    m_x  = (*rkIntegrator.getFunction(3))(CLHEP::c_light*t).clone();
    m_y  = (*rkIntegrator.getFunction(4))(CLHEP::c_light*t).clone();
    m_z  = (*rkIntegrator.getFunction(5))(CLHEP::c_light*t).clone();
  }
}

GeoXPEngine::~GeoXPEngine() {
  delete m_x;
  delete m_y;
  delete m_z;
  delete m_px;
  delete m_py;
  delete m_pz;
}

const Genfun::AbsFunction &  GeoXPEngine::x() const {
  return *m_x;
}
const Genfun::AbsFunction &  GeoXPEngine::y() const {
  return *m_y;
}
const Genfun::AbsFunction &  GeoXPEngine::z() const {
  return *m_z;
}
const Genfun::AbsFunction &  GeoXPEngine::px() const {
  return *m_px;
}
const Genfun::AbsFunction &  GeoXPEngine::py() const {
  return *m_py;
}
const Genfun::AbsFunction &  GeoXPEngine::pz() const {
  return *m_pz;
}
