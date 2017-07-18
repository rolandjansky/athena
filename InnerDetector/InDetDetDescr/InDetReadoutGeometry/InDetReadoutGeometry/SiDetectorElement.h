/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiDetectorElement.h
 **/

#ifndef INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
#define INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H

// Base class.
#include "TrkDetElementBase/TrkDetElementBase.h"

// Data member classes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "InDetReadoutGeometry/SiIntersect.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

class AtlasDetectorID;
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

   The following coordinate frames are used in these elements. (Modernised for upgrade by N. Hessey, May 2016)

   - Global frame:\n
       Currently global frame in G4/GeoModel/Atlas. 
          - x points towards centre of LHC
          - y points up
          - z points along the beam, giving a RH system
          - phi from RH grip rule along z axis (x to y, zero degrees along x axis)


   - GeoModel frame:\n
       Knows only (x, y, z). But different sensor shapes have different orientations:

       SCT sensors
          - Depth = GeoModel x
          - Phi   = GeoModel y
          - Eta   = GeoModel z

       GeoModelXml sensors (StripBoxDesign and StripStereoAnnulus) are oriented:
          - Eta   = GeoModel x
          - Phi   = GeoModel y
          - Depth = GeoModel z

       This orientation is stored in m_eta/phi/depthAxis.

       The readout side is determined from design()->readoutSide().

       GeoModel-transforms transform from GeoModel to Global frames.


   - SiLocalPosition frame:\n      
          - Phi   = x
          - Eta   = y
          - Depth = z
          (This choice is implicit in the phi() and r() methods in SiLocalPosition)
          Introduced by David Calvet ca. 2001


   - Hit frame:\n
          - Eta   = x
          - Phi   = y
          - Depth = z
         Introduced by Grant Gorfine, ca. 2003. From ChangeLog:
         2003-08-13  Grant Gorfine  <Grant.Gorfine@cern.ch>
         * Introduce support for a local hit (simulation) frame which is
           intended to be the local frame in GeoModel/G4 and the hits.
           This frame has depth,phi,eta defined to always be along
           x,y,z. Included method (hitLocalToLocal) to convert between this
           and SiLocalPosition (they differ by some minus signs in the
           axes).  Implemented unit vector methods and a few others.

         I think the hits root file stores hits in this frame.

         Nigel: I think this is what I call the GeoModel frame, but with some mapping of (x, y, z) to (eta, phi, depth). 
         I don't see how this gives the cross product correctly: clearly in GeoModel frame, i cross j = k. In Hit frame, 
         the intention is that eta cross phi = depth. 
         A lot of code refers to the Hit frame, but it is not explicitly used? 
         The statement "depth,phi,eta is always x,y,z" is not true (any more):
             GeoModel does not know about eta, phi, depth and has
             restrictions on axes directions for some shapes.
         The statement "SiLocalPosition differs from Hit by only some sign changes" is not true: they swap eta and phi
         (which also swaps the sense of z). 
         Furthermore, hitLocalToLocal no longer takes Hit to SiLocalposition. It takes GeoModel to Reco. Don't know when this 
         changed.

   - Reconstruction (Reco) frame:\n
          - distPhi   = Reco x
          - distEta   = Reco y
          - distDepth = Reco z

          The sense of these axes are chosen to be:
          - distPhi   in direction of increasing global-phi
          - distEta   in direction of increasing global-z in barrel and 
                      increasing r in both endcaps. This means in the endcap at +ve z, the Reco eta axis points in the direction 
                      of decreasing eta. The advantage of this convention is that nextInEta() is one ring out, for both endcaps.
          - distDepth Choosen to give right-handed coordinate system: 
                      turns out to be always away from intersection point for barrel, 
                      always opposite to Global-z for endcap, independent of which side of a module/petal the sensor is

          Which makes them either parallel or anti-parallel to the corresponding axis in other frames.

    - Module frame:\n
         For Pixels, same as Reco frame. 
         For SCT, it is the Reco frame of the axial or radial sensor, as opposed to the stereo sensor. 
         Not well dfined for StripStereoAnnulus, where neither sensor is radial.

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
   - modified Nigel Hessey: rationalise some reference frame names, add sensroCenter() method, clean some code May 2016
