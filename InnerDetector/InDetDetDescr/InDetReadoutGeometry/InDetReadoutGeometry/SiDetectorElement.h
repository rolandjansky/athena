/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiDetectorElement.h
**/

#ifndef INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
#define INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H

// Base class.
#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"

// Data member classes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "ReadoutGeometryBase/SiIntersect.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "ReadoutGeometryBase/SiCellId.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

class AtlasDetectorID;
class GeoVFullPhysVol;

namespace Trk{
 class Surface;
 class SurfaceBounds;
}

namespace InDetDD {

    class SiReadoutCellId;

  /**
  
   @class SiDetectorElement
  
   Class to hold geometrical description of a silicon detector element. 
   A detector element is a module in the pixels and one side of a module in the SCT.
   This class is shared between the Pixel and SCT detector since there is a lot of commonality.
    
   @par Coordinate Frames.
  
   The following coordinate frames are used in these elements.
  
    - Global frame:\n
         Currently global frame in G4/GeoModel. Probably eventually
         will be global frame most suitable for reconstruction 
         (eg solenoid axis).
  
    - Local hit frame:\n 
         Local frame for hits. It is the same as local frame in G4 and GeoModel. 
         I also refer to this as the local simulation frame. 
         By convention elements are orientated such that:
            - hitDepth = local x
            - hitPhi   = local y
            - hitEta   = local z
            . 
         Directions of these correspond to the physical wafer. Consequently hitDepth and hitPhi axes go in 
         different directions depending on the orientation of the module.
         The readout side is determined from design()->readoutSide(). 
  
   - Local reconstruction frame:\n 
            - distPhi  = local x
            - distEta  = local y  
            - distDepth = local z
            .
         The directions of the axes are defined as
            - distPhi in direction of increasing phi
            - distEta in direction of increasing z in barrel and increasing r in endcap.
            - distDepth (normal) choosen to give right-handed coordinate. 
               =>  away from intersection point for barrel, decreasing z for endcap
     
   @par Overview of Methods 
  
   Methods are grouped into the the following categories
  
      -  Identification
      -  Navigation
      -  Transformation/Orientation
      -  Module Frame
      -  Element Extent
      -  Design methods
      -  Intersection Tests
      -  Lorentz Correction
      -  Readout cell id
      -  Miscellaneous
      -  Cache handling.
  
   @author Grant Gorfine
   - modified & maintained: Nick Styles, Andreas Salzburger
   - modified Nigel Hessey: get directions from the design instead of hard-wiring them   

  */  

  class SiDetectorElement : public SolidStateDetectorElementBase {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
    public:
    
    
      /// Constructor:
      SiDetectorElement(const Identifier &id, 
    		    const SiDetectorDesign *design,
    		    const GeoVFullPhysVol *geophysvol,
    		    SiCommonItems * commonItems);
    
      /// Destructor:
      virtual ~SiDetectorElement();
    
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Identification
      /// Methods to identify the element and identifier manipulation.
      // 
      ///////////////////////////////////////////////////////////////////
    
      //@{
    
      bool isPixel() const;
      bool isSCT() const;
      bool isBarrel() const;
      bool isEndcap() const;
      bool isBlayer() const;
      bool isInnermostPixelLayer() const;
      bool isNextToInnermostPixelLayer() const;
      int  getPixelLayer() const;
      bool isInclined() const;
      bool isBarrelRing() const;
      bool isDBM() const;

      // Identifier <-> SiCellId (ie strip number or pixel eta_index,phi_index)
      // satisfy SolidStateDetectorElementBase interface
    
      /// Identifier from SiCellId (ie strip number or pixel eta_index,phi_index)
      Identifier identifierFromCellId(const SiCellId & cellId) const;
    
      /// SiCellId from Identifier
      SiCellId   cellIdFromIdentifier(const Identifier & identifier) const;
      
      //@}
    
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Navigation
      /// Methods to access neighbours. 
      //
      ///////////////////////////////////////////////////////////////////
    
