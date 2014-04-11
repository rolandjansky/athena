/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ResizeableBoxEnvelope_H
#define ResizeableBoxEnvelope_H

#include "FadsGeometry/DetectorFacility.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

class G4LogicalVolume;

typedef std::map<std::string, G4ThreeVector, std::less<std::string> > HoleSize;
typedef std::map<std::string, G4ThreeVector, std::less<std::string> > HolePosition;

class ResizeableBoxEnvelope: public FADS::DetectorFacility {
public:
	ResizeableBoxEnvelope(G4String);
	void BuildGeometry();
	void SetDx(double);
	void SetDy(double);
	void SetDz(double);
	void SetMaterial(std::string);
	void DefineSubtractionBox(std::string);
	void SetSubtractionBoxPosition(std::string, G4ThreeVector&);
	void SetSubtractionBoxDimensions(std::string, G4ThreeVector&);
private:
	double dx;
	double dy;   
	double dz;
	G4LogicalVolume *theVolume;
	std::string material;
	HoleSize holes;
	HolePosition holesPos;
};
#endif 
