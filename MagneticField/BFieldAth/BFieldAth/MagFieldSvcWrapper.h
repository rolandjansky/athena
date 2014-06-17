/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDSVCWRAPPER_h
#define MAGFIELDSVCWRAPPER_h
#include "BFieldCore/AbstractMagneticField.h"
	
namespace MagField {
  class IMagFieldSvc;
}

class MagFieldSvcWrapper: public AbstractMagneticField {
 public:
  // Constructor
  MagFieldSvcWrapper( MagField::IMagFieldSvc& service );
  // Destructor
  virtual ~MagFieldSvcWrapper() {}
 public:
  // XYZ in cm => BXYZ in Tesla (Gradient Telsa/cm)
  virtual void field_tesla_cm(float* XYZ,float* BXYZ);
  virtual void fieldGradient_tesla_cm(float* XYZ,float* BXYZ);
  // XYZ in mm => BXYZ in KiloGauss (G4) (Gradient kiloGauss/cm)
  virtual void field_XYZ_in_mm(float* XYZ,float* BXYZ);
  virtual void fieldGradient_XYZ_in_mm(float* XYZ,float* BXYZ);
  // set Tilts extern parameters of b-field axis dx,dy,dz,dtheta,dphi,dpsi
  void setSolenoidTilts( float* Dxyz_i, double theta, double phi, double psi );
  // Where am I with respect to the Magnetic Field calculation
  virtual void whereAmI(float* XYZ, int &iz); 
  virtual const char * whereAmI(float* XYZ); 

 private:
  MagField::IMagFieldSvc* m_service;

};
#endif