*/

class SiDetectorElement: public Trk::TrkDetElementBase {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:
    /// Constructor:
    SiDetectorElement(const Identifier &id,
                      const SiDetectorDesign *design,
                      const GeoVFullPhysVol *geophysvol,
                      SiCommonItems *commonItems);

    /// Destructor:
    virtual ~SiDetectorElement();


    ///////////////////////////////////////////////////////////////////
    //
    /// @name Identification
    /// Methods to identify the element and identifier manipulation.
    //
    ///////////////////////////////////////////////////////////////////

    // @{

    /// identifier of this detector element:
    virtual Identifier identify() const;

    /// identifier hash
    virtual IdentifierHash identifyHash() const;

    /// Returns the id helper
    const AtlasDetectorID *getIdHelper() const;

    bool isPixel() const;
    bool isSCT() const;
    bool isBarrel() const;
    bool isEndcap() const;
    bool isBlayer() const;
    bool isInnermostPixelLayer() const;
    bool isNextToInnermostPixelLayer() const;
    bool isDBM() const;

    // Identifier <-> SiCellId (ie strip number or pixel eta_index,phi_index)

    /// Identifier from SiCellId (ie strip number or pixel eta_index,phi_index)
    Identifier identifierFromCellId(const SiCellId &cellId) const;

    /// SiCellId from Identifier
    SiCellId cellIdFromIdentifier(const Identifier &identifier) const;

    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Navigation
    /// Methods to access neighbours.
    //
    ///////////////////////////////////////////////////////////////////

    // @{
    const SiDetectorElement *nextInEta() const;
    const SiDetectorElement *prevInEta() const;
    const SiDetectorElement *nextInPhi() const;
    const SiDetectorElement *prevInPhi() const;
    const SiDetectorElement *otherSide() const; // Useful for SCT only
    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Transformation/Orientation
    //
    ///////////////////////////////////////////////////////////////////

    // @{

    /// Center in global coordinates
    /// This is now defined to be somewhere near the centre-of-gravity of the sensor in the global frame
    /// - not the position of the GeoModel frame in the global frame; these differ a lot for StripStereoAnnulusDesign
    const Amg::Vector3D &center() const;

    /// Check sensor orientation
    void checkOrientation(const HepGeom::Vector3D<double> &unitR, 
                          const HepGeom::Vector3D<double> &globalEtaAxis, 
                          const HepGeom::Vector3D<double> &globalDepthAxis) const;

    /// Swaps eta and phi, taking into account the axes sense between Reco and other frames. Only 2D. 
    /// This takes Reco to SiLocalPosition, and vice versa (SiLocalPosition to Reco), i.e it is self-inverse.
    /// Somehow automatic casting occurs from Amg::Vector2D to SiLocalPosition. 
    Amg::Vector2D hitLocalToLocal(double xEta, double xPhi) const;

    /// GeoModel to Reco. Nothing to do with the 2D version.
    HepGeom::Point3D<double> hitLocalToLocal3D(const HepGeom::Point3D<double> &hitPosition) const;

    /// Reco to GeoModel (again, ignore the name!)
    const HepGeom::Transform3D recoToHitTransform() const;

    /// GeoModel to Global (mis-named). It is the GeoModel aligned transform.
    virtual const HepGeom::Transform3D &transformHit() const;

    /// Reco to Global (calculated as GeoModelToGlobal * RecoToGeoModel 
    const HepGeom::Transform3D &transformCLHEP() const;
    const Amg::Transform3D &transform() const;

    /// Default-Reco to Global, i.e. with no misalignment.
    const HepGeom::Transform3D defTransformCLHEP() const;
    const Amg::Transform3D defTransform() const;

