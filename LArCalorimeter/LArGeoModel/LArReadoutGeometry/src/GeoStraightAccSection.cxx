/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/GeoStraightAccSection.h"
void GeoStraightAccSection::Clockwork::buildFastCache() {
  delete r1;
  r1=new Rep1();
}

void GeoStraightAccSection::Clockwork::buildSmallCache() {
  if (r2) for (int i=0;i<14;i++) delete r2->transfunction[i]; 
  delete r2;
  r2=new Rep2();
  for (int i=0;i<14;i++) r2->transfunction[i]=0;
}

void GeoStraightAccSection::Clockwork::fillFastCache() {
  if (!r1) buildFastCache();
  for (int i=0;i<1024;i++) {
    for (int j=0;j<14; j++) {
      HepGeom::Transform3D XF = (*r2->transfunction[j])(i);
      r1->xcent[i][j] = XF.dx();
      r1->ycent[i][j] = XF.dy();
      r1->cosu [i][j] = -XF.xy();
      r1->sinu [i][j] = XF.xz();
      r1->halfLength[i][j]= r2->halfLength[j];
    }
  }
}

// Modifiers
double & GeoStraightAccSection::XCent(int stackid, int cellid) 
{
  if (m_c->r2) throw std::runtime_error("Error in GeoStraightAccSection: Illegal Modification Sequence");
  if (!m_c->r1) m_c->buildFastCache();
  return m_c->r1->xcent[stackid][cellid];
}
double & GeoStraightAccSection::YCent(int stackid, int cellid) 
{
  if (m_c->r2) throw std::runtime_error("Error in GeoStraightAccSection: Illegal Modification Sequence");
  if (!m_c->r1) m_c->buildFastCache();
  return m_c->r1->ycent[stackid][cellid];
}
double & GeoStraightAccSection::Cosu(int stackid, int cellid)
{
  if (m_c->r2) throw std::runtime_error("Error in GeoStraightAccSection: Illegal Modification Sequence");
  if (!m_c->r1) m_c->buildFastCache();
  return m_c->r1->cosu[stackid][cellid];
}
double & GeoStraightAccSection::Sinu(int stackid, int cellid)
{
  if (m_c->r2) throw std::runtime_error("Error in GeoStraightAccSection: Illegal Modification Sequence");
  if (!m_c->r1) m_c->buildFastCache();
  return m_c->r1->sinu[stackid][cellid];
}
double  & GeoStraightAccSection::HalfLength(int stackid, int cellid)
{
  if (m_c->r2) throw std::runtime_error("Error in GeoStraightAccSection: Illegal Modification Sequence");
  if (!m_c->r1)m_c->buildFastCache();
  return m_c->r1->halfLength[stackid][cellid];
}


void GeoStraightAccSection::setHalfLength(int stackid, double halfLength) {
  if (m_c->r1) throw std::runtime_error("Error in GeoStraightAccSection: Illegal Modification Sequence");
  if (!m_c->r2) m_c->buildSmallCache();
  m_c->r2->halfLength[stackid]=halfLength;
}

void GeoStraightAccSection::setTransform(int stackid, GeoXF::TRANSFUNCTION f) {
  if (m_c->r1) throw std::runtime_error("Error in GeoStraightAccSection: Illegal Modification Sequence");
  if (!m_c->r2) m_c->buildSmallCache();
  delete m_c->r2->transfunction[stackid];
  m_c->r2->transfunction[stackid]=f.clone();
}

GeoStraightAccSection::GeoStraightAccSection():m_c(new Clockwork) {
  m_c->r1=NULL;
  m_c->r2=NULL;
}	

GeoStraightAccSection::~GeoStraightAccSection() {
  if (m_c->r2) for (int i=0;i<14;i++) delete m_c->r2->transfunction[i];
  delete m_c->r2;
  delete m_c->r1;
  delete m_c;
}

