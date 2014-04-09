/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericPGONEnvelope_H
#define GenericPGONEnvelope_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>
#include <string>
class G4LogicalVolume;

class GenericPGONEnvelope: public FADS::DetectorFacility {
public:
	GenericPGONEnvelope(G4String);
	void BuildGeometry();
	void SetMaterial(std::string);
	void SetNSides(int i) {nsides=i;}
	void SetNSurfaces(int i) {nsurfaces=i;}
	void SetPhiMin(double p) {phimin=p;}
	void SetDeltaPhi(double p) {deltaphi=p;}
	void SetZSurface(int i,double z) 
	{
		assert (i>=0 && (unsigned int)i<nsurfaces);
		if (!z_surface) z_surface=new double[nsurfaces];
		*(z_surface+i)=z;
	}
	void SetRinSurface(int i,double r) 
	{
		assert (i>=0 && (unsigned int)i<nsurfaces);
		if (!rin_surface) rin_surface=new double[nsurfaces];
		*(rin_surface+i)=r;
	}
	void SetRouSurface(int i,double r) 
	{
		assert (i>=0 && (unsigned int)i<nsurfaces);
		if (!rou_surface) rou_surface=new double[nsurfaces];
		*(rou_surface+i)=r;
	}
private:
	unsigned int nsurfaces;
	unsigned int nsides;
	double phimin;
	double deltaphi;
	double *z_surface;
	double *rin_surface;
	double *rou_surface;
	
	G4LogicalVolume *theVolume;
	std::string material;
};
#endif 
