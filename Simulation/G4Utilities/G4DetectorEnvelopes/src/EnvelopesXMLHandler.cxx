/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include "G4DetectorEnvelopes/EnvelopesXMLHandler.h"
#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

EnvelopesXMLHandler::EnvelopesXMLHandler(std::string s,EnvelopeGeometryManager *gm):
		 DescriptionFactory(s),geometryManager(gm)
{
}

void EnvelopesXMLHandler::BuildDescription()
{
std::cout <<"this is EnvelopesXMLHandler. handler called "<<std::endl;

std::cout
<<GetAttributeAsDouble("AtlasInnerRadius")<<" "
<<GetAttributeAsDouble("AtlasOuterRadius")<<" "
<<GetAttributeAsDouble("AtlasZMax")<<std::endl; 
	double x;
	if (CheckAttribute("AtlasInnerRadius"))
	{
		x=GetAttributeAsDouble("AtlasInnerRadius");
		geometryManager->parameters.atlasInnerRadius=x;
	}
	if (CheckAttribute("AtlasOuterRadius"))
	{
		x=GetAttributeAsDouble("AtlasOuterRadius");
		geometryManager->parameters.atlasOuterRadius=x;
	}
	if (CheckAttribute("AtlasZMax"))
	{
		x=GetAttributeAsDouble("AtlasZMax");
		geometryManager->parameters.atlasMaxZ=x;
	}
	
	if (CheckAttribute("IdetInnerRadius"))
	{
		x=GetAttributeAsDouble("IdetInnerRadius");
		geometryManager->parameters.idetInnerRadius=x;
	}
	if (CheckAttribute("IdetOuterRadius"))
	{
		x=GetAttributeAsDouble("IdetOuterRadius");
		geometryManager->parameters.idetOuterRadius=x;
	}
	if (CheckAttribute("IdetZMax"))
	{
		x=GetAttributeAsDouble("IdetZMax");
		geometryManager->parameters.idetMaxZ=x;
	}
	
	if (CheckAttribute("CaloInnerRadius"))
	{
		x=GetAttributeAsDouble("CaloInnerRadius");
		geometryManager->parameters.caloInnerRadius=x;
	}
	if (CheckAttribute("CaloOuterRadius"))
	{
		x=GetAttributeAsDouble("CaloOuterRadius");
		geometryManager->parameters.caloOuterRadius=x;
	}
	if (CheckAttribute("CaloZMax"))
	{
		x=GetAttributeAsDouble("CaloZMax");
		geometryManager->parameters.caloMaxZ=x;
	}
	
	if (CheckAttribute("MuonInnerRadius"))
	{
		x=GetAttributeAsDouble("MuonInnerRadius");
		geometryManager->parameters.muonInnerRadius=x;
	}
	if (CheckAttribute("MuonOuterRadius"))
	{
		x=GetAttributeAsDouble("MuonOuterRadius");
		geometryManager->parameters.muonOuterRadius=x;
	}
	if (CheckAttribute("MuonZMax"))
	{
		x=GetAttributeAsDouble("MuonZMax");
		geometryManager->parameters.muonMaxZ=x;
	}
}
