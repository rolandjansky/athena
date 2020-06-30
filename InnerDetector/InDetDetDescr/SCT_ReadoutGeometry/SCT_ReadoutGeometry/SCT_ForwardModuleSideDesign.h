/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ForwardModuleSideDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SCT_FORWARDMODULESIDEDESIGN_H
#define INDETREADOUTGEOMETRY_SCT_FORWARDMODULESIDEDESIGN_H

// Base class
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

//Data member classes
#include "SCT_ReadoutGeometry/SCT_ForwardFrameTransformation.h"
#include "SCT_ReadoutGeometry/SCT_ForwardModuleSideGeometry.h"
#include "TrkSurfaces/TrapezoidBounds.h"

namespace Trk{
  class SurfaceBounds;
}


namespace InDetDD {

  /** 
  @class SCT_ForwardModuleSideDesign

   Design descriptor for forward modules in the SCT,
   carries the bounds, and readout information.

  @author Grant Gorfine
  - modified & maintained: Nick Styles, Andreas Salzburger
  */

  class SCT_ForwardModuleSideDesign : public SCT_ModuleSideDesign {

  public:

     /** Constructor with parameters:
         local axis corresponding to eta direction
         local axis corresponding to phi direction
         local axis corresponding to depth direction
         thickness of silicon sensor
         number of crystals within module side
         number of diodes within crystal
         number of cells within module side
         index of diode connected to cell with index 0
         r from inner crystal center to beam
         inner crystal half height
         r from outer crystal (if present) center to beam
         outer crystal (if present) half height
         strip step in angle (same for both crystals)
         eta coordinate of crystal center
        phi coordinate of crystal center */
      SCT_ForwardModuleSideDesign(const double thickness,
			      const int crystals,const int diodes,
			      const int cells,const int shift,
			      const bool swapStripReadout,
			      InDetDD::CarrierType carrierType,
			      const double radius1,const double halfHeight1,
			      const double radius2,const double halfHeight2,
			      const double step,
			      const double etaCenter,const double phiCenter,
			      int readoutSide = -1);

      /** Destructor: */
      virtual ~SCT_ForwardModuleSideDesign() = default;

      ///////////////////////////////////////////////////////////////////
      // Const methods:
      ///////////////////////////////////////////////////////////////////

      /** Returns distance to nearest detector active edge 
          +ve = inside
          -ve = outside */
      virtual void distanceToDetectorEdge(const SiLocalPosition & localPosition, 
      				      double & etaDist, double & phiDist) const;

      /** Test if near bond gap within tolerances */
      virtual bool nearBondGap(const SiLocalPosition & localPosition, double etaTol) const;
 
      /** check if the position is in active area */
      virtual bool inActiveArea(const SiLocalPosition &chargePos, bool checkBondGap = true) const;

      /** give distance to the nearest diode in units of pitch, from 0.0 to 0.5, 
           this method should be fast as it is called for every surface charge
           in the SCT_SurfaceChargesGenerator */
      virtual double scaledDistanceToNearestDiode(const SiLocalPosition &chargePos) const;
  
      /** give the strip pitch (dependence on position needed for forward) */
      virtual double stripPitch(const SiLocalPosition & chargePos) const;

      /** give the strip pitch (For Forward returns pitch at center) */
      virtual double stripPitch() const;

      /** readout or diode id -> position, size */
      virtual SiDiodesParameters parameters(const SiCellId & cellId) const;
      virtual SiLocalPosition localPositionOfCell(const SiCellId & cellId) const;
      virtual SiLocalPosition localPositionOfCluster(const SiCellId & cellId, int clusterSize) const;

      /** position -> id */
      virtual SiCellId cellIdOfPosition(const SiLocalPosition & localPos) const;

      /** this method returns the ends of the strip */
      virtual std::pair<SiLocalPosition,SiLocalPosition> endsOfStrip(const SiLocalPosition &position) const;
  
      /** method for stereo angle computation - returns a vector parallel to the strip being hit */
      virtual HepGeom::Vector3D<double> phiMeasureSegment(const SiLocalPosition &position) const;

      /** Nominal center radius of double wafer. */
      double radius() const;

      /** Method to calculate length of a module */
      virtual double length() const;

      /** Method to calculate average width of a module */
      virtual double width() const;

      /** Method to calculate minimum width of a module */
      virtual double minWidth() const;

      /** Method to calculate maximum width of a module */
      virtual double maxWidth() const;
  
      /** give length of dead area */
      virtual double deadAreaLength() const;
  
      /** give upper boundary of dead area */
      virtual double deadAreaUpperBoundary() const;
  
      /** give lower boundary of dead area */
      virtual double deadAreaLowerBoundary() const;
 
      /** Pitch in phi direction */
      virtual double phiPitch() const;

      /** Pitch in phi direction */
      virtual double phiPitch(const SiLocalPosition & localPosition) const;

      /** Pitch in eta direction */
      virtual double etaPitch() const;

      /** Angular pitch */
      double angularPitch() const;

      /** Shape of element */
      virtual DetectorShape shape() const;

      /** Element boundary */
      virtual const Trk::SurfaceBounds & bounds() const; 

    private:

       /** Forbidden default constructor */
       SCT_ForwardModuleSideDesign();
  
       /** Copy constructor: */
       SCT_ForwardModuleSideDesign(const SCT_ForwardModuleSideDesign &design);

       /** Assignment operator: */
       SCT_ForwardModuleSideDesign &operator=(const SCT_ForwardModuleSideDesign &design);

        ///////////////////////////////////////////////////////////////////
        // Private data:
        ///////////////////////////////////////////////////////////////////

    private:
       SCT_ForwardModuleSideGeometry m_geometry; //!< geometry of module side
       SCT_ForwardFrameTransformation m_frame; //!< polar / cartesian frame transformation
       Trk::TrapezoidBounds m_bounds;
    };
    
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
  
  
  inline double SCT_ForwardModuleSideDesign::radius() const
  {
    return m_geometry.radius();
  }
  
  // Same as stripPitch
  inline double SCT_ForwardModuleSideDesign::phiPitch(const SiLocalPosition & localPosition) const
  {
    return stripPitch(localPosition);
  }
  
  // Same as stripPitch
  inline double SCT_ForwardModuleSideDesign::phiPitch() const
  {
    return stripPitch();
  }
  
  // Same as active length
  inline double SCT_ForwardModuleSideDesign::etaPitch() const
  {
    return length();
  }
  
  // Same as active length
  inline double SCT_ForwardModuleSideDesign::angularPitch() const
  {
    return m_geometry.angularPitch();
  }

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SCT_FORWARDMODULESIDEDESIGN_H
