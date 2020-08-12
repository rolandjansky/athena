/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/AtlasG4EventUserInfo.h"

HepMC::GenEvent* AtlasG4EventUserInfo::GetHepMCEvent()
{
	return m_theEvent;
}
void AtlasG4EventUserInfo::SetHepMCEvent(HepMC::GenEvent* ev)
{
        m_theEvent=ev;
}
int AtlasG4EventUserInfo::GetNrOfPrimaryParticles() const
{
	return m_nrOfPrimaryParticles;
}
void AtlasG4EventUserInfo::SetNrOfPrimaryParticles(int nr)
{
	m_nrOfPrimaryParticles=nr;
}
int AtlasG4EventUserInfo::GetNrOfPrimaryVertices() const
{
	return m_nrOfPrimaryVertices;
}
void AtlasG4EventUserInfo::SetNrOfPrimaryVertices(int nr)
{
	m_nrOfPrimaryVertices=nr;
}
void AtlasG4EventUserInfo::SetVertexPosition(const G4ThreeVector vtx)
{
	m_vertexPosition=vtx;
}
const G4ThreeVector AtlasG4EventUserInfo::GetVertexPosition() const
{
	return m_vertexPosition;
}
