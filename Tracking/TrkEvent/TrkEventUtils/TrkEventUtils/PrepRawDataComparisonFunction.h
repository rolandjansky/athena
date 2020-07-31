/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataComparisonFunction.h
//   Header file for struct PrepRawDataComparisonFunction
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch / Andreas.Salzburger@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRKNIRVANA_PREPRAWDATACOMPARISONFUNCTION_H
#define TRKNIRVANA_PREPRAWDATACOMPARISONFUNCTION_H

// Trk
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
// STL
#include <cmath>
#include <stdexcept>

namespace Trk {
/**
 * Class providing comparison function, or relational definition, for
 * PrepRawData
 */
class PrepRawDataComparisonFunction
{
public:
  /** Full relation definition using a straight line propagation */
  PrepRawDataComparisonFunction(const Amg::Vector3D& sp,
                                const Amg::Vector3D& dir)
    : m_point(sp)
    , m_direction(dir.unit())
  {}

  PrepRawDataComparisonFunction() = delete;
  PrepRawDataComparisonFunction(const PrepRawDataComparisonFunction& PCF) =
    default;
  PrepRawDataComparisonFunction(PrepRawDataComparisonFunction&& PCF) = default;

  PrepRawDataComparisonFunction& operator=(
    const PrepRawDataComparisonFunction& PCF) = default;

  PrepRawDataComparisonFunction& operator=(
    PrepRawDataComparisonFunction&& PCF) = default;

  ~PrepRawDataComparisonFunction() = default;

  /** The comparison function defining in what case a PRD is 'smaller' than
      a second one */
  bool operator()(const Trk::PrepRawData* one,
                  const Trk::PrepRawData* two) const
  {

    // --- identify the surface type and get intersection path for surface 1
    //
    double path1 = 0;
    const Trk::Surface& sf1 = one->detectorElement()->surface(one->identify());
    const Trk::Surface::SurfaceType surfType1  = sf1.type();
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
      default: {
        throw std::runtime_error(
          "PrepRawDataComparisonFunction: surface type error!");
      }
    } // --- no raw data on Cylinder. Ever.

    // --- identify the surface type and get intersection path for surface 1
    //
    double path2 = 0;
    const Trk::Surface& sf2 = two->detectorElement()->surface(two->identify());
    const Trk::Surface::SurfaceType surfType2 = sf2.type();
    switch (surfType2) {
      case Trk::Surface::Plane: {
        const Trk::PlaneSurface& opsf =
          static_cast<const Trk::PlaneSurface&>(sf2);
        path2 = this->pathIntersectWithPlane(opsf);
      } break;
      case Trk::Surface::Line: {
        const Trk::StraightLineSurface& ossf =
          static_cast<const Trk::StraightLineSurface&>(sf2);
        path2 = this->pathIntersectWithLine(ossf);
      } break;
      case Trk::Surface::Disc: {
        const Trk::DiscSurface& odsf =
          static_cast<const Trk::DiscSurface&>(sf2);
        path2 = this->pathIntersectWithDisc(odsf);
      } break;
      default: {
        throw std::runtime_error(
          "PrepRawDataComparisonFunction: surface type error!");
      }
    } // --- no raw data on Cylinder. Ever.

    return path1 < path2;
  }

private:
  Amg::Vector3D m_point;
  Amg::Vector3D m_direction;

  double pathIntersectWithPlane(const Trk::PlaneSurface& psf) const
  {
    double denom = m_direction.dot(psf.normal()); 
    return (denom) ? psf.normal().dot(psf.center() - m_point) / (denom)
                   : denom;
  }

  double pathIntersectWithLine(const Trk::StraightLineSurface& lsf) const
  {
    Amg::Vector3D dirWire(lsf.transform().rotation().col(2).unit());
    Amg::Vector3D trackToWire(lsf.center() - m_point);
    double parallelity = m_direction.dot(dirWire);
    double denom = 1 - parallelity * parallelity;
    return (std::abs(denom) > 10e-7)
             ? (trackToWire.dot(m_direction) -
                trackToWire.dot(dirWire) * parallelity) /
                 denom
             : 0.;
  }

  double pathIntersectWithDisc(const Trk::DiscSurface& dsf) const
  {
    double denom = m_direction.dot(dsf.normal());
    return (denom) ? dsf.normal().dot(dsf.center() - m_point) / (denom)
                   : denom;
  }
};

} // end of namespace

#endif // TRKNIRVANA_PREPRAWDATACOMPARISONFUNCTION_H

