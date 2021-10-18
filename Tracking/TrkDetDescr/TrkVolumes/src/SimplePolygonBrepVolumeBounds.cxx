/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimplePolygonBrepVolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
#include "TrkVolumes/CombinedVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/PrismVolumeBounds.h"
#include "TrkVolumes/Volume.h"
#include "TrkVolumes/VolumeExcluder.h"
// TrkSurfaces
#include "TrkGeometrySurfaces/SubtractedPlaneSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
// STD
#include <cmath>
#include <iomanip>
#include <iostream>

Trk::SimplePolygonBrepVolumeBounds::SimplePolygonBrepVolumeBounds()
  : VolumeBounds()
  , m_halfX(0.)
  , m_halfY(0.)
  , m_halfZ(0.)
  , m_ordering(-1)
  , m_combinedVolume(nullptr)
  , m_envelope(nullptr)
  , m_objectAccessor()
{
  //@TODO an object created by the default constructor cannot be copied or
  //assigned.
}

Trk::SimplePolygonBrepVolumeBounds::SimplePolygonBrepVolumeBounds(
  std::vector<std::pair<float, float>> xyVtx,
  float halez)
  : VolumeBounds()
  , m_halfX(0.)
  , m_halfY(0.)
  , m_halfZ(halez)
  , m_ordering(-1)
  , m_combinedVolume(nullptr)
  , m_envelope(nullptr)
  , m_objectAccessor()
{
  m_xyVtx.resize(xyVtx.size());
  double xmin = xyVtx[0].first;
  double xmax = xyVtx[0].first;
  double ymin = xyVtx[0].second;
  double ymax = xyVtx[0].second;
  for (unsigned int i = 0; i < xyVtx.size(); i++) {
    m_xyVtx[i] = xyVtx[i];
    if (xyVtx[i].first < xmin)
      xmin = xyVtx[i].first;
    if (xyVtx[i].first > xmax)
      xmax = xyVtx[i].first;
    if (xyVtx[i].second < ymin)
      ymin = xyVtx[i].second;
    if (xyVtx[i].second > ymax)
      ymax = xyVtx[i].second;
  }
  double ehalfX = 0.5 * (xmax - xmin);
  double ehalfY = 0.5 * (ymax - ymin);
  m_halfX = fmax(std::abs(xmax), fabs(xmin));
  m_halfY = fmax(std::abs(ymax), fabs(ymin));
  Amg::Transform3D transXY(
    Amg::Translation3D(Amg::Vector3D(0.5 * (xmin + xmax), 0., 0.)) *
    Amg::Translation3D(Amg::Vector3D(0., 0.5 * (ymin + ymax), 0.)));
  m_envelope = new Trk::Volume(
    new Amg::Transform3D(transXY),
    new Trk::CuboidVolumeBounds(ehalfX, ehalfY, m_halfZ));

  processSubVols();
}

Trk::SimplePolygonBrepVolumeBounds::SimplePolygonBrepVolumeBounds(
  std::vector<std::pair<double, double>> xyVtx,
  double halez)
  : VolumeBounds()
  , m_halfX(0.)
  , m_halfY(0.)
  , m_halfZ(halez)
  , m_ordering(-1)
  , m_combinedVolume(nullptr)
  , m_envelope(nullptr)
  , m_objectAccessor()
{
  m_xyVtx.resize(xyVtx.size());
  double xmin = xyVtx[0].first;
  double xmax = xyVtx[0].first;
  double ymin = xyVtx[0].second;
  double ymax = xyVtx[0].second;
  for (unsigned int i = 0; i < xyVtx.size(); i++) {
    m_xyVtx[i] = xyVtx[i];
    if (xyVtx[i].first < xmin)
      xmin = xyVtx[i].first;
    if (xyVtx[i].first > xmax)
      xmax = xyVtx[i].first;
    if (xyVtx[i].second < ymin)
      ymin = xyVtx[i].second;
    if (xyVtx[i].second > ymax)
      ymax = xyVtx[i].second;
  }
  double ehalfX = 0.5 * (xmax - xmin);
  double ehalfY = 0.5 * (ymax - ymin);
  m_halfX = fmax(std::abs(xmax), fabs(xmin));
  m_halfY = fmax(std::abs(ymax), fabs(ymin));
  Amg::Transform3D transXY(
    Amg::Translation3D(Amg::Vector3D(0.5 * (xmin + xmax), 0., 0.)) *
    Amg::Translation3D(Amg::Vector3D(0., 0.5 * (ymin + ymax), 0.)));
  m_envelope = new Trk::Volume(
    new Amg::Transform3D(transXY),
    new Trk::CuboidVolumeBounds(ehalfX, ehalfY, m_halfZ));

  processSubVols();
}