    /// Hit to Global. Deprecated. Just use transformHit * point. Only used in SCT RTT, two lines.
    HepGeom::Point3D<double> globalPositionHit(const HepGeom::Point3D<double> &simulationLocalPos) const;
    Amg::Vector3D globalPositionHit(const Amg::Vector3D &simulationLocalPos) const;

    /// Reco to Global, (2D and 3D) to 3D, CLHEP and Amg style.
    HepGeom::Point3D<double> globalPositionCLHEP(const Amg::Vector2D &localPos) const;
    Amg::Vector3D globalPosition(const Amg::Vector2D &localPos) const;
    HepGeom::Point3D<double> globalPosition(const HepGeom::Point3D<double> &localPos) const;
    Amg::Vector3D globalPosition(const Amg::Vector3D &localPos) const;

    /// Global to Reco, 2D
    Amg::Vector2D localPosition(const HepGeom::Point3D<double> &globalPosition) const;
    Amg::Vector2D localPosition(const Amg::Vector3D &globalPosition) const;

    /// Sense of GeoModel axes relative to Reco axes (i.e. whether parallel or anti-parallel)
    /// axes (LocalPosition). Returns +/-1 for parallel/anti-parallel.
    double hitDepthDirection() const;
    double hitPhiDirection() const;
    double hitEtaDirection() const;

    // To determine if readout direction between online and offline needs swapping, see methods
    // swapPhiReadoutDirection() and swapEtaReadoutDirection() below in "Readout Cell id" section

    // Orientation.
    // Reco directions:
    //  phiAxis - in direction of increasing Global-phi
    //  etaAxis - in direction of increasing z in the barrel and increasing r in the endcap
    //  depth   - choosen to give right-handed coordinate frame (depth cross phiAxis = etaAxis)

    /// Get Reco phi axis in Global frame.
    const HepGeom::Vector3D<double> &phiAxisCLHEP() const;
    const Amg::Vector3D &phiAxis() const;

    /// Get Reco eta axis in Global frame.
    /// In direction of increasing z in the barrel and increasing r in both endcaps.
    const Amg::Vector3D &etaAxis() const;
    const HepGeom::Vector3D<double> &etaAxisCLHEP() const;

    /// Get Reco depth axis in Global frame. Chosen to give right-handed coordinate frame.
    const Amg::Vector3D &normal() const;

    /// Compute sin(tilt angle) at a given position:
    /// at center
    double sinTilt() const; // At center
    /// at given local position
    double sinTilt(const Amg::Vector2D &localPos) const;
    /// at given global position
    double sinTilt(const HepGeom::Point3D<double> &globalPosition) const;

    /// Compute sin/cos(stereo angle) at a given position:
    /// at center
    double cosStereo() const;
    double sinStereo() const;
    /// at given local position
    double sinStereo(const Amg::Vector2D &localPos) const;
    /// at given global position
    double sinStereo(const HepGeom::Point3D<double> &globalPosition) const;

    /// Check if it is the stereo side (useful for SCT)
    bool isStereo() const;

    /// Angle of strip in local frame with respect to the etaAxis.
    /// Zero for all elements except trapezoidal and Strip(Stereo)Annulus detectors (ie SCT forward modules).
    /// at given local position
    double sinStereoLocal(const Amg::Vector2D &localPos) const;
    /// at given global position
    double sinStereoLocal(const HepGeom::Point3D<double> &globalPos) const;

    /// Element Surface
    virtual const Trk::Surface &surface() const;

    // @}

    /** Returns the full list of surfaces associated to the module this element is in: 2 surfaces for strip modules */
    virtual const std::vector<const Trk::Surface *> &surfaces() const;

