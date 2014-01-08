/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyBFStand.h"
#include "BFieldStand/MagFieldFactory.h"

MuonboyBFStand::MuonboyBFStand(AbstractMagneticField* mag){ m_mag_field = mag; }

MuonboyBFStand::~MuonboyBFStand(){}

void MuonboyBFStand::field(float* XYZ,float* BXYZ){

  if (m_mag_field) m_mag_field->field(XYZ,BXYZ);

}
void MuonboyBFStand::fieldd(float* XYZ,float* BXYZ){

  if (m_mag_field) m_mag_field->fieldGradient(XYZ,BXYZ);

}

void MuonboyBFStand::field_tesla_cm(float* XYZ,float* BXYZ){

  if (m_mag_field) m_mag_field->field_tesla_cm(XYZ,BXYZ);

}
void MuonboyBFStand::fieldGradient_tesla_cm(float* XYZ,float* BXYZ){

  if (m_mag_field) m_mag_field->fieldGradient_tesla_cm(XYZ,BXYZ);

}
