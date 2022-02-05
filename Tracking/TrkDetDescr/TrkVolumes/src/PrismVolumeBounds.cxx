/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrismVolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/PrismVolumeBounds.h"
#include "TrkVolumes/Volume.h"
// TrkSurfaces
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TriangleBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
// STD
#include <cmath>
#include <iomanip>
#include <iostream>

Trk::PrismVolumeBounds::PrismVolumeBounds()
  : VolumeBounds()
  , m_halfZ()
  , m_baseBounds(nullptr)
  , m_ordering()
  , // invalid cache
  m_objectAccessor()
{}

Trk::PrismVolumeBounds::PrismVolumeBounds(
  std::vector<std::pair<float, float>> xyVtx,
  float halez)
  : VolumeBounds()
  , m_halfZ(halez)
  , m_baseBounds(nullptr)
  , m_ordering()
  , // invalid cache
  m_objectAccessor()
{
  m_xyVtx.resize(xyVtx.size());
  m_xyVtx.assign(xyVtx.begin(), xyVtx.end());
  m_baseBounds = new Trk::TriangleBounds(m_xyVtx);
}

Trk::PrismVolumeBounds::PrismVolumeBounds(
  std::vector<std::pair<double, double>> xyVtx,
  double halez)
  : VolumeBounds()
  , m_halfZ(halez)
  , m_baseBounds(nullptr)
  , m_ordering()
  , // invalid cache
  m_objectAccessor()
{
  m_xyVtx.resize(xyVtx.size());
  m_xyVtx.assign(xyVtx.begin(), xyVtx.end());
  m_baseBounds = new Trk::TriangleBounds(m_xyVtx);
}

Trk::PrismVolumeBounds::PrismVolumeBounds(const Trk::PrismVolumeBounds& trabo)
  : VolumeBounds()
  , m_halfZ(trabo.m_halfZ)
  , m_baseBounds(nullptr)
  , m_ordering(trabo.m_ordering)
  , // ordering cache will be valid if trabo/other cache is valid
  m_objectAccessor(trabo.m_objectAccessor)
{
  m_xyVtx.resize(trabo.m_xyVtx.size());
  m_xyVtx.assign(trabo.m_xyVtx.begin(), trabo.m_xyVtx.end());
  m_baseBounds = new Trk::TriangleBounds(m_xyVtx);
}

Trk::PrismVolumeBounds::~PrismVolumeBounds()
{
  delete m_baseBounds;
}

Trk::PrismVolumeBounds&
Trk::PrismVolumeBounds::operator=(const Trk::PrismVolumeBounds& trabo)
{
  if (this != &trabo) {
    m_halfZ = trabo.m_halfZ;
    m_objectAccessor = trabo.m_objectAccessor;
    m_xyVtx.resize(trabo.m_xyVtx.size());
    m_xyVtx.assign(trabo.m_xyVtx.begin(), trabo.m_xyVtx.end());
    delete m_baseBounds;
    m_baseBounds = new Trk::TriangleBounds(m_xyVtx);
    m_ordering =
      trabo.m_ordering; // ordering cache will be valid if trabo cache is valid
  }
  return *this;
}

const std::vector<const Trk::Surface*>*
  Trk::PrismVolumeBounds::decomposeToSurfaces
  (const Amg::Transform3D& transform) 
{
  std::vector<const Trk::Surface*>* retsf =
    new std::vector<const Trk::Surface*>;

  // face surfaces xy
  //  (1) - at positive local z
  Trk::PlaneSurface* xyPlane = new Trk::PlaneSurface(
    Amg::Transform3D(
      transform * Amg::Translation3D(Amg::Vector3D(0., 0., m_halfZ))),
    new Trk::TriangleBounds(m_xyVtx));
  retsf->push_back(xyPlane);
  //  (2) - at negative local z
  Trk::PlaneSurface* xymPlane = new Trk::PlaneSurface(
    Amg::Transform3D(
      transform * Amg::Translation3D(Amg::Vector3D(0., 0., -m_halfZ)) *
      Amg::AngleAxis3D(180 * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.))),
    new Trk::TriangleBounds(mirror_xyVtx()));
  retsf->push_back(xymPlane);
  // loop over xy vertices
  //  (3)
  for (unsigned int iv = 0; iv < m_xyVtx.size(); iv++) {
    if (iv != m_xyVtx.size() - 1)
      retsf->push_back(sideSurf(transform, iv, iv + 1));
    else
      retsf->push_back(sideSurf(transform, iv, 0));
  }

  return retsf;
}