    /**
       @name Module Frame
       Methods to help work with the module frame.
       This is mainly of use in the SCT where the module frame can in general be different from the element frame. 
       It is actully defined as the frame of one of the sides (currently the axial side in the barrel, radial side in the SCT),
       but using these methods one does not need to make any assumptions about what the actual frame is.  In the following
       methods the local element frame is the Reco frame of this element.
     */

    // @{

    /// Module to global frame transform. Includes misalignment. 
    /// The module frame is defined to be the Reco frame of the axial/radial side of an SCT module.
    /// In the pixel it is the same as the element.
    const Amg::Transform3D &moduleTransform() const;

    /// Default module to global frame transform, ie with no misalignment.
    Amg::Transform3D defModuleTransform() const;

    /// Take a transform acting in the Reco frame of a detector-element, and return its equivalent in the module frame.
    Amg::Transform3D localToModuleFrame(const Amg::Transform3D &localTransform) const;

    /// Transformation from Reco frame of the element to Reco frame of the module. 
    /// If one is already in the module frame it will return the Identity transform.
    Amg::Transform3D localToModuleTransform() const;

    /// Check if the element and module frame are the same.
    bool isModuleFrame() const;

    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Element Extent
    /// Methods to get extent of sensor in r, phi and z.
    ///////////////////////////////////////////////////////////////////