Trk::SimplePolygonBrepVolumeBounds::SimplePolygonBrepVolumeBounds(
  const Trk::SimplePolygonBrepVolumeBounds& trabo)
  : VolumeBounds()
  , m_halfX(trabo.m_halfX)
  , m_halfY(trabo.m_halfY)
  , m_halfZ(trabo.m_halfZ)
  , m_ordering(trabo.m_ordering)
  , m_combinedVolume(trabo.m_combinedVolume->clone())
  , m_envelope(trabo.m_envelope->clone())
  , m_objectAccessor(trabo.m_objectAccessor)
{
  m_xyVtx.resize(trabo.m_xyVtx.size());
  for (unsigned int i = 0; i < m_xyVtx.size(); i++)
    m_xyVtx[i] = trabo.m_xyVtx[i];
}

Trk::SimplePolygonBrepVolumeBounds::~SimplePolygonBrepVolumeBounds()
{
  delete m_combinedVolume;
  delete m_envelope;
}

Trk::SimplePolygonBrepVolumeBounds&
Trk::SimplePolygonBrepVolumeBounds::operator=(
  const Trk::SimplePolygonBrepVolumeBounds& trabo)
{
  if (this != &trabo) {
    m_halfX = trabo.m_halfX;
    m_halfY = trabo.m_halfY;
    m_halfZ = trabo.m_halfZ;
    m_objectAccessor = trabo.m_objectAccessor;
    m_xyVtx.resize(trabo.m_xyVtx.size());
    for (unsigned int i = 0; i < m_xyVtx.size(); i++)
      m_xyVtx[i] = trabo.m_xyVtx[i];
    delete m_combinedVolume;
    delete m_envelope;
    m_combinedVolume = trabo.m_combinedVolume->clone();
    m_envelope = trabo.m_envelope->clone();
  }
  return *this;
}