      //@{
      const SiDetectorElement * nextInEta() const;
      const SiDetectorElement * prevInEta() const;
      const SiDetectorElement * nextInPhi() const;
      const SiDetectorElement * prevInPhi() const;
      const SiDetectorElement * otherSide() const; // Useful for SCT only
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Transformation/Orientation
      //
      ///////////////////////////////////////////////////////////////////
    
      //@{

      /// Compute sin(tilt angle) at a given position:
      /// at center
      double sinTilt() const; // At center
      /// at given local position
      double sinTilt(const Amg::Vector2D &localPos) const;
      /// at given global position
      double sinTilt(const HepGeom::Point3D<double> &globalPosition) const;
    
      /// Compute sin(stereo angle) at a given position: at center
      double sinStereo() const; // At center
      /// at given local position
      double sinStereo(const Amg::Vector2D &localPos) const;
      /// at given global position
      double sinStereo(const HepGeom::Point3D<double> &globalPosition) const;

      /// Check if it is the stereo side (useful for SCT) 
      bool isStereo() const;
    
      /// Angle of strip in local frame with respect to the etaAxis. 
      /// Zero for all elements except trapezoidal detectors (ie SCT forward modules).
      double sinStereoLocal(const Amg::Vector2D &localPos) const;
      /// See previous method
      double sinStereoLocal(const HepGeom::Point3D<double> &globalPos) const;
    
      //@}

      /** Returns the full list of surfaces associated to this detector element */
      virtual const std::vector<const Trk::Surface*>& surfaces() const;

      /**
      
      @name Module Frame 
      Methods to help work with the module frame. 
      This is mainly of of use in the SCT where the module frame can
      in general be different from the element frame. It is actully
      defined as the frame of one of the sides (currently the axial
      side), but using these methods one does not need to make any
      assumptions about what the actual frame is.  In the following
      methods the local element frame is the local reconstruction
      frame of this element.
      */
      
      //@{
    
      /// Module to global frame transform. 
      /// Includes misalignment. The module frame is defined to be the
      /// local reconstruction frame of the axial layer in the SCT. For
      /// the pixel it is the same as the element.
      //const HepGeom::Transform3D & moduleTransform() const; 
      const Amg::Transform3D & moduleTransform() const;
    
      /// Default module to global frame transform, ie with no misalignment. 
      /// The module frame is defined to be the
      /// local reconstruction frame of the axial layer in the SCT. For
      /// the pixel it is the same as the element.
      Amg::Transform3D defModuleTransform() const;
      
    
      /// Take a transform of the local element frame and return its equivalent in the module frame.
      //HepGeom::Transform3D localToModuleFrame(const HepGeom::Transform3D & localTransform) const;
      Amg::Transform3D localToModuleFrame(const Amg::Transform3D & localTransform) const;
    
      /// Transformation from local element to module frame.  This can be
      /// used to take a local position in the element frame and transform
      /// it to a position in the module frame. If one is already in the
      /// module frame it will return the Identity transform.
      //HepGeom::Transform3D localToModuleTransform() const;
      Amg::Transform3D localToModuleTransform() const;
    
      /// Check if the element and module frame are the same.
      bool isModuleFrame() const;
    
    
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Element Extent
      /// Methods to get extent of element in r,phi and z.  
      ///////////////////////////////////////////////////////////////////
    
      //@{
      double get_rz() const;
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Design methods
      //
      ///////////////////////////////////////////////////////////////////
      //@{
    
      /// access to the local description:
      const SiDetectorDesign &design() const;
    
      InDetDD::CarrierType carrierType() const; // carrier type for readout. ie holes for SCT 
                                                // and electrons for pixels.
    
      // Pitch 
      //
      // NOTE: phiPitch is ambiguous for the Forward SCT where it varies along the strip.
      //       etaPitch is ambiguous for the pixel which has long pixels between FE chips.
      //
      // For these cases:
      //
      // phiPitch:  For SCT Forward returns pitch at center.
      // etaPitch:  For pixel returns average pitch. (Active_length/number_of_cells)
      //
      // All return pitch in distance units. 
      //
      double phiPitch() const;
      double phiPitch(const Amg::Vector2D &) const; // Useful for SCT Forward.
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Intersection Tests
      //
      ///////////////////////////////////////////////////////////////////
    
