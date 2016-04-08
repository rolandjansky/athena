/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ConstantField_H
#define ConstantField_H

#include "FadsField/MagneticFieldMap.h"
#include "G4Field/ConstantFieldXMLHandler.h"
// Geant4 includes
#include "G4SystemOfUnits.hh"

#include <iostream>

class ConstantField : public FADS::MagneticFieldMap 
{
public:
	ConstantField();
	ConstantField(std::string);
	~ConstantField();
	
	void FieldValue(const double *xyzPos, double *bfield) const;
	void Initialize();
	void SetXComponent(double xf) 
	{
		std::cout<<"Setting X component for field "<<GetName()<< 
			" to "<<xf/CLHEP::tesla<<" Tesla"<<std::endl;
		FieldValueX=xf;
	}
	void SetYComponent(double yf) 
	{
		std::cout<<"Setting Y component for field "<<GetName()<< 
			" to "<<yf/CLHEP::tesla<<" Tesla"<<std::endl;
		FieldValueY=yf;
	}
	void SetZComponent(double zf) 
	{
		std::cout<<"Setting Z component for field "<<GetName()<< 
			" to "<<zf/CLHEP::tesla<<" Tesla"<<std::endl;
		FieldValueZ=zf;
	}
	void SetXMinField(double xm) {checkXdim=true;xMinField=xm;}
	void SetYMinField(double ym) {checkYdim=true;yMinField=ym;}
	void SetZMinField(double zm) {checkZdim=true;zMinField=zm;}
	void SetXMaxField(double xm) {checkXdim=true;xMaxField=xm;}
	void SetYMaxField(double ym) {checkYdim=true;yMaxField=ym;}
	void SetZMaxField(double zm) {checkZdim=true;zMaxField=zm;}
	
private:
	ConstantFieldXMLHandler *theHandler;
	double FieldValueX;
	double FieldValueY;
	double FieldValueZ;
	
	bool checkXdim;
	bool checkYdim;
	bool checkZdim;
	double xMinField;
	double xMaxField;
	double yMinField;
	double yMaxField;
	double zMinField;
	double zMaxField;
};

#endif
