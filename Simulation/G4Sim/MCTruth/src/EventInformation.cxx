/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/EventInformation.h"

HepMC::GenEvent* EventInformation::GetHepMCEvent()
{
	return m_theEvent;
}
void EventInformation::SetHepMCEvent(HepMC::GenEvent* ev)
{
	m_theEvent=ev;
}
int EventInformation::GetNrOfPrimaryParticles() const
{
	return m_nrOfPrimaryParticles;
}
void EventInformation::SetNrOfPrimaryParticles(int nr)
{
	m_nrOfPrimaryParticles=nr;
}
int EventInformation::GetNrOfPrimaryVertices() const
{
	return m_nrOfPrimaryVertices;
}
void EventInformation::SetNrOfPrimaryVertices(int nr)
{
	m_nrOfPrimaryVertices=nr;
}
void EventInformation::SetVertexPosition(const G4ThreeVector vtx)
{
	m_vertexPosition=vtx;
}
const G4ThreeVector EventInformation::GetVertexPosition() const
{
	return m_vertexPosition;
}
