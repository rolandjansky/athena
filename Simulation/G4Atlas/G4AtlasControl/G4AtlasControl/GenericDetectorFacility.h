/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericDetectorFacility_H
#define GenericDetectorFacility_H

#include "FadsGeometry/DetectorFacility.h"
#include <string>

class G4LogicalVolume;

class GenericDetectorFacility: public FADS::DetectorFacility {
public:
	GenericDetectorFacility(const std::string);
	void SetEnvelopeVolume(G4LogicalVolume*);
	G4LogicalVolume* GetEnvelopeVolume() const;
	void BuildGeometry();
};

#endif
