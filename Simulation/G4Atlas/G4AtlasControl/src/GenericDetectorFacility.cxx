/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/GenericDetectorFacility.h"
#include "G4LogicalVolume.hh"
#include <iostream>

GenericDetectorFacility::GenericDetectorFacility(const std::string s): FADS::DetectorFacility(s)
{
	std::cout<<"GenericDetectorFacility being instantiated for "
		 <<"detector "<<s<<std::endl;
}
void GenericDetectorFacility::SetEnvelopeVolume(G4LogicalVolume *lv)
{
	SetDetectorEnvelope(lv);
}
G4LogicalVolume* GenericDetectorFacility::GetEnvelopeVolume() const
{
	return envelope.theEnvelope;
}
void GenericDetectorFacility::BuildGeometry()
{
	G4LogicalVolume* temp=GetEnvelopeVolume();
	if (temp==0)
	{
		std::cout<<"Warning! Envelope for detector "<<
		GetName()<<" not set!! this might cause a crash!"
		<<std::endl;
	}
}
