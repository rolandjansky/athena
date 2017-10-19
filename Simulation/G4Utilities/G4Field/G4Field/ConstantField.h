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
		m_FieldValueX=xf;
	}
	void SetYComponent(double yf) 
	{
		std::cout<<"Setting Y component for field "<<GetName()<< 
			" to "<<yf/CLHEP::tesla<<" Tesla"<<std::endl;
		m_FieldValueY=yf;
	}
	void SetZComponent(double zf) 
	{
		std::cout<<"Setting Z component for field "<<GetName()<< 
			" to "<<zf/CLHEP::tesla<<" Tesla"<<std::endl;
		m_FieldValueZ=zf;
	}
	void SetXMinField(double xm) {m_checkXdim=true;m_xMinField=xm;}
	void SetYMinField(double ym) {m_checkYdim=true;m_yMinField=ym;}
	void SetZMinField(double zm) {m_checkZdim=true;m_zMinField=zm;}
	void SetXMaxField(double xm) {m_checkXdim=true;m_xMaxField=xm;}
	void SetYMaxField(double ym) {m_checkYdim=true;m_yMaxField=ym;}
	void SetZMaxField(double zm) {m_checkZdim=true;m_zMaxField=zm;}
	
private:
	ConstantFieldXMLHandler *m_theHandler;
	double m_FieldValueX;
	double m_FieldValueY;
	double m_FieldValueZ;
	
	bool m_checkXdim;
	bool m_checkYdim;
	bool m_checkZdim;
	double m_xMinField;
	double m_xMaxField;
	double m_yMinField;
	double m_yMaxField;
	double m_zMinField;
	double m_zMaxField;
};

#endif
