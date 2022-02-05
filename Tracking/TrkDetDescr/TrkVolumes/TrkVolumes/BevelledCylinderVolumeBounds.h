/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BevelledCylinderVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BEVELLEDCYLINDERVOLUMESBOUNDS_H
#define TRKVOLUMES_BEVELLEDCYLINDERVOLUMESBOUNDS_H

// Trk
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/BevelledCylinderVolumeBoundaryAccessors.h"
// Eigen
#include "GeoPrimitives/GeoPrimitives.h"
#include "CxxUtils/CachedUniquePtr.h"

class MsgStream;


namespace Trk {

   /** @struct BevelledBoundaryIntersector
          - identical code to TrkExUtils/RealLinearEquation (but forbidden dependency!)
      */
   struct BevelledBoundaryIntersector {

   double yOfX;            //!< the result of x
   double segLength;      //!< length of the line segment

    BevelledBoundaryIntersector(double px, double py, double k, double xprime) {
      double deltax = xprime-px;
      yOfX = py + k*(deltax);
      double deltay = yOfX-py;
      segLength = sqrt(deltax*deltax+deltay*deltay);
    }

   };

   class Surface;
   class EllipseBounds;
   class TrapezoidBounds;
   class RectangleBounds;
   class CylinderBounds;
   class DiscBounds;

  /**
   @class BevelledCylinderVolumeBounds

   Bounds for a cylindrical Volume, the decomposeToSurfaces method creates a
   vector of up to 6 surfaces:

   case A) 3 Surfaces (full cylindrical tube):
    BoundarySurfaceFace [index]:
        - negativeFaceXY [0] : Trk::DiscSurface with \f$ r_{inner}=0 \f$,
                               parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY [1] : Trk::DiscSurface with \f$ r_{inner}=0 \f$,
                               parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - cylinderCover  [2] : Trk::CylinderSurface confining the Trk::Volume

   case B) 4 Surfaces (tube with inner and outer radius):
    BoundarySurfaceFace [index]:
        - negativeFaceXY [0] : Trk::DiscSurface with \f$ r_{inner}>0 \f$,
                               parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY [1] : Trk::DiscSurface with \f$ r_{inner}>0 \f$,
                               parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - tubeOuterCover [2] : Trk::CylinderSurface with \f$ r = r_{outer} \f$
        - tubeInnerCover [3] : Trk::CylinderSurface with \f$ r = r_{inner} \f$

   case C) 6 Surfaces (sectoral tube with inner and outer radius):
    BoundarySurfaceFace [index]:
        - negativeFaceXY        [0] : Trk::DiscSurface with \f$ r_{inner}>0  \f$ and \f$ \phi < \pi \f$,
                                      parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY        [1] : Trk::DiscSurface with \f$ r_{inner}>0 \f$ and \f$ \phi < \pi \f$,
                                      parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - tubeSectorOuterCover  [2] : Trk::CylinderSurface with \f$ r = r_{outer} \f$
        - tubeSectorInnerCover  [3] : Trk::CylinderSurface with \f$ r = r_{inner} \f$
        - tubeSectorNegativePhi [4] : Rectangular Trk::PlaneSurface attached to [0] and [1] at negative \f$ \phi \f$
        - tubeSectorNegativePhi [5] : Rectangular Trk::PlaneSurface attached to [0] and [1] at positive \f$ \phi \f$

   case D) 6 Surfaces (sectoral bevelled tube with inner and/or outer radius replaced by plane surface):
    BoundarySurfaceFace [index]:
        - negativeFaceXY        [0] : Trk::DiscSurface with \f$ r_{inner}>0  \f$ and \f$ \phi < \pi \f$,
                                      parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY        [1] : Trk::DiscSurface with \f$ r_{inner}>0 \f$ and \f$ \phi < \pi \f$,
                                      parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - tubeSectorOuterCover  [2] : Trk::CylinderSurface with \f$ r = r_{outer} \f$
                                   OR rectangular plane surface
        - tubeSectorInnerCover  [3] : Trk::CylinderSurface with \f$ r = r_{inner} \f$
                                   OR rectangular plane surface
        - tubeSectorNegativePhi [4] : Rectangular Trk::PlaneSurface attached to [0] and [1] at negative \f$ \phi \f$
        - tubeSectorNegativePhi [5] : Rectangular Trk::PlaneSurface attached to [0] and [1] at positive \f$ \phi \f$

    @image html CylinderVolumeBounds_decomp.gif

    @author Andreas.Salzburger@cern.ch
    */