    // @{
    // Extent in r,z and phi
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
                         double &etaMin, double &etaMax,
                         double &phiMin, double &phiMax,
                         double &rz) const;
    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Design methods
    //
    ///////////////////////////////////////////////////////////////////
    // @{

    /// access to the local description:
    const SiDetectorDesign &design() const;

    // Methods return values from design (presumably saves CPU compared to design()->width. etc?)
    double width() const;    // Width in phi direction. For the SCT endcap it returns the average width.
    double minWidth() const; // Min, max width. Needed for the SCT endcap.
    double maxWidth() const;
    double length() const;   // Length in eta direction (z - barrel, r - endcap)
    double thickness() const;
    InDetDD::CarrierType carrierType() const; // carrier type for readout. ie holes for SCT
                                              // and electrons for pixels.

    virtual const Trk::SurfaceBounds &bounds() const;

    // Pitch
    //
    // NOTE: phiPitch is ambiguous for the Forward SCT where it varies along the strip.
    //       etaPitch is ambiguous for the pixel which has long pixels between FE chips.
    //
    // For these cases:
    //
    // phiPitch():  For SCT Forward returns pitch at center.
    // etaPitch():  For pixel returns average pitch. (Active_length/number_of_cells)
    //
    // All return pitch in length-units, never angles.
    //
    double etaPitch() const;
    double phiPitch() const;
    double phiPitch(const Amg::Vector2D &) const; // Useful for SCT Forward.
    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Intersection Tests
    //
    ///////////////////////////////////////////////////////////////////

    // @{
    // Test if near bond gap within tolerances
    bool nearBondGap(Amg::Vector2D localPosition, double etaTol) const;
    bool nearBondGap(HepGeom::Point3D<double> globalPosition, double etaTol) const;

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Lorentz Correction
    //
    ///////////////////////////////////////////////////////////////////
    // @{

    /// tan(Lorentz angle). Component in phi direction (Hit frame)
    double getTanLorentzAnglePhi() const;

    /// Same as getTanLorentzAnglePhi()
    double getTanLorentzAngle() const;

    /// tan(Lorentz angle). Component in eta direction (Hit frame)
    double getTanLorentzAngleEta() const;

    /// Lorentz shift in distPhi (Reco frame)
    double getLorentzCorrection() const;

    /// Correct a local position (reconstruction frame) for the Lorentz angle:
    /// Users generally shouldn't need to call this as localPositionOfCell() returns the Lorentz corrected position.
    Amg::Vector2D correctLocalPosition(const Amg::Vector2D &position) const;
    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Readout cell id
    /// Cell id's are the strip number in SCT and phi_index, eta_index in the pixel
    /// as defined in the offline identifier. Their direction runs in the distPhi, distEta
    /// direction in the Reco frame.
    ///
    /// For methods taking a SiCellId (basically (phi, eta) index for pixel, or strip for SCT) you
    /// can do the following for example:
    ///   - For pixel\n
    ///      localPositionOfCell(SiCellId(phi_index,eta_index));\n
    ///   - For SCT\n
    ///      localPositionOfCell(SiCellId(strip));\n
    //
    ///////////////////////////////////////////////////////////////////
    // @{

    /// Determine if readout direction between online and offline needs swapping.
    /// Returns true if online and offline numbers run in opposite directions.
    /// For strip/phi_index
    bool swapPhiReadoutDirection() const;
    /// For eta_index (only relevant for pixel)
    bool swapEtaReadoutDirection() const;

    /// Full identifier of the cell for a given position:
    /// assumes a raw local position (no Lorentz shift)
    Identifier identifierOfPosition(const Amg::Vector2D &localPos) const;
    /// As in previous method but returns SiCellId
    SiCellId cellIdOfPosition(const Amg::Vector2D &localPos) const;

    /// Returns position (center) of cell. These are corrected for the Lorentz shift
    Amg::Vector2D localPositionOfCell(const SiCellId &cellId) const;
    /// As above
    Amg::Vector2D localPositionOfCell(const Identifier &id) const;

    /// Returns position (center) of cell. These are the raw positions *NOT* corrected for the Lorentz shift
    Amg::Vector2D rawLocalPositionOfCell(const SiCellId &cellId) const;
    /// As above
    Amg::Vector2D rawLocalPositionOfCell(const Identifier &id) const;

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
    /// Only relevant for pixel. For SCT always returns an invalid ID.
    SiCellId gangedCell(const SiCellId &cellId) const;

    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Miscellaneous
    //
    ///////////////////////////////////////////////////////////////////
    // @{
    /// Special method for SCT to retrieve the two ends of the strip containing a point given in the reco frame.
    /// Returned coordinates are in the global frame.
    std::pair<Amg::Vector3D, Amg::Vector3D > endsOfStrip(const Amg::Vector2D &position) const;
    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Cache handling.
    //
    ///////////////////////////////////////////////////////////////////
    // @{.
    //   - Methods to handle invalidating and updating caches. The cached values include values that are affected by
    // alignment
    //     Surface are only created on demand.  The method updateAllCaches also creates the surfaces as well as calling
    // updateCache.
    //     Conditions cache contains Lorentz angle related quantities.

    /// Signal that cached values are no longer valid.
    /// Invalidate general cache
    void invalidate() const;

    /// invalidate conditions cache
    void invalidateConditions() const;

    /// Recalculate all cached values.
    void updateCache() const;

    /// Recalculate subset of cached values.
    void updateConditionsCache() const;

    /// Update all caches including surfaces.
    void updateAllCaches() const;

    // @}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Methods to satisfy TrkDetElementBase interface
    //
    ///////////////////////////////////////////////////////////////////
    // {@
    virtual const Amg::Transform3D &transform(const Identifier &) const {
        return transform();
    }

    virtual const Trk::Surface &surface(const Identifier &) const {
        return surface();
    }

    virtual const Amg::Vector3D &center(const Identifier &) const {
        return center();
    }

    virtual const Amg::Vector3D &normal(const Identifier &) const {
        return normal();
    }

    virtual const Trk::SurfaceBounds &bounds(const Identifier &) const {
        return bounds();
    }

    // @}

