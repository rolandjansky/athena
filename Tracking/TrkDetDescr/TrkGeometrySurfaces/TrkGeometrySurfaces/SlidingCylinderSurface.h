/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SlidingCylinderSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRYSURFACES_SLIDINGCYLINDERSURFACE_H
#define TRKGEOMETRYSURFACES_SLIDINGCYLINDERSURFACE_H

// Trk
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/CylinderSurface.h"
// Geometry & Math
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;
class Identifier;

namespace Trk {

/**
 @class SlidingCylinderSurface
 Class for a Calo CylinderSurface with variable depth in the ATLAS detector.
 The variable depth is stored as a binned vector of radial corrections.
 Local eta bin is defined by base curvature and z position in base transform (
 corrected for misalignement ). It inherits from CylinderSurface.

 @author Sarka.Todorova@cern.ch
 */

class SlidingCylinderSurface : public CylinderSurface
{
public:
  /** Default Constructor - needed for persistency*/
  SlidingCylinderSurface();

  /** Copy Constructor*/
  SlidingCylinderSurface(const SlidingCylinderSurface& psf);

  /** Copy Constructor with shift*/
  SlidingCylinderSurface(const SlidingCylinderSurface& psf,
                         const Amg::Transform3D& transf);

  /**Constructor */
  SlidingCylinderSurface(const CylinderSurface& surf,
                         Trk::BinUtility* bu = nullptr,
                         const std::vector<float>* offset = nullptr,
                         Amg::Transform3D* align = nullptr);

  /**Destructor*/
  virtual ~SlidingCylinderSurface();

  /**Assignment operator*/
  SlidingCylinderSurface& operator=(const SlidingCylinderSurface& psf);

  /**Equality operator*/
  virtual bool operator==(const Surface& sf) const override;

  /** This method returns true if the GlobalPosition is on the Surface for both,
    within or without check of whether the local position is inside boundaries
    or not */
  virtual bool isOnSurface(const Amg::Vector3D& glopo,
                           BoundaryCheck bchk = true,
                           double tol1 = 0.,
                           double tol2 = 0.) const override;

  /** Specialized for DiscSurface: LocalToGlobal method without dynamic memory
   * allocation */
  virtual void localToGlobal(const Amg::Vector2D& locp,
                             const Amg::Vector3D& mom,
                             Amg::Vector3D& glob) const override;

  /** Specialized for DiscSurface: GlobalToLocal method without dynamic memory
   * allocation - boolean checks if on surface
   */
  virtual bool globalToLocal(const Amg::Vector3D& glob,
                             const Amg::Vector3D& mom,
                             Amg::Vector2D& loc) const override;

  /** fast straight line distance evaluation to Surface */
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir) const override;

  /** fast straight line distance evaluation to Surface - with bound option*/
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool Bound) const override;

  /**This method allows access to the bin utility*/
  const Trk::BinUtility* binUtility() const { return m_etaBin; }

  /**This method allows access to the radial offset values*/
  const std::vector<float>* offset() const { return m_depth; }

  /** Return properly formatted class name for screen output */
  virtual std::string name() const override
  {
    return "Trk::SlidingCylinderSurface";
  }

protected:
  const std::vector<float>* m_depth;
  Trk::BinUtility* m_etaBin;
  Amg::Transform3D* m_align;
};

} // end of namespace

#endif // TRKGEOMETRYSURFACES_SLIDINGCYLINDERSURFACE_H
