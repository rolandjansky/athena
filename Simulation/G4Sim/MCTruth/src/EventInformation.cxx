/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/EventInformation.h"

HepMC::GenEvent* EventInformation::GetHepMCEvent()
{
	return theEvent;
}
void EventInformation::SetHepMCEvent(HepMC::GenEvent* ev)
{
	theEvent=ev;
}
int EventInformation::GetNrOfPrimaryParticles() const
{
	return nrOfPrimaryParticles;
}
void EventInformation::SetNrOfPrimaryParticles(int nr)
{
	nrOfPrimaryParticles=nr;
}
int EventInformation::GetNrOfPrimaryVertices() const
{
	return nrOfPrimaryVertices;
}
void EventInformation::SetNrOfPrimaryVertices(int nr)
{
	nrOfPrimaryVertices=nr;
}
void EventInformation::SetVertexPosition(const G4ThreeVector vtx)
{
	vertexPosition=vtx;
}
const G4ThreeVector EventInformation::GetVertexPosition() const
{
	return vertexPosition;
}
