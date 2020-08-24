/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeasurementBaseComparisonFunction.h
//   Header file for struct MeasurementBaseComparisonFunction
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch / Andreas.Salzburger@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRKNIRVANA_MEASUREMENTBASECOMPARISONFUNCTION_H
#define TRKNIRVANA_MEASUREMENTBASECOMPARISONFUNCTION_H

// Trk
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
// extra-maths for cylinder intersections
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"
// STL
#include <algorithm>
#include <stdexcept>

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

/** Class implementing a comparison function
 * for sorting MeasurementBase objects*/

class MeasurementBaseComparisonFunction
{
public:
  MeasurementBaseComparisonFunction() = delete;
  ~MeasurementBaseComparisonFunction() = default;
  /** Full relation definition using a straight line propagation */
  MeasurementBaseComparisonFunction(const Amg::Vector3D& sp,
                                    const Amg::Vector3D& dir)
    : m_point(sp)
    , m_direction(dir.unit())
  {}

  /*
   * Default methods
   */
  MeasurementBaseComparisonFunction(
    const MeasurementBaseComparisonFunction& MCF) = default;
  MeasurementBaseComparisonFunction& operator=(
    MeasurementBaseComparisonFunction& MCF) = default;
  MeasurementBaseComparisonFunction(MeasurementBaseComparisonFunction&& MCF) =
    default;
  MeasurementBaseComparisonFunction& operator=(
    MeasurementBaseComparisonFunction&& MCF) = default;

  /** The comparison function defining in what case a Measurement is 'smaller'
     than a second one */
  bool operator()(const Trk::MeasurementBase* one,
                  const Trk::MeasurementBase* two) const
  {

    // --- flexible sorting along a predicted direction
    double path1 = 0;
    const Trk::Surface& sf1 = one->associatedSurface();
    const Trk::Surface::SurfaceType surfType1 = sf1.type();
    switch (surfType1) {
      case Trk::Surface::Plane: {
        const Trk::PlaneSurface& opsf =
          static_cast<const Trk::PlaneSurface&>(sf1);
        path1 = this->pathIntersectWithPlane(opsf);
      } break;
      case Trk::Surface::Line: {
        const Trk::StraightLineSurface& ossf =
          static_cast<const Trk::StraightLineSurface&>(sf1);
        path1 = this->pathIntersectWithLine(ossf);
      } break;
      case Trk::Surface::Disc: {
        const Trk::DiscSurface& odsf =
          static_cast<const Trk::DiscSurface&>(sf1);
        path1 = this->pathIntersectWithDisc(odsf);
      } break;
      case Trk::Surface::Cylinder: {
        const Trk::CylinderSurface& ocsf =
          static_cast<const Trk::CylinderSurface&>(sf1);
        path1 = this->pathIntersectWithCylinder(ocsf, one->globalPosition());
      } break;
      case Trk::Surface::Perigee: {
        const Trk::PerigeeSurface& ogsf =
          static_cast<const Trk::PerigeeSurface&>(sf1);
        path1 = this->pathIntersectWithLine(ogsf);
      } break;
      default: {
        throw std::runtime_error(
          "MeasurementBaseComparisonFunction: surface type error for Sf1!");
      }
    }

    // --- identify the 2nd surface type and get intersection path for surface 1
    double path2 = 0;
    const Trk::Surface& sf2 = two->associatedSurface();
    const Trk::Surface::SurfaceType surfType2 = sf2.type();
    switch (surfType2) {
      case Trk::Surface::Plane: {
        const Trk::PlaneSurface& tpsf =
          static_cast<const Trk::PlaneSurface&>(sf2);
        path2 = this->pathIntersectWithPlane(tpsf);
      } break;
      case Trk::Surface::Line: {
        const Trk::StraightLineSurface& tssf =
          static_cast<const Trk::StraightLineSurface&>(sf2);
        path2 = this->pathIntersectWithLine(tssf);
      } break;
      case Trk::Surface::Disc: {
        const Trk::DiscSurface& tdsf =
          static_cast<const Trk::DiscSurface&>(sf2);
        path2 = this->pathIntersectWithDisc(tdsf);
      } break;
      case Trk::Surface::Cylinder: {
        const Trk::CylinderSurface& tcsf =
          static_cast<const Trk::CylinderSurface&>(sf2);
        path2 = this->pathIntersectWithCylinder(tcsf, two->globalPosition());
      } break;
      case Trk::Surface::Perigee: {
        const Trk::PerigeeSurface& tgsf =
          static_cast<const Trk::PerigeeSurface&>(sf2);
        path2 = this->pathIntersectWithLine(tgsf);
      } break;
      default: {
        throw std::runtime_error(
          "MeasurementBaseComparisonFunction: surface type error for Sf2!");
      }
    }
    return path1 < path2;
  }

private:
  Amg::Vector3D m_point;
  Amg::Vector3D m_direction;