      //@{
      // Test if near bond gap within tolerances
      bool nearBondGap(Amg::Vector2D localPosition, double etaTol) const;
      bool nearBondGap(HepGeom::Point3D<double> globalPosition, double etaTol) const;
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Lorentz Correction
      //
      ///////////////////////////////////////////////////////////////////
      //@{
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Readout cell id
      /// Cell id's are the strip number in SCT and phi_index,eta_index in the pixel
      /// as defined in the offline identifier. Their direction runs in the distPhi, distEta
      /// direction in the Reconstruction local frame. 
      ///
      /// For methods taking a SiCellId (basically phi,eta index for pixel or strip for SCT) you
      /// can do the following fro example:
      ///   - For pixel\n 
      ///      localPositionOfCell(SiCellId(phi_index,eta_index));\n
      ///   - For SCT\n
      ///      localPositionOfCell(SiCellId(strip));\n
      //
      ///////////////////////////////////////////////////////////////////
      //@{
    
      /// Determine if readout direction between online and offline needs swapping.
      /// Returns true if online and offline numbers run in opposite directions.
      /// For strip/phi_index
      bool swapPhiReadoutDirection() const;
      /// For eta_index (only relevant for pixel)
      bool swapEtaReadoutDirection() const;
    
      /// If cell is ganged return the id of the other cell which shares the readout
      /// for this cell, otherwise return an invalid id.
      /// This is a more convenient (and slightly faster) alternative than 
      /// using the above two methods.
      /// Only relevant for pixel. For SCT always returns an invalid ID.
      SiCellId gangedCell(const SiCellId & cellId) const;
    
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Miscellaneous
      //
      ///////////////////////////////////////////////////////////////////
      //@{
      /// Special method for SCT to retrieve the two ends of a "strip"
      /// Returned coordinates are in global frame.
      //std::pair<HepGeom::Point3D<double>,HepGeom::Point3D<double> > endsOfStrip(const Amg::Vector2D &position) const;
      std::pair<Amg::Vector3D,Amg::Vector3D > endsOfStrip(const Amg::Vector2D &position) const;
      //@}
    
      ///////////////////////////////////////////////////////////////////
      //
      /// @name Cache handling.
      //
      ///////////////////////////////////////////////////////////////////
      //@{.
      //   - Methods to handle invalidating and updating caches. The cached values include values that are affected by alignment
      //     Surface are only created on demand.  The method updateAllCaches also creates the surfaces as well as calling updateCache.
      //     Conditions cache contains Lorentz angle related quantities.
    
      /// Recalculate all cached values. 
      void updateCache() const;
    
      /// Update all caches including surfaces.
      void updateAllCaches() const;
    
      //@}
    
      //////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////////////////
    
    public:
    
    
      ///////////////////////////////////////////////////////////////////
      /// @name Non-const methods:
      /// These are not needed by most clients.
      ///////////////////////////////////////////////////////////////////
      //@{
      // Set neighbours. 
    
      void setNextInEta(const SiDetectorElement *);
      void setPrevInEta(const SiDetectorElement *);
      void setNextInPhi(const SiDetectorElement *);
      void setPrevInPhi(const SiDetectorElement *);
      void setOtherSide(const SiDetectorElement *); // For SCT only
    
      //@}

      ///////////////////////////////////////////////////////////////////
      // Private methods:
      ///////////////////////////////////////////////////////////////////
    private:
      // Don't allow copying.
      SiDetectorElement();
      SiDetectorElement(const SiDetectorElement&);
      SiDetectorElement &operator=(const SiDetectorElement&);

      ///////////////////////////////////////////////////////////////////
      // Protected data:
      ///////////////////////////////////////////////////////////////////
    protected:
      const SiDetectorElement * m_nextInEta;
      const SiDetectorElement * m_prevInEta;
      const SiDetectorElement * m_nextInPhi;
      const SiDetectorElement * m_prevInPhi;
      const SiDetectorElement * m_otherSide;
    
      bool m_isPixel;
      bool m_isBarrel;
      bool m_isDBM;
      
