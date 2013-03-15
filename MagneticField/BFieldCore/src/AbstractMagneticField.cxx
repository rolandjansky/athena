/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  17/04/00
//
// AbstractMagneticField is a class providing Magnetic Field
//
#include "BFieldCore/AbstractMagneticField.h"
#include <cmath>
 
AbstractMagneticField::AbstractMagneticField(){m_lock  = 0;}
AbstractMagneticField::~AbstractMagneticField(){}
void AbstractMagneticField::setLock(){m_lock =1;}

void AbstractMagneticField::field(float* XYZ,float* BXYZ) {
  field_tesla_cm(XYZ, BXYZ);
  for (int i=0; i<3; i++) BXYZ[i] *= 10.;
}
void AbstractMagneticField::field_tesla_mm(float* XYZ,float* BXYZ) {
  float XYZ2[3];
  for (int i=0; i<3; i++) XYZ2[i] = XYZ[i]*0.1;
  field_tesla_cm(XYZ2, BXYZ);
}
void AbstractMagneticField::fieldGradient(float* XYZ,float* BXYZ) {
  fieldGradient_tesla_cm(XYZ, BXYZ);
  for (int i=0; i<12; i++) BXYZ[i] *= 10.;
}
// Magnetic Field status
void AbstractMagneticField::setToroidBarrelOn(bool state){toroidBarrelOn=state;}
bool AbstractMagneticField::getToroidBarrelOn() const {return toroidBarrelOn ;}

void AbstractMagneticField::setToroidECTAOn(bool state){toroidECTAOn=state;}
bool AbstractMagneticField::getToroidECTAOn() const {return toroidECTAOn ;}

void AbstractMagneticField::setToroidECTCOn(bool state){toroidECTCOn=state;}
bool AbstractMagneticField::getToroidECTCOn() const {return toroidECTCOn ;}

void AbstractMagneticField::setAllToroidOn(bool state){allToroidOn=state;}
bool AbstractMagneticField::getAllToroidOn() const {return allToroidOn ;}

void AbstractMagneticField::setSolenoidOn(bool state){solenoidOn=state;}
bool AbstractMagneticField::getSolenoidOn() const {return solenoidOn ;}

void AbstractMagneticField::setFieldStatusOn(bool state){fieldStatusOn=state;}
bool AbstractMagneticField::getFieldStatusOn() const {return fieldStatusOn ;}