public:
    ///////////////////////////////////////////////////////////////////
    /// @name Non-const methods:
    /// These are not needed by most clients.
    ///////////////////////////////////////////////////////////////////
    // @{
    // Set neighbours.

    void setNextInEta(const SiDetectorElement *);
    void setPrevInEta(const SiDetectorElement *);
    void setNextInPhi(const SiDetectorElement *);
    void setPrevInPhi(const SiDetectorElement *);
    void setOtherSide(const SiDetectorElement *); // For SCT only

    // @}

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    // Common code for constructors.
    void commonConstructor();

    // Calculate extent in r, z and phi. The values are cached and there are 
    // rMin(), rMax() etc methods.
    void getExtent(double &rMin, double &rMax,
                   double &zMin, double &zMax,
                   double &phiMin, double &phiMax) const;

    // Return the four corners of an element in local???? coordinates.
    // Pass it a vector of length 4 which will be filled with the four corners.
    // This function is used by getEtaPhiRegion()
    void getCorners(HepGeom::Point3D<double> *corners) const;

    // Get eta and phi coresponding to a point in local???? coordinates.
    // Requires as input the vertex spread. Returns etaMin, etaMax, and phi.
    // This function is used by getEtaPhiRegion()
    void getEtaPhiPoint(const HepGeom::Point3D<double> &point, double deltaZ,
                        double &etaMin, double &etaMax, double &phi) const;


    // Declaring the Message method for further use
    MsgStream &msg(MSG::Level lvl) const {
        return m_commonItems->msg(lvl);
    }

    // Declaring the Method providing Verbosity Level
    bool msgLvl(MSG::Level lvl) const {
        return m_commonItems->msgLvl(lvl);
    }

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    // Don't allow copying.
    SiDetectorElement();
    SiDetectorElement(const SiDetectorElement &);
    SiDetectorElement &operator = (const SiDetectorElement &);

    ///////////////////////////////////////////////////////////////////
    // Protected data:
    ///////////////////////////////////////////////////////////////////
protected:
    Identifier m_id; // wafer-identifier of this detector element
    IdentifierHash m_idHash; // hash id
    const SiDetectorDesign *m_design; // local description of this detector element
    // const AtlasDetectorID* m_idHelper; // id helper
    SiCommonItems *m_commonItems;

    const SiDetectorElement *m_nextInEta;
    const SiDetectorElement *m_prevInEta;
    const SiDetectorElement *m_nextInPhi;
    const SiDetectorElement *m_prevInPhi;
    const SiDetectorElement *m_otherSide;

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
    mutable bool m_depthDirection;
    mutable bool m_phiDirection;
    mutable bool m_etaDirection;

    mutable bool m_cacheValid; // Alignment associated quatities.
    mutable bool m_conditionsCacheValid; // Lorentz angle related values.
    mutable bool m_firstTime;
    mutable bool m_isStereo;      

    // Reco to Global transformation
    mutable Amg::Transform3D     m_transform;
    mutable HepGeom::Transform3D m_transformCLHEP; 
    mutable HepGeom::Transform3D m_transformHit;

    mutable Amg::Vector3D m_normal;
    mutable HepGeom::Vector3D<double> m_normalCLHEP;
    mutable Amg::Vector3D m_etaAxis;
    mutable Amg::Vector3D m_phiAxis;
    mutable HepGeom::Vector3D<double> m_etaAxisCLHEP;
    mutable HepGeom::Vector3D<double> m_phiAxisCLHEP;
    mutable Amg::Vector3D m_center;
    mutable HepGeom::Vector3D<double> m_centerCLHEP;

    mutable double m_minZ;
    mutable double m_maxZ;
    mutable double m_minR;
    mutable double m_maxR;
    mutable double m_minPhi;
    mutable double m_maxPhi;

    mutable double m_tanLorentzAnglePhi;
    mutable double m_tanLorentzAngleEta;
    mutable double m_lorentzCorrection;

    mutable Trk::Surface *m_surface;
    mutable std::vector<const Trk::Surface *> m_surfaces;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline bool SiDetectorElement::isEndcap() const {
    return(!isBarrel() && !isDBM());
}

inline HepGeom::Point3D<double> SiDetectorElement::globalPositionHit(const HepGeom::Point3D<double> &localPos) const {
    // deprecated. Just use the following line directly: Only used in one place in Athena (SCT RTT).
    return transformHit() * localPos;
}