// faces in xy
Trk::PlaneSurface*
Trk::PrismVolumeBounds::sideSurf(
  const Amg::Transform3D& transform,
  unsigned int iv1,
  unsigned int iv2) const
{
  Trk::PlaneSurface* plane = nullptr;

  double xdif = m_xyVtx[iv2].first - m_xyVtx[iv1].first;
  double ydif = m_xyVtx[iv2].second - m_xyVtx[iv1].second;
  double xsize = sqrt(xdif * xdif + ydif * ydif);

  double ori = ordering() > 0 ? 1. : -1.;

  Amg::Vector3D pos(
    0.5 * (m_xyVtx[iv1].first + m_xyVtx[iv2].first),
    0.5 * (m_xyVtx[iv1].second + m_xyVtx[iv2].second),
    0.);
  double phi = ori * ydif < 0 ? M_PI / 2 : -M_PI / 2;
  if (ori > 0 && ydif > 0)
    phi = M_PI / 2;
  if (std::abs(xdif) > 1e-6) {
    phi = atan(ydif / xdif);
    if (xdif < 0)
      phi += M_PI;
  }

  Amg::Transform3D tr(
    transform * Amg::Translation3D(pos) *
    Amg::AngleAxis3D(phi, Amg::Vector3D(0., 0., 1.)) *
    Amg::AngleAxis3D(-ori * 90 * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.)));
  plane =
    new Trk::PlaneSurface(tr, new Trk::RectangleBounds(0.5 * xsize, m_halfZ));

  // verify position of vertices - uncomment for debugging
  // if
  // (!plane->isOnSurface(Trk::GlobalPosition(m_xyVtx[iv1].first,m_xyVtx[iv1].second,m_halfZ),true,0.001,0.001)
  //    ||
  //    !plane->isOnSurface(Trk::GlobalPosition(m_xyVtx[iv2].first,m_xyVtx[iv2].second,-m_halfZ),true,0.001,0.001)
  //    )
  //  std::cout << "ERROR in PRISM side boundary:vertices out of plane"<<
  //  std::endl;

  // orientation
  int ivr = (iv1 == 0 || iv2 == 0) ? 1 : 0;
  if (ivr == 1 && (iv1 == 1 || iv2 == 1))
    ivr = 2;

  double ox = m_xyVtx[ivr].first - pos[0];
  double oy = m_xyVtx[ivr].second - pos[1];
  Amg::Vector3D d(ox, oy, 0.);

  // protect against wrong orientation
  if (d.dot(plane->normal()) > 0.) {
    delete plane;
    tr = Amg::Transform3D(
      transform * Amg::Translation3D(pos) *
      Amg::AngleAxis3D(phi + M_PI, Amg::Vector3D(0., 0., 1.)) *
      Amg::AngleAxis3D(
        -ori * 90 * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.)));
    plane =
      new Trk::PlaneSurface(tr, new Trk::RectangleBounds(0.5 * xsize, m_halfZ));
  }

  return plane;
}

bool
Trk::PrismVolumeBounds::inside(const Amg::Vector3D& pos, double tol) const
{
  if (std::abs(pos.z()) > m_halfZ + tol)
    return false;
  // xy plane
  Amg::Vector2D locp(pos.x(), pos.y());
  return (m_baseBounds->inside(locp, tol, tol));
}

std::vector<std::pair<double, double>>
Trk::PrismVolumeBounds::mirror_xyVtx() const
{
  std::vector<std::pair<double, double>> mirrored;
  mirrored.resize(m_xyVtx.size());

  for (unsigned int i = 0; i < m_xyVtx.size(); i++)
    mirrored[i] =
      std::pair<double, double>(m_xyVtx[i].first, -m_xyVtx[i].second);

  return mirrored;
}

int
Trk::PrismVolumeBounds::ordering() const
{
  if (m_ordering.isValid()) {
    return *(m_ordering.ptr());
  }

  int tmp_ordering = 1;

  double yd2 = m_xyVtx[2].second - m_xyVtx[1].second;
  double yd0 = m_xyVtx[0].second - m_xyVtx[1].second;
  double xd2 = m_xyVtx[2].first - m_xyVtx[1].first;
  double xd0 = m_xyVtx[0].first - m_xyVtx[1].first;
  double ph2 = yd2 < 0 ? -M_PI / 2 : M_PI / 2;
  if (std::abs(xd2) > 1e-6) {
    ph2 = atan(yd2 / xd2);
    if (xd2 < 0)
      ph2 += M_PI;
  }
  double ph0 = yd0 < 0 ? -M_PI / 2 : M_PI / 2;
  if (std::abs(xd0) > 1e-6) {
    ph0 = atan(yd0 / xd0);
    if (xd0 < 0)
      ph0 += M_PI;
  }
  if (ph0 < 0)
    ph0 += 2 * M_PI;
  if (ph2 < 0)
    ph2 += 2 * M_PI;

  if ((ph0 > ph2 && ph0 - ph2 < M_PI) || (ph2 - ph0) > M_PI)
    tmp_ordering = 0;

  m_ordering.set(tmp_ordering);
  return *(m_ordering.ptr());
}

// ostream operator overload
MsgStream&
Trk::PrismVolumeBounds::dump(MsgStream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::PrismVolumeBounds: (halfZ, generating vtx) = ";
  temp_sl << "( " << m_halfZ << ")";
  for (unsigned int i = 0; i < m_xyVtx.size(); i++)
    temp_sl << "(" << m_xyVtx[i].first << "," << m_xyVtx[i].second << ")";
  sl << temp_sl.str();

  return sl;
}

std::ostream&
Trk::PrismVolumeBounds::dump(std::ostream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::PrismVolumeBounds:  (halfZ, generating vtx) = ";
  temp_sl << "( " << m_halfZ << ")";
  for (unsigned int i = 0; i < m_xyVtx.size(); i++)
    temp_sl << "(" << m_xyVtx[i].first << "," << m_xyVtx[i].second << ")";
  sl << temp_sl.str();

  return sl;
}
