/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiDetectorElement.h
 **/

#ifndef INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
#define INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H

// Base class.
#include "TrkDetElementBase/TrkDetElementBase.h"

// Data member classes
#include "CxxUtils/CachedUniquePtr.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiIntersect.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include "CLHEP/Geometry/Point3D.h"

#include <atomic>
#include <mutex>

class AtlasDetectorID;
class GeoAlignmentStore;
class GeoVFullPhysVol;

namespace Trk {
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

  /**
   * Some notes on Thread safety for  AthenaMT
   * The private methods of this class do not have locks
   * 
   * The method updateCache of is of particular interest as 
   * it set all cache values and at the end sets the  
   * m_cacheValid atomic variable to true.
   *
   * The const methods call the updateCache() under a mutex lock
   * when the need to perform  lazy initialization
   * std::lock_guard<std::mutex> lock(m_mutex);
   *    if (!m_cacheValid) updateCache();
   *
   * So as concurrent const operations are valid 
   * and do not race with each other. 
   *
   * The non-const methods do not use a mutex lock. They can set the state
   * of the cache or the cache itself (invalidate/setCache methods etc)
   *
   * Note: Synchronisation of creating SiDetElements for different events 
   * and accessing for each events 
   * can be done via write/read handles or similar EventContext aware 
   * framework machinery. 
   */
 

  class SiDetectorElement : public Trk::TrkDetElementBase {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:
    
    
    /// Constructor:
    SiDetectorElement(const Identifier& id, 
                      const SiDetectorDesign* design,
                      const GeoVFullPhysVol* geophysvol,
                      const SiCommonItems* commonItems,
                      const GeoAlignmentStore* geoAlignStore=nullptr);
    
    /// Destructor:
    virtual ~SiDetectorElement();
    
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Identification
    /// Methods to identify the element and identifier manipulation.
    // 
    ///////////////////////////////////////////////////////////////////
    
    //@{
    
    /// identifier of this detector element (inline)
    Identifier identify() const;
    
    /// identifier hash (inline)
    IdentifierHash identifyHash() const;
    
    /// Returns the id helper (inline)
    const AtlasDetectorID* getIdHelper() const;
    
    bool isPixel() const;
    bool isSCT() const;
    bool isBarrel() const;
    bool isEndcap() const; // inline
    bool isBlayer() const;
    bool isInnermostPixelLayer() const;
    bool isNextToInnermostPixelLayer() const;
    bool isDBM() const;

    // Identifier <-> SiCellId (ie strip number or pixel eta_index,phi_index)
    
    /// Identifier from SiCellId (ie strip number or pixel eta_index,phi_index)
    Identifier identifierFromCellId(const SiCellId &cellId) const;
    
    /// SiCellId from Identifier
    SiCellId cellIdFromIdentifier(const Identifier& identifier) const;
      
    //@}
    
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Navigation
    /// Methods to access neighbours. 
    //
    ///////////////////////////////////////////////////////////////////
    
    //@{
    const SiDetectorElement* nextInEta() const; // inline
    const SiDetectorElement* prevInEta() const; // inline
    const SiDetectorElement* nextInPhi() const; // inline
    const SiDetectorElement* prevInPhi() const; // inline
    const SiDetectorElement* otherSide() const; // Useful for SCT only, inline
    //@}
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Transformation/Orientation
    //
    ///////////////////////////////////////////////////////////////////
    
    //@{
    // Position 
    /// Local (simulation/hit frame) to global transform
    virtual const GeoTrf::Transform3D& transformHit() const;
    /// Local (reconstruction frame) to global transform
    const Amg::Transform3D& transform() const;
    /// Default Local (reconstruction frame) to global transform
    /// ie with no misalignment. 
    const HepGeom::Transform3D defTransformCLHEP() const;
    const Amg::Transform3D defTransform() const;
    /// Center in global coordinates
    const Amg::Vector3D& center() const;
    
    const HepGeom::Transform3D& transformCLHEP() const;
    
