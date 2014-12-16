/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "GaudiKernel/MsgStream.h"
#include <string>

Trk::ScatteringAngles::ScatteringAngles():
	m_deltaPhi(0.0),
	m_deltaTheta(0.0),
	m_sigmaDeltaPhi(0.0),
	m_sigmaDeltaTheta(0.0)
{}

Trk::ScatteringAngles::ScatteringAngles(
		double deltaPhi, 
		double deltaTheta, 
		double sigmaDeltaPhi, 
		double sigmaDeltaTheta
  ):
  m_deltaPhi(deltaPhi),
  m_deltaTheta(deltaTheta),
  m_sigmaDeltaPhi(sigmaDeltaPhi),
  m_sigmaDeltaTheta(sigmaDeltaTheta)
{}

Trk::ScatteringAngles::~ScatteringAngles()
{
}

// Overload of << operator for MsgStream for debug output
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::ScatteringAngles& saos)
{
	std::string name("ScatteringAngles: ");
	sl << name<<"deltaPhi        : " << saos.deltaPhi()<<endreq;
	sl << name<<"deltaTheta      : " << saos.deltaTheta() <<endreq;
	sl << name<<"sigmaDeltaPhi   : " << saos.sigmaDeltaPhi()<<endreq;
	sl << name<<"sigmaDeltaTheta : " << saos.sigmaDeltaTheta() <<endreq;
	return sl; 
}
// Overload of << operator for std::ostream for debug output
std::ostream& Trk::operator << ( std::ostream& sl, const Trk::ScatteringAngles& saos)
{
	sl << "ScatteringAngles: " <<std::endl;
	sl << "\t deltaPhi        : " << saos.deltaPhi()<<std::endl;
	sl << "\t deltaTheta      : " << saos.deltaTheta() <<std::endl;
	sl << "\t sigmaDeltaPhi   : " << saos.sigmaDeltaPhi()<<std::endl;
	sl << "\t sigmaDeltaTheta : " << saos.sigmaDeltaTheta() <<std::endl;
	return sl; 
}



