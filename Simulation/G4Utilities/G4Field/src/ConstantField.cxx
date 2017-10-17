/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4Field/ConstantField.h"
#include <iostream>
#include "FadsField/MagneticFieldMapT.h"
#include "G4Field/ConstantFieldXMLHandler.h"
// Geant4 includes
#include "G4ThreeVector.hh"

static FADS::MagneticFieldMapT<ConstantField> id("ConstantField");

ConstantField::ConstantField()
{
	m_checkXdim=m_checkYdim=m_checkZdim=false;
	m_xMinField=m_yMinField=m_zMinField=-100*CLHEP::km;
	m_xMaxField=m_yMaxField=m_zMaxField=100*CLHEP::km;
	m_theHandler=new ConstantFieldXMLHandler(GetName(),this);
	std::cout<<" Constant field "<<GetName()<<"being initialised "<<std::endl;
	m_FieldValueX=0;
	m_FieldValueY=0;
	m_FieldValueZ=2.*CLHEP::tesla;
}

ConstantField::ConstantField(std::string n):FADS::MagneticFieldMap(n)
{
	m_checkXdim=m_checkYdim=m_checkZdim=false;
	m_xMinField=m_yMinField=m_zMinField=-100*CLHEP::km;
	m_xMaxField=m_yMaxField=m_zMaxField=100*CLHEP::km;
	m_theHandler=new ConstantFieldXMLHandler(n,this);
	std::cout<<" Constant field "<<n<<"being initialised "<<std::endl;
	m_FieldValueX=0;
	m_FieldValueY=0;
	m_FieldValueZ=2.*CLHEP::tesla;
}

ConstantField::~ConstantField()
{
	delete m_theHandler;
}

void ConstantField::Initialize()
{
	std::cout<<" Constant field "<<GetName()<<"initialized. Field components: "<<
		m_FieldValueX/CLHEP::tesla<<" "<<m_FieldValueY/CLHEP::tesla<<" "<<m_FieldValueZ/CLHEP::tesla<<
		" (field components in Tesla)"<<std::endl;
}

void ConstantField::FieldValue(const double *pos, double *bfield) const
{

  // check whether we need to set the field to zero
  bool setZero = m_checkXdim && (pos[0]<m_xMinField || pos[0]>m_xMaxField);
  setZero     |= m_checkYdim && (pos[1]<m_yMinField || pos[1]>m_yMaxField);
  setZero     |= m_checkZdim && (pos[2]<m_zMinField || pos[2]>m_zMaxField);

  if (setZero) {
    for (int i=0; i<3; i++) bfield[i] = 0.;
  } else {
    bfield[0] = m_FieldValueX;
    bfield[1] = m_FieldValueY;
    bfield[2] = m_FieldValueZ;
  }

	return;
}
