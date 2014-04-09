/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ResizeableTrdEnvelope_H
#define ResizeableTrdEnvelope_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

class G4LogicalVolume;

class ResizeableTrdEnvelope: public FADS::DetectorFacility {
public:
	ResizeableTrdEnvelope(G4String);
	void BuildGeometry();
	void SetDx1(double);
	void SetDx2(double);
	void SetDy1(double);
	void SetDy2(double);
	void SetDz(double);
	void SetMaterial(std::string);
private:
	double dx1;
	double dx2;
	double dy1;
	double dy2;   
	double dz;
	G4LogicalVolume *theVolume;
	std::string material;
};
#endif 