const std::vector<const Trk::Surface*>*
  Trk::SimplePolygonBrepVolumeBounds::decomposeToSurfaces
  (const Amg::Transform3D& transform) 
{
  std::vector<const Trk::Surface*>* retsf =
    new std::vector<const Trk::Surface*>;

  // face surfaces xy
  //  (1) - at negative local z
  Trk::PlaneSurface xymPlane(
    Amg::Transform3D(
      transform * Amg::Translation3D(Amg::Vector3D(0., 0., -m_halfZ))),
    new Trk::RectangleBounds(m_halfX, m_halfY));
  Trk::VolumeExcluder* volExcl = new Trk::VolumeExcluder(new Trk::Volume(
    *m_combinedVolume,
    Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0., 0., -m_halfZ)))));
  retsf->push_back(new Trk::SubtractedPlaneSurface(xymPlane, volExcl, true));
  //  (2) - at positive local z
  Trk::PlaneSurface xyPlane(
    Amg::Transform3D(
      transform * Amg::Translation3D(Amg::Vector3D(0., 0., m_halfZ))),
    new Trk::RectangleBounds(m_halfX, m_halfY));
  volExcl = new Trk::VolumeExcluder(new Trk::Volume(
    *m_combinedVolume,
    Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0., 0., m_halfZ)))));
  retsf->push_back(new Trk::SubtractedPlaneSurface(xyPlane, volExcl, true));
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
Trk::SimplePolygonBrepVolumeBounds::sideSurf(
  const Amg::Transform3D& transform,
  unsigned int iv1,
  unsigned int iv2) const
{
  Trk::PlaneSurface* plane = nullptr;

  double xdif = m_xyVtx[iv2].first - m_xyVtx[iv1].first;
  double ydif = m_xyVtx[iv2].second - m_xyVtx[iv1].second;
  double xsize = sqrt(xdif * xdif + ydif * ydif);

  double ori = m_ordering > 0 ? -1. : 1.;

  Amg::Vector3D pos(
    0.5 * (m_xyVtx[iv1].first + m_xyVtx[iv2].first),
    0.5 * (m_xyVtx[iv1].second + m_xyVtx[iv2].second),
    0.);
  double phi = ori * ydif < 0 ? M_PI / 2 : -M_PI / 2;
  if (ori > 0 && ydif > 0)
    phi = M_PI / 2;
  if (std::abs(xdif) > 1e-6) {
    phi = std::atan(ydif / xdif);
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
  //  ||
  //  !plane->isOnSurface(Trk::GlobalPosition(m_xyVtx[iv2].first,m_xyVtx[iv2].second,-m_halfZ),true,0.001,0.001)
  //  )
  // std::cout << "ERROR in SIMPLEPGONBREP side boundary:vertices out of
  // plane"<< std::endl;

  return plane;
}

bool
Trk::SimplePolygonBrepVolumeBounds::inside(const Amg::Vector3D& pos, double tol)
  const
{
  return (m_combinedVolume->inside(pos, tol));
}

void
Trk::SimplePolygonBrepVolumeBounds::processSubVols()
{
  // translate into prisms (triangulate)
  Trk::Volume* cVol = nullptr;
#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif
  std::vector<std::pair<double, double>> triangles = TriangulatePolygonCheck(
    m_xyVtx); //@TODO change argument to const vector<pair< > >
  std::vector<std::pair<double, double>> vertices;
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif
  for (unsigned int i = 0; i < triangles.size(); i = i + 3) {
    vertices.push_back(triangles[i]);
    vertices.push_back(triangles[i + 1]);
    vertices.push_back(triangles[i + 2]);
    Trk::Volume* newVol =
      new Trk::Volume(nullptr, new Trk::PrismVolumeBounds(vertices, m_halfZ));
    if (cVol)
      cVol = new Trk::Volume(
        nullptr, new Trk::CombinedVolumeBounds(cVol, newVol, false));
    else
      cVol = newVol;
    vertices.clear();
  }
  m_combinedVolume = cVol;
}

// ostream operator overload
MsgStream&
Trk::SimplePolygonBrepVolumeBounds::dump(MsgStream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::SimplePolygonBrepVolumeBounds: (halfZ, xy vertices) = ";
  temp_sl << "( " << m_halfZ << ")";
  for (unsigned int i = 0; i < m_xyVtx.size(); i++)
    temp_sl << "(" << m_xyVtx[i].first << "," << m_xyVtx[i].second << ")";
  sl << temp_sl.str();
  return sl;
}

std::ostream&
Trk::SimplePolygonBrepVolumeBounds::dump(std::ostream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::SimplePolygonBrepVolumeBounds: (halfZ, xy vertices) = ";
  temp_sl << "( " << m_halfZ << ")";
  for (unsigned int i = 0; i < m_xyVtx.size(); i++)
    temp_sl << "(" << m_xyVtx[i].first << "," << m_xyVtx[i].second << ")";
  sl << temp_sl.str();
  return sl;
}

//////////////////////////////////////////////////////////////////////////
// Triangulate Polygon
// M. Wolter
//////////////////////////////////////////////////////////////////////////

bool
Trk::SimplePolygonBrepVolumeBounds::Xor(bool x, bool y) 
// XOR: Arguments are negated to ensure that they are 0/1. Then the bitwise Xor
// operator may apply.
{
  return !x ^ !y;
}

#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif

bool
Trk::SimplePolygonBrepVolumeBounds::Left(
  std::pair<double, double> a,
  std::pair<double, double> b,
  std::pair<double, double> c) const
// Returns true iff c is strictly to the left of the directed line through a to
// b.
{
  double CrossZ = (b.first - a.first) * (c.second - a.second) -
                  (c.first - a.first) * (b.second - a.second);
  if (m_ordering == 1)
    return (CrossZ >= 0.);
  if (m_ordering == 0)
    return (CrossZ < 0.);
  return false;
}

bool
Trk::SimplePolygonBrepVolumeBounds::Intersect(
  std::pair<double, double> a,
  std::pair<double, double> b,
  std::pair<double, double> c,
  std::pair<double, double> d) const
// Returns true iff segments ab and cd intersect
{

  return Xor(Left(a, b, c), Left(a, b, d)) && Xor(Left(c, d, a), Left(c, d, b));
}

bool
Trk::SimplePolygonBrepVolumeBounds::InCone(
  int i,
  int j,
  std::vector<std::pair<double, double>> inputVertices) const
// 	Returns true iff the diagonal (i,j) is internal to the polygon in
//  the neighborhood of the i endpoint.
{
  int iPlus1 = (i + 1) % inputVertices.size();
  int iMinus1 = (i + inputVertices.size() - 1) % inputVertices.size();

  /* If P[i] is a convex vertex [ i+1 left or on (i-1,i) ]. */
  if (Left(inputVertices[iMinus1], inputVertices[i], inputVertices[iPlus1]))
    return Left(inputVertices[i], inputVertices[j], inputVertices[iMinus1]) &&
           Left(inputVertices[j], inputVertices[i], inputVertices[iPlus1]);

  /* Assume (i-1,i,i+1) not collinear. */
  /* else v_i is reflex. */
  else
    return !(
      Left(inputVertices[i], inputVertices[j], inputVertices[iPlus1]) &&
      Left(inputVertices[j], inputVertices[i], inputVertices[iMinus1]));
}

bool
Trk::SimplePolygonBrepVolumeBounds::Diagonalie(
  int i,
  int j,
  std::vector<std::pair<double, double>> inputVertices) const
{
  // Returns TRUE iff (v_i, v_j) is a proper internal *or* external diagonal of
  // this polygon, *ignoring edges incident to v_i and v_j*.

  /* For each edge (k,k+1) of P */
  for (int k = 0; k < (int)inputVertices.size(); k++) {

    int kPlus1 = (k + 1) % inputVertices.size();

    /* Skip edges incident to i or j */
    if (!((k == i) || (kPlus1 == i) || (k == j) || (kPlus1 == j)))
      if (Intersect(
            inputVertices[i],
            inputVertices[j],
            inputVertices[k],
            inputVertices[kPlus1]))
        return false;
  }
  return true;
}

bool
Trk::SimplePolygonBrepVolumeBounds::Diagonal(
  int i,
  int j,
  const std::vector<std::pair<double, double>>& inputVertices) const
// Returns TRUE iff (v_i, v_j) is a proper internal diagonal of P.
{
  // std::cout<<"MW Diagonal "<<i<<" "<<j<<" "<<InCone(i,j, inputVertices)<<"
  // "<<Diagonalie(i,j, inputVertices)<<std::endl;
  return InCone(i, j, inputVertices) && Diagonalie(i, j, inputVertices);
}

std::vector<std::pair<double, double>>
Trk::SimplePolygonBrepVolumeBounds::TriangulatePolygon(
  const std::vector<std::pair<double, double>>& Vertices) const
{
  // Subtracting ears method
  //
  // One way to triangulate a simple polygon is by using the assertion that any
  // simple polygon without holes
  //  has at least two so called 'ears'. An ear is a triangle with two sides on
  //  the edge of the polygon and the other one completely inside it. The
  //  algorithm then consists of finding such an ear, removing it from the
  //  polygon (which results in a new polygon that still meets the conditions)
  //  and repeating until there is only one triangle left.
  //

  int NSize = Vertices.size();
  std::vector<std::pair<double, double>> outTriangles;
  std::vector<std::pair<double, double>> inputVertices;
  inputVertices.reserve(NSize);
  for (int i = 0; i < NSize; i++)
    inputVertices.push_back((Vertices)[i]);

  // for (int i; i<NSize;i++) std::cout<<"MW input vertices:
  // "<<inputVertices[i].first<<" "<<inputVertices[i].second<<std::endl;
  // Triangulates this polygon and saves triangle edges in TriPoly.
  // Triangles are stored CCW, with each set of 3 consecutive points in TriPoly
  // representing 1 triangle.
  // Assumes this polygon is closed.

  if (NSize < 4)
    return inputVertices;

  // Start triangulating
  int VerticesLeft = NSize;
  while (VerticesLeft > 3) {
    // std::cout<<"MW vertices left "<<VerticesLeft<<std::endl;
    bool bCornerCut = false;
    for (int i = 0; i < VerticesLeft; i++) {

      int iPlus1 = i + 1;
      if (iPlus1 == VerticesLeft)
        iPlus1 = 0;
      int iPlus2 = (iPlus1 + 1);
      if (iPlus2 == VerticesLeft)
        iPlus2 = 0;

      if (Diagonal(i, iPlus2, inputVertices)) {

        outTriangles.push_back(inputVertices[i]);
        outTriangles.push_back(inputVertices[iPlus1]);
        outTriangles.push_back(inputVertices[iPlus2]);

        inputVertices.erase(inputVertices.begin() + iPlus1);
        VerticesLeft--;
        bCornerCut = true;
        break;
      }
    }
    if (!bCornerCut) { // Error - bad poly
      // std::cout<<"MW 	Error - bad poly"<<std::endl;
      std::vector<std::pair<double, double>> out;
      return out;
    }
  }

  if (VerticesLeft == 3) {
    outTriangles.push_back(inputVertices[0]);
    outTriangles.push_back(inputVertices[1]);
    outTriangles.push_back(inputVertices[2]);
    inputVertices.erase(inputVertices.begin() + 1);
    VerticesLeft--;
  }

  return outTriangles;
}

std::vector<std::pair<double, double>>
Trk::SimplePolygonBrepVolumeBounds::TriangulatePolygonCheck(
  const std::vector<std::pair<double, double>>& Vertices)
{
  // Perform triangulation. Check the orientation of the verices in the polygon
  // m_ordering   = -1    not set
  // m_ordering   =  1    anticlockwise
  // m_ordering   =  0    clockwise

  if (m_ordering == -1)
    m_ordering = 1;
  std::vector<std::pair<double, double>> outTriangles =
    TriangulatePolygon(Vertices);
  if (outTriangles.empty()) {
    m_ordering = -m_ordering + 1;
    outTriangles = TriangulatePolygon(Vertices);
  }

  return outTriangles;
}
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif

