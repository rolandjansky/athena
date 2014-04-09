/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ResizeableTubsEnvelope_H
#define ResizeableTubsEnvelope_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>
#include <string>
class G4LogicalVolume;

class ResizeableTubsEnvelope: public FADS::DetectorFacility {
public:
	ResizeableTubsEnvelope(G4String);
	void BuildGeometry();
	void SetRmin(double);
	void SetRmax(double);
	void SetDz(double);
	void SetStartPhi(double);
	void SetDeltaPhi(double);
	void SetMaterial(std::string);
private:
	double rmin;
	double rmax;   
	double dz;
	double startphi;
	double deltaphi;
	G4LogicalVolume *theVolume;
	std::string material;
};
#endif 
