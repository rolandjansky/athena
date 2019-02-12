/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ForwardModuleSideGeometry.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SCT_FORWARDMODULESIDEGEOMETRY_H
#define INDETREADOUTGEOMETRY_SCT_FORWARDMODULESIDEGEOMETRY_H

#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetReadoutGeometry/SCT_ForwardPolarPosition.h"

namespace InDetDD {

  /**
     @class SCT_ForwardModuleSideGeometry
     
     Geometry descriptor holding strig number,
     crystel inner and outer half length & radius.
     
     @author Grant Gorfine
     
   */

  class SCT_ForwardModuleSideGeometry {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
    public:

      /** Constructor with parameters:
         r from inner crystal center to beam
         inner crystal half height
         r from outer crystal (if present) center to beam
         outer crystal (if present) half height
         number of strips (same for both crystals)
         strip step in angle (same for both crystals)
         number of crystals per module side */

      SCT_ForwardModuleSideGeometry(const double radius1, const double halfHeight1,
                                    const double radius2, const double halfHeight2,
                                    const int strips, const double step, 
                                    const int nCrystals);

      /** Destructor: */
      virtual ~SCT_ForwardModuleSideGeometry() = default;

      ///////////////////////////////////////////////////////////////////
      // Const methods:
      ///////////////////////////////////////////////////////////////////

      /** Nominal center radius of double wafer. */
      double radius() const;

      /** r from inner crystal center to beam:*/
      double radius1() const;

      /** inner crystal half height: */
      double halfHeight1() const;

      /** r from outer crystal (if present) center to beam: */
      double radius2() const;

      /** outer crystal (if present) half height: */
      double halfHeight2() const;

      /** number of strips (same for both crystals): */
      int strips() const;

      /** strip step in angle (same for both crystals): */
      double step() const;
      double angularPitch() const;

      /** number of crystals per module side */
      int nCrystals() const;
 
      void distanceToDetectorEdge(const SiLocalPosition & localPosition,
 			      const SCT_ForwardPolarPosition &polarPos,
			      double & etaDist, double & phiDist) const;
  
      /** Test if near bond gap within tolerances */
      bool nearBondGap(const SiLocalPosition & localPosition, double etaTol) const;
 
      /** check if position is in active area */
      bool inActiveArea(const SiLocalPosition & chargePosition, bool checkBondGap = true) const;

      /** DEPRECATED */
      bool inActiveArea(const SCT_ForwardPolarPosition &polarPos) const;
  
      /** give the strip pitch (dependence on position needed for forward)  */
      double stripPitch(const SCT_ForwardPolarPosition &polarPos) const;
  
      /** give distance to the nearest diode in units of pitch, from 0.0 to 0.5, 
          this method should be fast as it is called for every surface charge
          in the SCT_SurfaceChargesGenerator */
      double scaledDistanceToNearestDiode(const SCT_ForwardPolarPosition& polarPos) const;
 
      /** Method to calculate length of a module*/
      double length() const;

      /** Method to calculate average width of a module*/
      double width() const;

      /** Method to calculate minimum width of a module*/
      double minWidth() const;

      /** Method to calculate maximum width of a module */
      double maxWidth() const;
 
      /** give length of dead area */
      double deadAreaLength() const;

      /** give upper boundary of dead area */
      double deadAreaUpperBoundary() const;

      /** give lower boundary of dead area */
      double deadAreaLowerBoundary() const;
 
       ///////////////////////////////////////////////////////////////////
       // Private methods:
       ///////////////////////////////////////////////////////////////////
    private:

       SCT_ForwardModuleSideGeometry();

       ///////////////////////////////////////////////////////////////////
       // Private data:
       ///////////////////////////////////////////////////////////////////
    private:
      double m_radius; //!< r from nominal element center to beam
      double m_radius1; //!< r from inner crystal center to beam
      double m_halfHeight1; //!< inner crystal half height
      double m_radius2; //!< r from outer crystal (if present) center to beam
      double m_halfHeight2; //!< outer crystal (if present) half height
      int m_strips; //!< number of strips (same for both crystals) 
      double m_step; //!< strip step in angle (same for both crystals)
      int m_nCrystals; //!< number of crystals per module side
      double m_leftSide; //!< left border of module in angle
      double m_rightSide; //!< right border of module in angle
      double m_upperSide1; //!< upper side of crystal 1 
      double m_lowerSide1; //!< lower side of crystal 1 
      double m_upperSide2; //!< upper side of crystal 2 
      double m_lowerSide2; //!< lower side of crystal 2 

      double m_halfLength; //!< top = +halfLength, bot = -halfLength  
      double m_deadRegionUpper;   //!< relative to center of detector.
      double m_deadRegionLower;   //!< relative to center of detector.
      double m_tangent;  //!< tan(theta) at edge = (halfWidth) / (radius) 

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline double SCT_ForwardModuleSideGeometry::radius1() const
{
  return m_radius1;
}

inline double SCT_ForwardModuleSideGeometry::radius() const
{
  return m_radius;
}

inline double SCT_ForwardModuleSideGeometry::halfHeight1() const
{
  return m_halfHeight1;
}

inline double SCT_ForwardModuleSideGeometry::radius2() const
{
  return m_radius2;
}

inline double SCT_ForwardModuleSideGeometry::halfHeight2() const
{
  return m_halfHeight2;
}

inline int SCT_ForwardModuleSideGeometry::strips() const
{
  return m_strips;
}

inline double SCT_ForwardModuleSideGeometry::step() const
{
  return m_step;
}

inline double SCT_ForwardModuleSideGeometry::angularPitch() const
{
  return m_step;
}

inline int SCT_ForwardModuleSideGeometry::nCrystals() const
{
  return m_nCrystals;
}

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SCT_FORWARDMODULESIDEGEOMETRY_H

