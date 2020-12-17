/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/GeoStraightAccSection.h"
void GeoStraightAccSection::Clockwork::buildSmallCache()
{
  if (!r2) {
    r2 = std::make_unique<Rep2>();
  }
  else {
    for (int i=0;i<14;i++) {
      r2->halfLength[i] = 0;
      r2->transfunction[i].reset();
    }
  }
}

void GeoStraightAccSection::Clockwork::fillFastCache (Rep1& rep1) const
{
  for (int i=0;i<1024;i++) {
    for (int j=0;j<14; j++) {
      GeoTrf::Transform3D XF = (*r2->transfunction[j])(i);
      rep1.xcent[i][j] = XF(0,3); //dx
      rep1.ycent[i][j] = XF(1,3); //dy
      rep1.cosu [i][j] = -XF(0,1);//xy
      rep1.sinu [i][j] = XF(0,2);//xz
      rep1.halfLength[i][j]= r2->halfLength[j];
    }
  }
}

// Modifiers
double & GeoStraightAccSection::XCent(int stackid, int cellid) 
{
  return m_c->getWritableFastCache().xcent[stackid][cellid];
}
double & GeoStraightAccSection::YCent(int stackid, int cellid) 
{
  return m_c->getWritableFastCache().ycent[stackid][cellid];
}
double & GeoStraightAccSection::Cosu(int stackid, int cellid)
{
  return m_c->getWritableFastCache().cosu[stackid][cellid];
}
double & GeoStraightAccSection::Sinu(int stackid, int cellid)
{
  return m_c->getWritableFastCache().sinu[stackid][cellid];
}
double  & GeoStraightAccSection::HalfLength(int stackid, int cellid)
{
  return m_c->getWritableFastCache().halfLength[stackid][cellid];
}


void GeoStraightAccSection::setHalfLength(int stackid, double halfLength) {
  m_c->getWritableSmallCache().halfLength[stackid]=halfLength;
}

void GeoStraightAccSection::setTransform(int stackid, GeoXF::TRANSFUNCTION f) {
  m_c->getWritableSmallCache().transfunction[stackid] =
    std::unique_ptr<GeoXF::Function> (f.clone());
}

GeoStraightAccSection::GeoStraightAccSection()
  : m_c(std::make_unique<Clockwork>())
{
}	

GeoStraightAccSection::~GeoStraightAccSection()
{
}

