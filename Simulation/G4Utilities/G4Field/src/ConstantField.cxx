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
	checkXdim=checkYdim=checkZdim=false;
	xMinField=yMinField=zMinField=-100*CLHEP::km;
	xMaxField=yMaxField=zMaxField=100*CLHEP::km;
	theHandler=new ConstantFieldXMLHandler(GetName(),this);
	std::cout<<" Constant field "<<GetName()<<"being initialised "<<std::endl;
	FieldValueX=0;
	FieldValueY=0;
	FieldValueZ=2.*CLHEP::tesla;
}

ConstantField::ConstantField(std::string n):FADS::MagneticFieldMap(n)
{
	checkXdim=checkYdim=checkZdim=false;
	xMinField=yMinField=zMinField=-100*CLHEP::km;
	xMaxField=yMaxField=zMaxField=100*CLHEP::km;
	theHandler=new ConstantFieldXMLHandler(n,this);
	std::cout<<" Constant field "<<n<<"being initialised "<<std::endl;
	FieldValueX=0;
	FieldValueY=0;
	FieldValueZ=2.*CLHEP::tesla;
}

ConstantField::~ConstantField()
{
	delete theHandler;
}

void ConstantField::Initialize()
{
	std::cout<<" Constant field "<<GetName()<<"initialized. Field components: "<<
		FieldValueX/CLHEP::tesla<<" "<<FieldValueY/CLHEP::tesla<<" "<<FieldValueZ/CLHEP::tesla<<
		" (field components in Tesla)"<<std::endl;
}

void ConstantField::FieldValue(const double *pos, double *bfield) const
{

  // check whether we need to set the field to zero
  bool setZero = checkXdim && (pos[0]<xMinField || pos[0]>xMaxField);
  setZero     |= checkYdim && (pos[1]<yMinField || pos[1]>yMaxField);
  setZero     |= checkZdim && (pos[2]<zMinField || pos[2]>zMaxField);

  if (setZero) {
    for (int i=0; i<3; i++) bfield[i] = 0.;
  } else {
    bfield[0] = FieldValueX;
    bfield[1] = FieldValueY;
    bfield[2] = FieldValueZ;
  }

	return;
}
