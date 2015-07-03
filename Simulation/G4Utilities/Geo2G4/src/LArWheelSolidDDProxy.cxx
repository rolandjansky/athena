/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArWheelSolidDDProxy
// proxy for LArWheelSolid to store in DetectorDescription
// Author: D. A. Maximov

#include "LArWheelSolidDDProxy.h"
#include "LArWheelSolid.h"
#include<iostream>

LArWheelSolidDDProxy::LArWheelSolidDDProxy(LArWheelSolid* plws) :
	m_plws(plws)
	{}

LArWheelSolidDDProxy::~LArWheelSolidDDProxy() {}

int LArWheelSolidDDProxy::Inside(const CLHEP::Hep3Vector& p) const {
	return m_plws->Inside(p);
}

double LArWheelSolidDDProxy::DistanceToIn(const CLHEP::Hep3Vector& p, const CLHEP::Hep3Vector& v) const {
	return m_plws->DistanceToIn(p, v);
}

double LArWheelSolidDDProxy::DistanceToIn(const CLHEP::Hep3Vector& p) const {
	return m_plws->DistanceToIn(p);
}

double LArWheelSolidDDProxy::DistanceToOut(const CLHEP::Hep3Vector& p, const CLHEP::Hep3Vector& v) const {
	return m_plws->DistanceToOut(p, v);
}

double LArWheelSolidDDProxy::DistanceToOut(const CLHEP::Hep3Vector& p) const {
	return m_plws->DistanceToOut(p);
}

CLHEP::Hep3Vector LArWheelSolidDDProxy::SurfaceNormal(const CLHEP::Hep3Vector& p) const {
	return m_plws->SurfaceNormal(p);
}

CLHEP::Hep3Vector LArWheelSolidDDProxy::GetPointOnSurface() const {
	return m_plws->GetPointOnSurface();
}

double LArWheelSolidDDProxy::GetCubicVolume() {
	return m_plws->GetCubicVolume();
}

double LArWheelSolidDDProxy::GetSurfaceArea() {
	return m_plws->GetSurfaceArea();
}

#ifdef DEBUG_LARWHEELSOLID
void LArWheelSolidDDProxy::SetVerbose(int v) const
{
	m_plws->SetVerbose(v);
}
#else
void LArWheelSolidDDProxy::SetVerbose(int) const
{
	std::cerr << "DEBUG_LARWHEELSOLID is off" << std::endl;
}
#endif