    /// Simulation/Hit local frame to reconstruction local frame. 2D.
    //  TODO: Will change order of parameters at some point.
    Amg::Vector2D hitLocalToLocal(double xEta, double xPhi) const;
    /// Same as previuos method but 3D.
    HepGeom::Point3D<double> hitLocalToLocal3D(const HepGeom::Point3D<double>& hitPosition) const;
    
    /// Transform to go from local reconstruction frame to local hit frame.
    const HepGeom::Transform3D recoToHitTransform() const;
    
    /// Directions of hit depth,phi,eta axes relative to reconstruction local position
    /// axes (LocalPosition). Returns +/-1. inline
    double hitDepthDirection() const;
    /// See previous method. inline
    double hitPhiDirection() const;
    /// See previous method. inline
    double hitEtaDirection() const;
    
    // To determine if readout direction between online and offline needs swapping, see methods
    // swapPhiReadoutDirection() and swapEtaReadoutDirection() below in "Readout Cell id" section
    
    
    // Orientation. 
    // Directions.
    //  phiAxis - in same direction as increasing phi and identifier phi_index/strip. 
    //            NB. This requires some flipping of axes with repsect to the hits.  
    //  etaAxis - in direction of increasing z in the barrel and increasing r in the endcap. 
    //  normal  - choosen to give right-handed coordinate frame (x=normal,y=phiAxis,z=etaAxis)
    //            NB. This requires some flipping of axes with repsect to the hits.  
      
    /// Get reconstruction local phi axes in global frame. 
    /// In same direction as increasing phi and identifier phi_index/strip. 
    const Amg::Vector3D& phiAxis() const;
    const HepGeom::Vector3D<double>& phiAxisCLHEP() const;
    /// Get reconstruction local eta axes in global frame. 
    /// In direction of increasing z in the barrel and increasing r in the endcap. 
    const Amg::Vector3D& etaAxis() const;
    const HepGeom::Vector3D<double>& etaAxisCLHEP() const;
    /// Get reconstruction local normal axes in global frame. Choosen to give right-handed coordinate frame.
    const Amg::Vector3D& normal() const;
     
    /// transform a hit local position into a global position (inline):
    HepGeom::Point3D<double> globalPositionHit(const HepGeom::Point3D<double>& simulationLocalPos) const;
    Amg::Vector3D globalPositionHit(const Amg::Vector3D& simulationLocalPos) const;
      
    /// transform a reconstruction local position into a global position (inline):
    HepGeom::Point3D<double> globalPosition(const HepGeom::Point3D<double>& localPos) const;
    Amg::Vector3D globalPosition(const Amg::Vector3D& localPos) const;

    /// as in previous method but for 2D local position (inline)
    HepGeom::Point3D<double> globalPositionCLHEP(const Amg::Vector2D& localPos) const;
    Amg::Vector3D globalPosition(const Amg::Vector2D& localPos) const;
    
    /// transform a global position into a 2D local position (reconstruction frame) (inline)
    Amg::Vector2D localPosition(const HepGeom::Point3D<double>& globalPosition) const;
    Amg::Vector2D localPosition(const Amg::Vector3D& globalPosition) const;
    
    /// Compute sin(tilt angle) at a given position:
    /// at center
    double sinTilt() const; // At center
    /// at given local position
    double sinTilt(const Amg::Vector2D& localPos) const;
    /// at given global position
    double sinTilt(const HepGeom::Point3D<double>& globalPosition) const;
    
    /// Compute sin(stereo angle) at a given position: at center
    double sinStereo() const; // At center
    /// at given local position
    double sinStereo(const Amg::Vector2D& localPos) const;
    /// at given global position
    double sinStereo(const HepGeom::Point3D<double>& globalPosition) const;
    
    /// Check if it is the stereo side (useful for SCT) 
    bool isStereo() const;
    
    /// Angle of strip in local frame with respect to the etaAxis. 
    /// Zero for all elements except trapezoidal detectors (ie SCT forward modules).
    double sinStereoLocal(const Amg::Vector2D& localPos) const;
    /// See previous method
    double sinStereoLocal(const HepGeom::Point3D<double>& globalPos) const;
    