 class BevelledCylinderVolumeBounds : public VolumeBounds {

  public:
    /**Default Constructor*/
    BevelledCylinderVolumeBounds();

    /**Constructor - cylinder segment bevelled in R */
    BevelledCylinderVolumeBounds(double rinner, double router, double halfPhiSector, double halez, int type);

    /**Copy Constructor */
    BevelledCylinderVolumeBounds(const BevelledCylinderVolumeBounds& cylbo);

    /**Destructor */
    virtual ~BevelledCylinderVolumeBounds();

    /**Assignment operator*/
    BevelledCylinderVolumeBounds& operator=(const BevelledCylinderVolumeBounds& cylbo);

    /**Virtual constructor */
    BevelledCylinderVolumeBounds* clone() const override;

    /**This method checks if position in the 3D volume frame is inside the cylinder*/
    bool inside(const Amg::Vector3D& , double tol=0.) const override;

    /** Method to decompose the Bounds into boundarySurfaces */
    const std::vector<const Trk::Surface*>* decomposeToSurfaces (const Amg::Transform3D& transform) override;

    /** Provide accessor for BoundarySurfaces */
    ObjectAccessor boundarySurfaceAccessor(const Amg::Vector3D& gp,
                                           const Amg::Vector3D& dir,
                                           bool forceInside=false) const override;

    /**This method returns the inner radius*/
    double innerRadius() const;

    /**This method returns the outer radius*/
    double outerRadius() const;

    /**This method returns the medium radius*/
    double mediumRadius() const;

    /**This method returns the delta radius*/
    double deltaRadius() const;

    /**This method returns the halfPhiSector angle*/
    double halfPhiSector() const;

    /**This method returns the halflengthZ*/
    double halflengthZ() const;

     /**This method returns the thetaMinus*/
    double thetaMinus() const;

     /**This method returns the thetaPlus*/
    double thetaPlus() const;

     /**This method returns the type*/
    int type() const;

    /** Output Method for MsgStream*/
    MsgStream& dump(MsgStream& sl) const override;

    /** Output Method for std::ostream */
    std::ostream& dump(std::ostream& sl) const override;

  private:
    /** This method returns the associated BevelledCylinderBounds of the inner BevelledCylinderSurfaces. */
    CylinderBounds* innerBevelledCylinderBounds() const;
    /** This method returns the associated BevelledCylinderBounds of the outer BevelledCylinderSurfaces. */
    CylinderBounds* outerBevelledCylinderBounds() const;
    /** This method returns the associated plane surface bounds of the inner bevelled surface*/
    RectangleBounds* innerBevelledPlaneBounds() const;
    /** This method returns the associated BevelledCylinderBounds of the outer BevelledCylinderSurfaces. */
    RectangleBounds* outerBevelledPlaneBounds() const;
    /** This method returns the associated EllipseBounds for the bottom/top EllipseSurface. */
    EllipseBounds* bottomEllipseBounds() const;
    /** This method returns the associated EllipseBounds for the bottom/top EllipseSurface. */
    EllipseBounds* topEllipseBounds() const;
    /** This method returns the associated CylinderBounds of the inner CylinderSurfaces. */
    CylinderBounds* innerCylinderBounds() const;
    /** This method returns the associated CylinderBounds of the outer CylinderSurfaces. */
    CylinderBounds* outerCylinderBounds() const;
    /** This method returns the associated DiscBounds for the bottom/top DiscSurface. */
    DiscBounds* discBounds() const;
    /** This method returns the bevelled area volume. */
    Volume* subtractedVolume() const;
    /** This method returns the associated PlaneBounds limiting a sectoral BevelledCylinderVolume. */
    TrapezoidBounds* sectorTrdBounds() const;
    RectangleBounds* sectorPlaneBounds() const;
    /** Private method to construct the accessors */
    void createBoundarySurfaceAccessors();

#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif
    double m_innerRadius;
    double m_outerRadius;
    double m_halfPhiSector;
    double m_halfZ;
    double m_thetaMinus;
    double m_thetaPlus;
    int    m_type;
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif

