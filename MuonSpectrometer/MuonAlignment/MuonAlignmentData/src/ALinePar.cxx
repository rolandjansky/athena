/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentData/ALinePar.h"

ALinePar::ALinePar():
  MuonAlignmentPar(),
  m_S(0.0),
  m_Z(0.0),
  m_T(0.0),
  m_rotS(0.0),
  m_rotZ(0.0),
  m_rotT(0.0)
{ }

ALinePar::~ALinePar()
{ }


void ALinePar::setParameters(float s, float z, float t, 
			     float rotS, float rotZ, float rotT)
{
  m_S = s;
  m_Z = z;
  m_T = t;
  m_rotS = rotS;
  m_rotZ = rotZ;
  m_rotT = rotT;
}

void ALinePar::getParameters(float& s, float& z, float& t, 
			     float& rotS, float& rotZ, float& rotT)
{
  s    = m_S;
  z    = m_Z;
  t    = m_T;
  rotS = m_rotS;
  rotZ = m_rotZ;
  rotT = m_rotT;
}

HepGeom::Transform3D ALinePar::deltaTransform()
{// does NOT account for AMDB origin being different from volume centre;
  // for that you would need access to full station Position info...
  // see MuonGeoModel/Station::getDeltaTransform() for details.
  return HepGeom::TranslateY3D(m_S)*HepGeom::TranslateZ3D(m_Z)*HepGeom::TranslateX3D(m_T)
    *HepGeom::RotateY3D(m_rotS)*HepGeom::RotateZ3D(m_rotZ)*HepGeom::RotateX3D(m_rotT);
}