    /// Element Surface
    virtual Trk::Surface& surface();
    virtual const Trk::Surface& surface() const;
    
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
    const Amg::Transform3D& moduleTransform() const;
    
    /// Default module to global frame transform, ie with no misalignment. 
    /// The module frame is defined to be the
    /// local reconstruction frame of the axial layer in the SCT. For
    /// the pixel it is the same as the element.
    Amg::Transform3D defModuleTransform() const;
      
    
    /// Take a transform of the local element frame and return its equivalent in the module frame.
    //HepGeom::Transform3D localToModuleFrame(const HepGeom::Transform3D & localTransform) const;
    Amg::Transform3D localToModuleFrame(const Amg::Transform3D& localTransform) const;
    
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
    // Extent in r,z and phi (inline)
    double rMin() const;
    double rMax() const;
    double zMin() const;
    double zMax() const;
    double phiMin() const;
    double phiMax() const;
    
    /// Method for building up region of interest table.
    /// Get eta/phi extent for the element. Returns min/max eta and phi
    /// and r (for barrel) or z (for endcap) Takes as input the vertex
    /// spread in z (-deltaZ to +deltaZ)
    void getEtaPhiRegion(double deltaZ, 
                         double& etaMin, double& etaMax,
                         double& phiMin, double& phiMax,
                         double& rz) const;
    //@}
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Design methods
    //
    ///////////////////////////////////////////////////////////////////
    //@{
    
    /// access to the local description (inline):
    const SiDetectorDesign& design() const;
    
    // Methods from design (inline)
    double width() const; // Width in phi direction. For the SCT endcap it returns the average width. 
    double minWidth() const; // Min, max width. Needed for the SCT endcap. 
    double maxWidth() const;
    double length() const; // Length in eta direction (z - barrel, r - endcap)
    double thickness() const;
    InDetDD::CarrierType carrierType() const; // carrier type for readout. ie holes for SCT 
    // and electrons for pixels.

    virtual const Trk::SurfaceBounds& bounds() const;
    
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
    double etaPitch() const; // inline
    double phiPitch() const; // inline
    double phiPitch(const Amg::Vector2D& localPosition) const; // Useful for SCT Forward. inline
    //@}
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Intersection Tests
    //
    ///////////////////////////////////////////////////////////////////
    
    //@{
    // Test if near bond gap within tolerances
    bool nearBondGap(const Amg::Vector2D& localPosition, double etaTol) const;
    bool nearBondGap(const HepGeom::Point3D<double>& globalPosition, double etaTol) const;
    
    // Test that it is in the active region
    // Intersect has 3 states
    // bool SiIntersect::in() const // definitely in
    // bool SiIntersect::out() const // definitely out
    // bool SiIntersect::nearBoundary() const // near a boundary within the tolerances 
    // bool SiIntersect::mayIntersect() const // in() OR nearBoundary()
    SiIntersect inDetector(const Amg::Vector2D& localPosition, double phiTol, double etaTol) const;
    SiIntersect inDetector(const HepGeom::Point3D<double>& globalPosition, double phiTol, double etaTol) const;
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
    /// For strip/phi_index (inline)
    bool swapPhiReadoutDirection() const;
    /// For eta_index (only relevant for pixel) (inline)
    bool swapEtaReadoutDirection() const;
    
    /// Full identifier of the cell for a given position:
    /// assumes a raw local position (no Lorentz shift)
    Identifier identifierOfPosition(const Amg::Vector2D& localPos) const;
    /// As in previous method but returns SiCellId 
    SiCellId cellIdOfPosition(const Amg::Vector2D& localPos) const;
    
    /// Returns position (center) of cell. These are the raw positions *NOT* corrected for the Lorentz shift
    Amg::Vector2D rawLocalPositionOfCell(const SiCellId & cellId) const;
    /// As above
    Amg::Vector2D rawLocalPositionOfCell(const Identifier& id) const;
    
