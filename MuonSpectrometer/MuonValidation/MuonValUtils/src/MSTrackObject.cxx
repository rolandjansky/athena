/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Author:   M.Schott(LMU)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class is derivated from MSPhysics Object
//		The main idea is, that a MSTrackObject can have several
//		assoziated MSPhysicsObjects.
//		This makes it easy to study reconstruction efficiencies and
//		Tag&Probe approach since we can assoziate inner Tracks to
//		outer tracks
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "MuonValUtils/MSTrackObject.h"


MSTrackObject::MSTrackObject()
{
	clear();
}

// Copy constructor
MSTrackObject::MSTrackObject(const MSTrackObject& ob)
  : MSPhysicsObject (ob)
{
	MSPhysicsObject::operator=(ob);

	Type			= ob.Type;
	Hits			= ob.Hits;
	Chi2			= ob.Chi2;
	Chi2oDOF		= ob.Chi2oDOF;
	EnergyLoss		= ob.EnergyLoss;
	m_TruthAssoziation	= ob.m_TruthAssoziation;
	m_TrackAssoziation	= ob.m_TrackAssoziation;
	m_TriggerAssoziation	= ob.m_TriggerAssoziation;

	
}

// Overload =
void MSTrackObject::operator = (const MSTrackObject& ob)
{
	MSPhysicsObject::operator=(ob);

	Type			= ob.Type;
	Hits			= ob.Hits;
	Chi2			= ob.Chi2;
	Chi2oDOF		= ob.Chi2oDOF;
	EnergyLoss		= ob.EnergyLoss;
	m_TruthAssoziation	= ob.m_TruthAssoziation;
	m_TrackAssoziation	= ob.m_TrackAssoziation;
	m_TriggerAssoziation	= ob.m_TriggerAssoziation;

}


void MSTrackObject::clear()
{
	MSPhysicsObject::clear();
	Type			= -1;
	Hits			= -1;
	Chi2			= -1.0;
	Chi2oDOF		= -1.0;
	EnergyLoss		= -1.0;
	m_TruthAssoziation.clear();
	m_TrackAssoziation.clear();
	m_TriggerAssoziation.clear();
}

void	MSTrackObject::setAssoziatedTruthInformation(MSPhysicsObject &ob)
{
	m_TruthAssoziation["MCTruth"]=ob;
}


bool	MSTrackObject::getAssoziatedTruthInformation(MSPhysicsObject &ob)
{
	if (m_TruthAssoziation.find("MCTruth") == m_TruthAssoziation.end())	return false;
	ob=m_TruthAssoziation["MCTruth"];
	return true;
}


void	MSTrackObject::setAssoziatedTrackInformation(string ContainerID, MSPhysicsObject &ob)
{
	m_TrackAssoziation[ContainerID]=ob;
}


bool	MSTrackObject::getAssoziatedTrackInformation(string ContainerID, MSPhysicsObject &ob)
{
	if (m_TrackAssoziation.find(ContainerID) == m_TrackAssoziation.end())	return false;
	ob=m_TrackAssoziation[ContainerID];
	return true;
}


void	MSTrackObject::setAssoziatedTriggerInformation(string ContainerID, MSPhysicsObject &ob)
{
	m_TriggerAssoziation[ContainerID]=ob;
}


bool	MSTrackObject::getAssoziatedTriggerInformation(string ContainerID, MSPhysicsObject &ob)
{
	if (m_TriggerAssoziation.find(ContainerID) == m_TriggerAssoziation.end())	return false;
	ob=m_TriggerAssoziation[ContainerID];
	return true;
}