//inline Amg::Vector3D SiDetectorElement::globalPosition(const Amg::Vector2D &localPos) const {
//    if (!m_cacheValid) {
//        updateCache();
//    }
//    Amg::Vector3D lp(localPos[0], localPos[1], 0.);
//    return transform() * lp;
//}

inline Amg::Vector3D SiDetectorElement::globalPositionHit(const Amg::Vector3D &localPos) const {
    return Amg::CLHEPTransformToEigen(transformHit()) * localPos;
}

inline HepGeom::Point3D<double> SiDetectorElement::globalPositionCLHEP(const Amg::Vector2D &localPos) const {
    if (!m_cacheValid) {
        updateCache();
    }
    return m_centerCLHEP + localPos[Trk::distEta] * m_etaAxisCLHEP + localPos[Trk::distPhi] * m_phiAxisCLHEP;
}

inline Amg::Vector3D SiDetectorElement::globalPosition(const Amg::Vector3D &localPos) const {
    return transform() * localPos;
}

inline HepGeom::Point3D<double> SiDetectorElement::globalPosition(const HepGeom::Point3D<double> &localPos) const {
    return transformCLHEP() * localPos;
}

inline Amg::Vector2D SiDetectorElement::localPosition(const HepGeom::Point3D<double> &globalPosition) const {
    if (!m_cacheValid) {
        updateCache();
    }
    HepGeom::Vector3D<double> relativePos = globalPosition - m_centerCLHEP - design().sensorCenter();
    
    if (design().shape() == InDetDD::Annulus)
          relativePos = globalPosition;
    
    return Amg::Vector2D(relativePos.dot(m_phiAxisCLHEP), relativePos.dot(m_etaAxisCLHEP));
}

inline Amg::Vector2D SiDetectorElement::localPosition(const Amg::Vector3D &globalPosition) const {
    if (!m_cacheValid) {
        updateCache();
    }
    HepGeom::Vector3D<double> sensorCCLHEP = design().sensorCenter();
    Amg::Vector3D sensorC(sensorCCLHEP[0], sensorCCLHEP[1], sensorCCLHEP[2]);
    Amg::Vector3D relativePos = globalPosition - m_center - sensorC;
    if (design().shape() == InDetDD::Annulus)
          relativePos = globalPosition;
    return Amg::Vector2D(relativePos.dot(m_phiAxis), relativePos.dot(m_etaAxis));
}

inline const SiDetectorDesign &SiDetectorElement::design() const {
    return *m_design;
}

inline const AtlasDetectorID *SiDetectorElement::getIdHelper() const {
    return m_commonItems->getIdHelper();
}

inline Identifier SiDetectorElement::identify() const {
    return m_id;
}

inline IdentifierHash SiDetectorElement::identifyHash() const {
    return m_idHash;
}

inline double SiDetectorElement::hitDepthDirection() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return (m_depthDirection) ? 1. : -1.;
}

inline double SiDetectorElement::hitPhiDirection() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return (m_phiDirection) ? 1. : -1.;
}

inline double SiDetectorElement::hitEtaDirection() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return (m_etaDirection) ? 1. : -1.;
}

inline InDetDD::CarrierType SiDetectorElement::carrierType() const {
    return m_design->carrierType();
}

inline SiCellId SiDetectorElement::gangedCell(const SiCellId &cellId) const {
    return m_design->gangedCell(cellId);
}

inline void SiDetectorElement::invalidate() const {
    m_cacheValid = false;
    // Conditions cache invalidation is done by SiLorentzAngleSvc.
}

inline void SiDetectorElement::invalidateConditions() const {
    m_conditionsCacheValid = false;
}

inline void SiDetectorElement::updateAllCaches() const {
    if (!m_cacheValid) {
        updateCache();
    }
    if (!m_conditionsCacheValid) {
        updateConditionsCache();
    }
    if (!m_surface) {
        surface();
    }
}

