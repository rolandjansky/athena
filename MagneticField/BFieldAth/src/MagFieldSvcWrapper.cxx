/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BFieldAth/MagFieldSvcWrapper.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

// Constructor
MagFieldSvcWrapper::MagFieldSvcWrapper( MagField::IMagFieldSvc& service )
  : m_service(&service)
{
  // copy magnet status from the underlying service
  setSolenoidOn( m_service->solenoidOn() );
  setToroidBarrelOn( m_service->toroidOn() );
  setToroidECTAOn( m_service->toroidOn() );
  setToroidECTCOn( m_service->toroidOn() );
  setAllToroidOn( m_service->toroidOn() );
  setFieldStatusOn( m_service->solenoidOn() && m_service->toroidOn() );
}

// XYZ[3] in cm -> BXYZ[3] in T
void MagFieldSvcWrapper::field_tesla_cm(float* XYZ,float* BXYZ) {
  double xyz[3];
  for( int i=0;i<3;++i ) xyz[i] = 10.*XYZ[i]; // cm -> mm
  double bxyz[3];
  m_service->getField(xyz, bxyz); // mm -> kT
  for( int i=0;i<3;++i ) BXYZ[i] = 1000.*bxyz[i]; // kT -> T
}

// XYZ[3] in cm -> BXYZ[12] in T and T/cm
void MagFieldSvcWrapper::fieldGradient_tesla_cm(float* XYZ,float* BXYZ) {
  double xyz[3];
  for( int i=0;i<3;++i ) xyz[i] = 10.*XYZ[i]; // cm -> mm
  double bxyz[3];
  double deriv[9];
  m_service->getField(xyz, bxyz, deriv); // mm -> kT and kT/mm
  for( int i=0;i<3;++i ) BXYZ[i] = 1000.*bxyz[i]; // kT -> T
  for( int i=0;i<9;++i ) BXYZ[i+3] = 10000.*deriv[i]; // kT/mm -> T/cm
}

// XYZ[3] in mm -> BXYZ[3] in kG
void MagFieldSvcWrapper::field_XYZ_in_mm(float* XYZ,float* BXYZ) {
  double xyz[3];
  for( int i=0;i<3;++i ) xyz[i] = XYZ[i]; // mm -> mm
  double bxyz[3];
  m_service->getField(xyz, bxyz); // mm -> kT
  for( int i=0;i<3;++i ) BXYZ[i] = 10000.*bxyz[i]; // kT -> kG
}

// XYZ[3] in mm -> BXYZ[12] in kG and kG/mm
void MagFieldSvcWrapper::fieldGradient_XYZ_in_mm(float* XYZ,float* BXYZ) {
  double xyz[3];
  for( int i=0;i<3;++i ) xyz[i] = XYZ[i]; // mm -> mm
  double bxyz[3];
  double deriv[9];
  m_service->getField(xyz, bxyz, deriv); // mm -> kT and kT/mm
  for( int i=0;i<3;++i ) BXYZ[i] = 10000.*bxyz[i]; // kT -> kG
  for( int i=0;i<9;++i ) BXYZ[i+3] = 10000.*deriv[i]; // kT/mm -> kG/mm
}

void MagFieldSvcWrapper::whereAmI(float* /*XYZ*/, int& /*iz*/ ){}

const char* MagFieldSvcWrapper::whereAmI(float* /*XYZ*/) { return "here"; }
