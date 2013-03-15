/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  18/05/00
//
// AbstractMagneticField is a class providing Magnetic Field
//
#ifndef ABSTRACTMAGNETICFIELD
#define ABSTRACTMAGNETICFIELD

#include "BFieldCore/config.h"

#include "BFieldCore/MagFieldMisc.h"

class AbstractMagneticField{
public:
   AbstractMagneticField();
   virtual ~AbstractMagneticField();

public:

// XYZ in cm Get BXYZ in Kilogauss 
   void field(float* XYZ,float* BXYZ);
// XYZ in mm Get BXYZ in Tesla 
   void field_tesla_mm(float* XYZ,float* BXYZ);
// XYZ in cm Get BXYZ in Tesla 
   virtual void field_tesla_cm(float* XYZ,float* BXYZ) = 0 ;
// XYZ in cm Get gradient in kilogauss/cm
   void fieldGradient(float* XYZ,float* BXYZ);
// XYZ in cm Get gradient in Tesla/cm
   virtual void fieldGradient_tesla_cm(float* XYZ,float* BXYZ) = 0 ;
// Get XYZ in mm / BXYZ in kilogauss gradient in kilogauss/mm
   virtual void field_XYZ_in_mm(float* XYZ,float* BXYZ) = 0 ;
   virtual void fieldGradient_XYZ_in_mm(float* XYZ,float* BXYZ) = 0 ;
// Where I am with respect to the Magnetic Field calculation
   virtual void whereAmI(float* XYZ, int &iz) = 0; 
   virtual const char * whereAmI(float* XYZ) = 0; 
// Only one Tilted setting
   virtual void setLock();
// Magnetic field Geometry
   enum Ou {solenoid, betweenSandT, toroid, OutCalculation};
// Magnetic Field status
   void setToroidBarrelOn(bool state);
   bool getToroidBarrelOn() const;
   void setToroidECTAOn(bool state);
   bool getToroidECTAOn() const;
   void setToroidECTCOn(bool state);
   bool getToroidECTCOn() const;
   void setSolenoidOn(bool state);
   bool getSolenoidOn() const;
   void setAllToroidOn(bool state);
   bool getAllToroidOn() const;
   void setFieldStatusOn(bool state);
   bool getFieldStatusOn() const;

protected:
   int    m_lock;
   float  Dxyz[3];
   double theta_i,phi_i,psi_i;
   double cdtheta,cdphi,sdtheta,sdphi;
/* psi(roll)   theta(pitch)    phi(yaw)  */
   double cos_psi,   sin_psi ;  
   double cos_theta, sin_theta ;
   double cos_phi,   sin_phi   ;
   double rot[4][4];/* Rotation matrix = Rx   x Ry   x Rz   */
   double tor[4][4];/* Rotation matrix = Rz-1 x Ry-1 x Rx-1 */
//
   bool toroidBarrelOn;
   bool toroidECTAOn  ;
   bool toroidECTCOn  ;
   bool allToroidOn   ;
   bool solenoidOn    ;
   bool fieldStatusOn ;
};
#endif