inline double SiDetectorElement::rMin() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return m_minR;
}

inline double SiDetectorElement::rMax() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return m_maxR;
}

inline double SiDetectorElement::zMin() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return m_minZ;
}

inline double SiDetectorElement::zMax() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return m_maxZ;
}

inline double SiDetectorElement::phiMin() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return m_minPhi;
}

inline double SiDetectorElement::phiMax() const {
    if (!m_cacheValid) {
        updateCache();
    }
    return m_maxPhi;
}

inline double SiDetectorElement::width() const {
    return m_design->width();
}

inline double SiDetectorElement::minWidth() const {
    return m_design->minWidth();
}

inline double SiDetectorElement::maxWidth() const {
    return m_design->maxWidth();
}

inline double SiDetectorElement::length() const {
    return m_design->length();
}

inline double SiDetectorElement::thickness() const {
    return m_design->thickness();
}

inline double SiDetectorElement::etaPitch() const {
    return m_design->etaPitch();
}

inline double SiDetectorElement::phiPitch() const {
    return m_design->phiPitch();
}

inline double SiDetectorElement::phiPitch(const Amg::Vector2D &localPosition) const {
    return m_design->phiPitch(localPosition);
}

inline const SiDetectorElement *SiDetectorElement::nextInEta() const {
    return m_nextInEta;
}

inline const SiDetectorElement *SiDetectorElement::prevInEta() const {
    return m_prevInEta;
}

inline const SiDetectorElement *SiDetectorElement::nextInPhi() const {
    return m_nextInPhi;
}

inline const SiDetectorElement *SiDetectorElement::prevInPhi() const {
    return m_prevInPhi;
}

inline const SiDetectorElement *SiDetectorElement::otherSide() const {
    return m_otherSide;
}

inline void SiDetectorElement::setNextInEta(const SiDetectorElement *element) {
    m_nextInEta = element;
}

inline void SiDetectorElement::setPrevInEta(const SiDetectorElement *element) {
    m_prevInEta = element;
}

inline void SiDetectorElement::setNextInPhi(const SiDetectorElement *element) {
    m_nextInPhi = element;
}

inline void SiDetectorElement::setPrevInPhi(const SiDetectorElement *element) {
    m_prevInPhi = element;
}

inline void SiDetectorElement::setOtherSide(const SiDetectorElement *element) { // For SCT only
    m_otherSide = element;
}

inline bool SiDetectorElement::swapPhiReadoutDirection() const {
    // equivalent to (m_design->swapHitPhiReadoutDirection() xor !m_phiDirection)
    return((!m_design->swapHitPhiReadoutDirection() && !m_phiDirection)
           || (m_design->swapHitPhiReadoutDirection() && m_phiDirection));
}

inline bool SiDetectorElement::swapEtaReadoutDirection() const {
    // equivalent to (m_design->swapHitEtaReadoutDirection() xor !m_etaDirection)
    return((!m_design->swapHitEtaReadoutDirection() && !m_etaDirection)
           || (m_design->swapHitEtaReadoutDirection() && m_etaDirection));
}

inline double SiDetectorElement::getTanLorentzAnglePhi() const {
    if (!m_conditionsCacheValid) {
        updateConditionsCache();
    }
    return m_tanLorentzAnglePhi;
}

// Same as getTanLorentzAnglePhi()
inline double SiDetectorElement::getTanLorentzAngle() const {
    if (!m_conditionsCacheValid) {
        updateConditionsCache();
    }
    return m_tanLorentzAnglePhi;
}

inline double SiDetectorElement::getTanLorentzAngleEta() const {
    if (!m_conditionsCacheValid) {
        updateConditionsCache();
    }
    return m_tanLorentzAngleEta;
}

inline double SiDetectorElement::getLorentzCorrection() const {
    if (!m_conditionsCacheValid) {
        updateConditionsCache();
    }
    return m_lorentzCorrection;
}
} // namespace InDetDD
#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