      // cached values for pixel detector elements
      bool m_isInclined;
      bool m_isInnermostPixel;
      bool m_isNextToInnermostPixel;
      
      //
      // Cached values.
      //
      mutable bool m_cacheValid; // used only for SiDetectorElement specific cache values
      mutable bool m_firstTime;
      mutable bool m_isStereo;
      mutable std::vector<const Trk::Surface*> m_surfaces;

    };
    
    ///////////////////////////////////////////////////////////////////
    // Inline methods:
    ///////////////////////////////////////////////////////////////////
    
    inline bool SiDetectorElement::isEndcap() const
    {
      return (!isBarrel()&&!isDBM());
    }

    inline const SiDetectorDesign &SiDetectorElement::design() const
    {
      return *dynamic_cast<const SiDetectorDesign *>(m_design);
    }

    inline InDetDD::CarrierType SiDetectorElement::carrierType() const
    {
      return dynamic_cast<const SiDetectorDesign *>(m_design)->carrierType();
    }
    
    inline SiCellId SiDetectorElement::gangedCell(const SiCellId & cellId) const
    {
      return dynamic_cast<const SiDetectorDesign *>(m_design)->gangedCell(cellId);
    }

    
    inline void SiDetectorElement::updateAllCaches() const
    {
      if (!m_cacheValid) updateCache();
      if (!m_conditionsCacheValid) updateConditionsCache();
      if (!m_surface) surface();
    }


    inline const SiDetectorElement * SiDetectorElement::nextInEta() const
    {
      return m_nextInEta;
    }
    
    inline const SiDetectorElement * SiDetectorElement::prevInEta() const
    {
      return m_prevInEta;
    }
    
    inline const SiDetectorElement * SiDetectorElement::nextInPhi() const
    {
      return m_nextInPhi;
    }
    
    inline const SiDetectorElement * SiDetectorElement::prevInPhi() const
    {
      return m_prevInPhi;
    }
    
    inline const SiDetectorElement * SiDetectorElement::otherSide() const
    {
      return m_otherSide;
    }
    
    inline void SiDetectorElement::setNextInEta(const SiDetectorElement * element)
    {
      m_nextInEta = element;
    }
    
    inline void SiDetectorElement::setPrevInEta(const SiDetectorElement * element)
    {
      m_prevInEta = element;
    }
    
    inline void SiDetectorElement::setNextInPhi(const SiDetectorElement * element)
    {
      m_nextInPhi = element;
    }
    
    inline void SiDetectorElement::setPrevInPhi(const SiDetectorElement * element)
    {
      m_prevInPhi = element;
    }
    
    inline void SiDetectorElement::setOtherSide(const SiDetectorElement * element) // For SCT only
    {
      m_otherSide = element;
    }

    // implemented so that base class method isn't hidden
    // by phiPitch(const Amg::Vector2D &)
    inline double SiDetectorElement::phiPitch() const
    {
    return SolidStateDetectorElementBase::phiPitch();
    }

    inline double SiDetectorElement::phiPitch(const Amg::Vector2D & localPosition) const
    {
      return m_design->phiPitch(localPosition);
    }
    
    inline bool SiDetectorElement::swapPhiReadoutDirection() const
    {
      // equivalent to (dynamic_cast<const SiDetectorDesign *>(m_design)->swapHitPhiReadoutDirection() xor !m_phiDirection)
      return ((!dynamic_cast<const SiDetectorDesign *>(m_design)->swapHitPhiReadoutDirection() && !m_phiDirection)
    	  || (dynamic_cast<const SiDetectorDesign *>(m_design)->swapHitPhiReadoutDirection() && m_phiDirection));
    }
    
    inline bool SiDetectorElement::swapEtaReadoutDirection() const
    {
      // equivalent to (dynamic_cast<const SiDetectorDesign *>(m_design)->swapHitEtaReadoutDirection() xor !m_etaDirection)
      return ((!dynamic_cast<const SiDetectorDesign *>(m_design)->swapHitEtaReadoutDirection() && !m_etaDirection)
    	  || (dynamic_cast<const SiDetectorDesign *>(m_design)->swapHitEtaReadoutDirection() && m_etaDirection));
    }

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H