    /// Test if readout cell has more than one diode associated with it.
    /// Number of cells sharing the same readout as this cell.
    /// ie generally 1 except for ganged pixels which will be 2.
    int numberOfConnectedCells(const SiCellId cellId) const;
    /// Get the cell ids sharing the readout for this cell.
    /// number = 0 will return the primary readout cell id.
    SiCellId connectedCell(const SiCellId cellId, int number) const;
    
    /// If cell is ganged return the id of the other cell which shares the readout
    /// for this cell, otherwise return an invalid id.
    /// This is a more convenient (and slightly faster) alternative than 
    /// using the above two methods.
    /// Only relevant for pixel. For SCT always returns an invalid ID. (inline)
    SiCellId gangedCell(const SiCellId& cellId) const;
    
    //@}
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Miscellaneous
    //
    ///////////////////////////////////////////////////////////////////
    //@{
    /// Special method for SCT to retrieve the two ends of a "strip"
    /// Returned coordinates are in global frame.
    std::pair<Amg::Vector3D,Amg::Vector3D> endsOfStrip(const Amg::Vector2D& position) const;
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
     
    /// Signal that cached values are no longer valid.
    /// Invalidate general cache (inline)
    void invalidate();
   
    ///Set/calculate cache values (inline)
    void setCache();

    ///Set/calculate all cache values including  surfaces. (inline)
    void setAllCaches();
   
    //@}
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Methods to satisfy TrkDetElementBase interface (inline)
    //
    ///////////////////////////////////////////////////////////////////
    //{@
    virtual const Amg::Transform3D& transform(const Identifier&) const;
    virtual const Trk::Surface& surface(const Identifier&) const;
    virtual const Amg::Vector3D& center(const Identifier&) const;
    virtual const Amg::Vector3D& normal(const Identifier&) const;
    virtual const Trk::SurfaceBounds& bounds(const Identifier&) const;
    //@}
    
    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    /// @name Non-const methods:
    /// These are not needed by most clients.
    ///////////////////////////////////////////////////////////////////
    //@{
    // Set neighbours. (inline)
    
    void setNextInEta(const SiDetectorElement* element);
    void setPrevInEta(const SiDetectorElement* element);
    void setNextInPhi(const SiDetectorElement* element);
    void setPrevInPhi(const SiDetectorElement* element);
    void setOtherSide(const SiDetectorElement* element); // For SCT only
    
    //@}

    const SiCommonItems* getCommonItems() const;
    
    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
    
  private:
    /*
     * Private Methods do not use locks
     * the methods calling them especially if const 
     * do
     */

    /// Recalculate  cached values. 
    void updateCache() const;
   
    /// Update all caches including surfaces.
    void updateAllCaches() const;

    /// Determine m_isStereo variable and m_stereoCacheValid variable.
    void determineStereo() const;
    
    // Common code for constructors.
    void commonConstructor();
    
    // Calculate extent in r,z and phi. The values are cached and there
    // are rMin(), rMax etc methods.
    // It is only used from updateCache
    void getExtent(double& rMin, double& rMax,
                   double& zMin, double& zMax,
                   double& phiMin, double& phiMax) const;
    
    // Return the four corners of an element in local coordinates.
    // Pass it an array of length 4.
    // This function is used by getEtaPhiRegion()
    void getCorners(HepGeom::Point3D<double>* corners) const;
    
    // Get eta and phi coresponding to a point in local coordinates. 
    // Requires as input the vertex spread. Returns etaMin, etaMax, and phi.
    // This function is used by getEtaPhiRegion()
    void getEtaPhiPoint(const HepGeom::Point3D<double>& point, double deltaZ,
                        double& etaMin, double& etaMax, double& phi) const;

    // Private recoToHitTransform Implementation method with no lock
    const HepGeom::Transform3D recoToHitTransformImpl() const;
 
    // Private Implementation  methods with no lock
    double sinStereoImpl() const; // At center
    // at given global position
    double sinStereoImpl(const HepGeom::Point3D<double>& globalPosition) const;
 