  double pathIntersectWithPlane(const Trk::PlaneSurface& psf) const
  {
    double denom = m_direction.dot(psf.normal()); 
    return (denom) ? psf.normal().dot(psf.center() - m_point) / (denom) : denom;
  }

  double pathIntersectWithLine(const Trk::StraightLineSurface& lsf) const
  {
    Amg::Vector3D dirWire(lsf.transform().rotation().col(2));
    dirWire.normalize();
    Amg::Vector3D trackToWire(lsf.center() - m_point);
    double parallelity = m_direction.dot(dirWire);
    double denom = 1 - parallelity * parallelity;
    return (fabs(denom) > 10e-7) ? (trackToWire.dot(m_direction) -
                                    trackToWire.dot(dirWire) * parallelity) /
                                     denom
                                 : 0.;
  }

  double pathIntersectWithLine(const Trk::PerigeeSurface& pgsf) const
  {
    Amg::Vector3D trackToWire(pgsf.center() - m_point);
    double parallelity = m_direction.dot(Trk::s_zAxis);
    double denom = 1 - parallelity * parallelity;
    return (fabs(denom) > 10e-7)
             ? (trackToWire.dot(m_direction) -
                trackToWire.dot(Trk::s_zAxis) * parallelity) /
                 denom
             : 0.;
  }

  double pathIntersectWithDisc(const Trk::DiscSurface& dsf) const
  {
    double denom = m_direction.dot(dsf.normal());
    return (denom) ? dsf.normal().dot(dsf.center() - m_point) / (denom) : denom;
  }

  double pathIntersectWithCylinder(const Trk::CylinderSurface& csf,
                                   const Amg::Vector3D& globalHit) const
  { // --- code from TrkExSlPropagator/LineCylinderIntersection.cxx

    // get the rotation by reference
    const Amg::Transform3D& locTrans = csf.transform();
    // take two points of line and calculate them to the 3D frame of the
    // cylinder
    Amg::Vector3D point1(locTrans.inverse() * m_point);
    Amg::Vector3D point2raw = m_point + m_direction;
    Amg::Vector3D point2(locTrans.inverse() * point2raw); // do it in two steps

    // new direction in 3D frame of cylinder
    Amg::Vector3D direc((point2 - point1).unit());

    if (!direc.x()) {
      return 0.;
    } else {
      // get line and circle constants
      double k = (direc.y()) / (direc.x());
      double d = (point2.x() * point1.y() - point1.x() * point2.y()) /
                 (point2.x() - point1.x());
      double R = csf.bounds().r();
      double first = 0.;
      double second = 0.;

      // and solve the quadratic equation  Trk::RealQuadraticEquation
      // pquad(1+k*k, 2*k*d, d*d-R*R);
      double a = 1 + k * k;
      double p = 2 * k * d;
      double q = d * d - R * R;
      double discriminant = p * p - 4 * a * q;
      if (discriminant < 0) {
        return 0.;
      } else {
        // solutions = (discriminant==0) ? one : two;
        double x0 =
          -0.5 * (p + (p > 0 ? sqrt(discriminant) : -sqrt(discriminant)));
        first = x0 / a;
        second = q / x0;
      }
      double t1 = (first - point1.x()) / direc.x();
      double t2 = (second - point1.x()) / direc.x();
      // the solutions in the 3D frame of the cylinder
      Amg::Vector3D dist1raw(point1 + t1 * direc - globalHit);
      Amg::Vector3D dist2raw(point1 + t2 * direc - globalHit);
      // return the solution which is closer to Meas'Base's global coordinates
      if (dist1raw.mag() < dist2raw.mag()) {
        return t1; 
      } else {
        return t2;
      }
    }
  }
};
} // end of namespace

#endif // TRKNIRVANA_MEASUREMENTBASECOMPARISONFUNCTION_H