    /** Accessors for Boundary surface access - static is not possible due to
       mismatched delete() / free () with TrkMagFieldUtils
      */
    BevelledCylinderVolumeBoundaryAccessors            m_boundaryAccessors;

    /** numerical stability */
    static const double s_numericalStable;

    CxxUtils::CachedUniquePtr<Trk::Volume> m_subtractedVolume;
 };

 inline BevelledCylinderVolumeBounds* BevelledCylinderVolumeBounds::clone() const
 { return new BevelledCylinderVolumeBounds(*this); }

 inline bool BevelledCylinderVolumeBounds::inside(const Amg::Vector3D &pos, double tol) const
 {
   double ros = pos.perp();
   bool insidePhi =  fabs(pos.phi()) <= m_halfPhiSector + tol;
   double cphi = cos(pos.phi());
   bool insideR = insidePhi;
   if (m_type < 1)  insideR  = insidePhi ? ((ros >=  m_innerRadius - tol ) && (ros <= m_outerRadius + tol)) : false;
   else if (m_type == 1 ) insideR = insidePhi ? ((ros>= m_innerRadius/cphi-tol)&&(ros<=m_outerRadius+tol)):false;
   else if (m_type == 2 ) insideR = insidePhi ? ((ros>= m_innerRadius-tol)&&(ros<=m_outerRadius/cphi+tol)):false;
   else if (m_type == 3 ) insideR = insidePhi ? ((ros>= m_innerRadius/cphi-tol)&&(ros<=m_outerRadius/cphi+tol)):false;
//   bool insideZ = insideR ? (fabs(pos.z()) <= m_halfZ + tol ) : false ;
   bool insideZ = insideR ? ((pos.z()<=m_halfZ-(pos.x()+m_outerRadius)*tan(m_thetaPlus) + tol)
			     && ( pos.z()>=-m_halfZ+(pos.x()+m_outerRadius)*tan(m_thetaMinus) - tol)) : false ;

   return insideZ;
 }

 inline double BevelledCylinderVolumeBounds::innerRadius() const { return m_innerRadius; }
 inline double BevelledCylinderVolumeBounds::outerRadius() const { return m_outerRadius; }
 inline double BevelledCylinderVolumeBounds::mediumRadius() const { return 0.5*(m_innerRadius+m_outerRadius); }
 inline double BevelledCylinderVolumeBounds::deltaRadius() const { return (m_outerRadius-m_innerRadius); }
 inline double BevelledCylinderVolumeBounds::halfPhiSector() const { return m_halfPhiSector; }
 inline double BevelledCylinderVolumeBounds::halflengthZ() const { return m_halfZ; }
 inline double BevelledCylinderVolumeBounds::thetaMinus() const { return m_thetaMinus; }
 inline double BevelledCylinderVolumeBounds::thetaPlus() const { return m_thetaPlus; }
 inline int BevelledCylinderVolumeBounds::type() const { return m_type; }

// inline EllipseBounds* BevelledCylinderVolumeBounds::topEllipseBounds() const { return this->topEllipseBounds(); }
// inline EllipseBounds* BevelledCylinderVolumeBounds::bottomEllipseBounds() const { return this->bottomEllipseBounds(); }

}


#endif // TRKVOLUMES_BEVELLEDCYLINDERVOLUMESBOUNDS_H