    // Declaring the Message method for further use (inline)
    MsgStream& msg(MSG::Level lvl) const;
    
    // Declaring the Method providing Verbosity Level (inline)
    bool msgLvl(MSG::Level lvl) const;
    
    // Don't allow copying and moving.
    SiDetectorElement() = delete;
    SiDetectorElement(const SiDetectorElement&) = delete;
    SiDetectorElement& operator=(const SiDetectorElement&) = delete;
    SiDetectorElement(SiDetectorElement&&) = delete;
    SiDetectorElement& operator=(SiDetectorElement&&) = delete;

    ///////////////////////////////////////////////////////////////////
    // Protected data:
    ///////////////////////////////////////////////////////////////////
  protected:
    Identifier m_id; // identifier of this detector element
    IdentifierHash m_idHash; // hash id
    const SiDetectorDesign* m_design; // local description of this detector element
    //const AtlasDetectorID* m_idHelper; // id helper
    const SiCommonItems* m_commonItems;
    
    const SiDetectorElement* m_nextInEta;
    const SiDetectorElement* m_prevInEta;
    const SiDetectorElement* m_nextInPhi;
    const SiDetectorElement* m_prevInPhi;
    const SiDetectorElement* m_otherSide;
    
    bool m_isPixel;
    bool m_isBarrel;
    bool m_isDBM;
      
    //
    // Cached values.
    //
    // Axes
    SiDetectorDesign::Axis m_hitEta;
    SiDetectorDesign::Axis m_hitPhi;
    SiDetectorDesign::Axis m_hitDepth;

    // Directions of axes. These are true if the hit/simulation and reconstruction local frames are
    // in the same direction and false if they are opposite.
    mutable bool m_depthDirection ATLAS_THREAD_SAFE; // Guarded by m_mutex // Direction of depth axis. 
    // Also direction of readout implant (n+ for pixel, p+ for SCT).
    mutable bool m_phiDirection ATLAS_THREAD_SAFE;
    mutable bool m_etaDirection ATLAS_THREAD_SAFE;

    mutable std::atomic_bool m_cacheValid; // Alignment associated quatities.
    mutable std::atomic_bool m_firstTime;
    // Since m_isStereo depends on m_otherSide->sinStereo(), a dedicated validity variable is needed.
    mutable std::atomic_bool m_stereoCacheValid;
    mutable bool m_isStereo ATLAS_THREAD_SAFE;

    mutable std::mutex m_mutex;

    mutable Amg::Transform3D m_transform ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable HepGeom::Transform3D m_transformCLHEP ATLAS_THREAD_SAFE; // Guarded by m_mutex

    mutable Amg::Vector3D m_normal ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable Amg::Vector3D m_etaAxis ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable HepGeom::Vector3D<double> m_etaAxisCLHEP ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable Amg::Vector3D m_phiAxis ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable HepGeom::Vector3D<double> m_phiAxisCLHEP ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable Amg::Vector3D m_center ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable HepGeom::Vector3D<double> m_centerCLHEP ATLAS_THREAD_SAFE; // Guarded by m_mutex

    mutable double m_minZ   ATLAS_THREAD_SAFE;// Guarded by m_mutex
    mutable double m_maxZ   ATLAS_THREAD_SAFE;// Guarded by m_mutex
    mutable double m_minR   ATLAS_THREAD_SAFE;// Guarded by m_mutex
    mutable double m_maxR   ATLAS_THREAD_SAFE;// Guarded by m_mutex
    mutable double m_minPhi ATLAS_THREAD_SAFE;// Guarded by m_mutex
    mutable double m_maxPhi ATLAS_THREAD_SAFE;// Guarded by m_mutex

    CxxUtils::CachedUniquePtrT<Trk::Surface> m_surface;
    mutable std::vector<const Trk::Surface*> m_surfaces ATLAS_THREAD_SAFE; // Guarded by m_mutex

    const GeoAlignmentStore* m_geoAlignStore{};
  };
    
} // namespace InDetDD

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
#include "InDetReadoutGeometry/SiDetectorElement.icc"

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
