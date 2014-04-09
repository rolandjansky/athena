/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EnvelopeGeometryManager_H
#define EnvelopeGeometryManager_H

#include "G4DetectorEnvelopes/EnvelopesXMLHandler.h"

struct EnvelopeParameters {
	double atlasInnerRadius;
	double atlasOuterRadius;
	double atlasMaxZ;
	double idetInnerRadius;
	double idetOuterRadius;
	double idetMaxZ;
	double caloInnerRadius;
	double caloOuterRadius;
	double caloMaxZ;
	double muonInnerRadius;
	double muonOuterRadius;
	double muonMaxZ;
};

class EnvelopeGeometryManager {
	friend class EnvelopesXMLHandler;
private:
	static EnvelopeGeometryManager* thePointer;
	EnvelopeParameters parameters;
	inline void SetParameters();
	EnvelopesXMLHandler *handler;

	EnvelopeGeometryManager()
	{
		SetParameters();
		handler=new EnvelopesXMLHandler("EnvelopeParameters",this);
	}
	EnvelopeGeometryManager(const EnvelopeGeometryManager&) {}
public:
	static EnvelopeGeometryManager* GetGeometryManager()
	{
		if (!thePointer) thePointer=new EnvelopeGeometryManager;
		return thePointer;
	}
	~EnvelopeGeometryManager() {delete handler;}
	inline double AtlasInnerRadius() const;
	inline double AtlasOuterRadius() const;
	inline double AtlasMaxZ() const;
	inline double IdetInnerRadius() const;
	inline double IdetOuterRadius() const;
	inline double IdetMaxZ() const;
	inline double CaloInnerRadius() const;
	inline double CaloOuterRadius() const;
	inline double CaloMaxZ() const;
	inline double MuonInnerRadius() const;
	inline double MuonOuterRadius() const;
	inline double MuonMaxZ() const;
};

double EnvelopeGeometryManager::AtlasInnerRadius() const
{
	return parameters.atlasInnerRadius;
}
double EnvelopeGeometryManager::AtlasOuterRadius() const
{
	return parameters.atlasOuterRadius;
}
double EnvelopeGeometryManager::AtlasMaxZ() const
{
	return parameters.atlasMaxZ;
}
double EnvelopeGeometryManager::IdetInnerRadius() const
{
	return parameters.idetInnerRadius;
}
double EnvelopeGeometryManager::IdetOuterRadius() const
{
	return parameters.idetOuterRadius;
}
double EnvelopeGeometryManager::IdetMaxZ() const
{
	return parameters.idetMaxZ;
}
double EnvelopeGeometryManager::CaloInnerRadius() const
{
	return parameters.caloInnerRadius;	
}
double EnvelopeGeometryManager::CaloOuterRadius() const
{
	return parameters.caloOuterRadius;
}
double EnvelopeGeometryManager::CaloMaxZ() const
{
	return parameters.caloMaxZ;
}
double EnvelopeGeometryManager::MuonInnerRadius() const
{
	return parameters.muonInnerRadius;
}
double EnvelopeGeometryManager::MuonOuterRadius() const
{
	return parameters.muonOuterRadius;
}
double EnvelopeGeometryManager::MuonMaxZ() const
{
	return parameters.muonMaxZ;
}
void EnvelopeGeometryManager::SetParameters()
{
	const double cm=10.;

    parameters.atlasInnerRadius=0*cm;
    parameters.atlasOuterRadius=4000*cm;
    parameters.atlasMaxZ=3000*cm;
    parameters.idetInnerRadius=3.5*cm;
    parameters.idetOuterRadius=115*cm;
    parameters.idetMaxZ=349*cm;
    parameters.caloInnerRadius=4*cm;
    parameters.caloOuterRadius=425*cm;
    parameters.caloMaxZ=674*cm;
    parameters.muonInnerRadius=6*cm;
    parameters.muonOuterRadius=3000*cm;
    parameters.muonMaxZ=3000*cm;

}

#endif
