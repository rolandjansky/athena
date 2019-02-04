/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_BarrelModuleSideDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SCT_BARRELMODULESIDEDESIGN_H
#define INDETREADOUTGEOMETRY_SCT_BARRELMODULESIDEDESIGN_H

// Base class
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"

namespace Trk{
 class RectangleBounds;
 class SurfaceBounds;
}

namespace InDetDD {

  class SiDiodesParameters;

   /** @class SCT_BarrelModuleSideDesign

      Barrel module design description for the SCT.

      @author Szymon Gadomski, Alessandro Fornaini
      - modified: Grant Gorfine, Andreas Salzburger
   */

  class SCT_BarrelModuleSideDesign : public SCT_ModuleSideDesign {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
    public:

      /** Copy constructor:*/
      SCT_BarrelModuleSideDesign(const SCT_BarrelModuleSideDesign &design);

      /** Constructor with parameters:
          strip pitch, 80 um in barrel SCT
          strip length
          geom position on G4 volume, care needed
          geom position on G4 volume, care needed
          includes detector edge and gap between decetors*/
      SCT_BarrelModuleSideDesign(const double thickness,
                             const int crystals,
			     const int diodes,
			     const int cells,
			     const int shift,
			     const bool swapStripReadout,
			     InDetDD::CarrierType carrierType,
			     const double stripPitch,
			     const double stripLength,
			     const double xEtaStripPatternCentre,
			     const double xPhiStripPatternCentre,
			     const double totalDeadLength,
			     int readoutSide = -1);

      /** Destructor: */
      virtual ~SCT_BarrelModuleSideDesign();

      /** Assignment operator: */
      SCT_BarrelModuleSideDesign &operator=(const SCT_BarrelModuleSideDesign &design);

      ///////////////////////////////////////////////////////////////////
      // Const methods:
      ///////////////////////////////////////////////////////////////////

      /** strip pitch */
      virtual double stripPitch() const;

      /** strip length */
      double stripLength() const;

      /** centre of the strip pattern in eta */
      double etaStripPatternCentre() const;

      /** centre of the strip pattern in phi*/
      double phiStripPatternCentre() const;

     /** Returns distance to nearest detector active edge 
         +ve = inside
         -ve = outside */
      virtual void distanceToDetectorEdge(const SiLocalPosition & localPosition, double & etaDist, double & phiDist) const;

      /** Test if near bond gap within tolerances */
      virtual bool nearBondGap(const SiLocalPosition & localPosition, double etaTol) const;

      /** check if the position is in active area */
      virtual bool inActiveArea(const SiLocalPosition &chargePos, bool checkBondGap = true) const;

      /** distance to the nearest diode in units of pitch, from 0.0 to 0.5, 
          this method should be fast as it is called for every surface charge
          in the SCT_SurfaceChargesGenerator
           an active area check, done in the Generator anyway, is removed here*/
      virtual double scaledDistanceToNearestDiode(const SiLocalPosition &chargePos) const;
  
      /** give the strip pitch (dependence on position needed for forward)  */
      virtual double stripPitch(const SiLocalPosition & chargePos) const;

      /** readout or diode id -> position, size */
      virtual SiDiodesParameters parameters(const SiCellId & cellId) const;
      virtual SiLocalPosition localPositionOfCell(const SiCellId & cellId) const;
      virtual SiLocalPosition localPositionOfCluster(const SiCellId & cellId, int clusterSize) const;

      /** position -> id */
      virtual SiCellId cellIdOfPosition(const SiLocalPosition & localPos) const;
    
      /** this method returns the ends of the strip */
      virtual std::pair<SiLocalPosition,SiLocalPosition> endsOfStrip(const SiLocalPosition &position) const;

      /** method for stereo angle computation - returns a vector parallel to the strip being hit */
      virtual HepGeom::Vector3D<double> phiMeasureSegment(const SiLocalPosition & ) const;

      /** Method to calculate length of a module*/
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

      /* Pitch in phi direction */
      virtual double phiPitch(const SiLocalPosition & localPosition) const;

      /** Pitch in eta direction */
      virtual double etaPitch() const;

      /** Return the surface bounds */
      virtual const Trk::SurfaceBounds & bounds() const; 

  private:

      SCT_BarrelModuleSideDesign();

      ///////////////////////////////////////////////////////////////////
      // Private data:
      ///////////////////////////////////////////////////////////////////

  private:
      double m_stripPitch; //!< strip pitch, 80 um in barrel SCT
      double m_stripLength; //!< strip length
      double m_xEtaStripPatternCentre; //!< geom position on G4 volume, care needed
      double m_xPhiStripPatternCentre; //!< geom position on G4 volume, care needed
      double m_totalDeadLength; //!< includes detector edge and gap between decetors
      double m_xPhiAbsSize; //!< size in xPhi/2 for active area
      double m_xEtaAbsSizeLow, m_xEtaAbsSizeHigh; //!< active area in xEta
      Trk::RectangleBounds * m_bounds; //!< surface bound description
  };

  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
  inline double SCT_BarrelModuleSideDesign::stripPitch() const
  {
    return m_stripPitch;
  }

  // this method is non trivial in forward, but in barrel it can
  // be an inline function
  inline double SCT_BarrelModuleSideDesign::stripPitch(const SiLocalPosition &) const
  {
    return m_stripPitch;
  }


  inline double SCT_BarrelModuleSideDesign::stripLength() const
  {
    return m_stripLength;
  }

  inline double SCT_BarrelModuleSideDesign::etaStripPatternCentre() const
  {
    return m_xEtaStripPatternCentre;
  }

  inline double SCT_BarrelModuleSideDesign::phiStripPatternCentre() const
  {
    return  m_xPhiStripPatternCentre;
  }

  // Same as stripPitch
  inline double SCT_BarrelModuleSideDesign::phiPitch(const SiLocalPosition &) const
  {
    return m_stripPitch;
  }

  // Same as stripPitch
  inline double SCT_BarrelModuleSideDesign::phiPitch() const
  {
    return m_stripPitch;
  }

  // Same as active length
  inline double SCT_BarrelModuleSideDesign::etaPitch() const
  {
    return length();
  }

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SCT_BARRELMODULESIDEDESIGN_H

