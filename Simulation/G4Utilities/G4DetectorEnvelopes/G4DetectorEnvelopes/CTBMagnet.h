/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CTBMagnet_H
#define CTBMagnet_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>
#include <string>
class G4LogicalVolume;

class CTBMagnet: public FADS::DetectorFacility {
public:
	CTBMagnet(G4String);
	void BuildGeometry();
	void SetDx(double);
	void SetDy(double);
	void SetDz(double);
	void SetBoreDx(double);
	void SetBoreDy(double);
	void SetMaterial(std::string);
private:
	double dx;
	double dy;   
	double dz;
	double boreDx;
	double boreDy;
	G4LogicalVolume *theVolume;
	std::string material;
};
#endif 
