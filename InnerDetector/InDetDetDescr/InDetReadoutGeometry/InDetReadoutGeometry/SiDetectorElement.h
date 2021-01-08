/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiDetectorElement.h
 **/

#ifndef INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
#define INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H

/**
 * Base class.
 */
#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"

namespace InDetDD {

  /**
   *  
   * @class SiDetectorElement
   *
   * Class to hold geometrical description of a silicon detector element. 
   * A detector element is a module in the pixels and one side of a module in the SCT.
   * This class is shared between the Pixel and SCT detector since there is a lot of commonality.
   * It inherits frm ReadoutGeometryBase/SolidStateDetectorElement, which is also used for HGTD
   * 
   * @par Coordinate Frames.
   *
   * The following coordinate frames are used in these elements.
   *
   * - Global frame:\n
   * Currently global frame in G4/GeoModel. Probably eventually
   * will be global frame most suitable for reconstruction 
   * (eg solenoid axis).
   *
   * - Local hit frame:\n 
   * Local frame for hits. It is the same as local frame in G4 and GeoModel. 
   * I also refer to this as the local simulation frame. 
   * By convention elements are orientated such that:
   * - hitDepth = local x
   * - hitPhi   = local y
   * - hitEta   = local z
   * . 
   * Directions of these correspond to the physical wafer. Consequently hitDepth and hitPhi axes go in 
   * different directions depending on the orientation of the module.
   * The readout side is determined from design()->readoutSide(). 
   *
   * - Local reconstruction frame:\n 
   * - distPhi  = local x
   * - distEta  = local y  
   * - distDepth = local z
   * .
   * The directions of the axes are defined as
   * - distPhi in direction of increasing phi
   * - distEta in direction of increasing z in barrel and increasing r in endcap.
   * - distDepth (normal) choosen to give right-handed coordinate. 
   * =>  away from intersection point for barrel, decreasing z for endcap
   * 
   * @par Overview of Methods 
   *
   * Methods are grouped into the the following categories
   *
   * -  Identification
   * -  Navigation
   * -  Module Frame
   * -  Element Extent
   * -  Design methods
   * -  Intersection Tests
   * -  Lorentz Correction
   * -  Readout cell id
   * -  Miscellaneous
   *
   *
   * @author Grant Gorfine
   * - modified & maintained: Nick Styles, Andreas Salzburger
   * - modified Nigel Hessey: get directions from the design instead of hard-wiring them   
   *
   * @par Some notes on Thread safety for  AthenaMT
   *
   * The private methods of this class do not have locks.
   *
   * The method updateCache of is of particular interest as
   * it set all cache values and at the end sets the
   * m_cacheValid atomic variable to true.
   *
   * The const methods call the updateCache() under a mutex lock
   * when the need to perform lazy initialization
   * \code{.cpp}
   * if (!m_cacheValid) {
   *   std::lock_guard<std::mutex> lock(m_mutex);
   *   if (!m_cacheValid) updateCache();
   * }
   * \endcode
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
 

  class SiDetectorElement final : public InDetDD::SolidStateDetectorElementBase {

  public:
    
    /**
     * Constructor with parameters
     */
    SiDetectorElement(const Identifier& id, 
                      const SiDetectorDesign* design,
                      const GeoVFullPhysVol* geophysvol,
                      const SiCommonItems* commonItems,
                      const GeoAlignmentStore* geoAlignStore=nullptr);
    
    /**
     * Destructor
     */
    virtual ~SiDetectorElement();
    
    /**
     * Don't allow no-argument constructor
     */
    SiDetectorElement() = delete;

    /**
     * Don't allow copy constructor
     */
    SiDetectorElement(const SiDetectorElement&) = delete;

    /**
     * Don't allow assignment operator
     */ 
    SiDetectorElement& operator=(const SiDetectorElement&) = delete;

    /**
     * Don't allow move constructor
     */
    SiDetectorElement(SiDetectorElement&&) = delete;

    /**
     * Don't allow move assignment operator
     */
    SiDetectorElement& operator=(SiDetectorElement&&) = delete;


    /**
     * @name Navigation setters
     * Non-const methods.
     * These are inline methods and set neighbours.
     */
    //@{
    void setNextInEta(const SiDetectorElement* element);
    void setPrevInEta(const SiDetectorElement* element);
    void setNextInPhi(const SiDetectorElement* element);
    void setPrevInPhi(const SiDetectorElement* element);
    /**
     * For SCT only
     */
    void setOtherSide(const SiDetectorElement* element);
    //@}

    /**
     * @name Navigation
     * Inline methods to access neighbours.
     */
    //@{
    const SiDetectorElement* nextInEta() const;
    const SiDetectorElement* prevInEta() const;
    const SiDetectorElement* nextInPhi() const;
    const SiDetectorElement* prevInPhi() const;
    /**
     * Useful for SCT only
     */
    const SiDetectorElement* otherSide() const;
    //@}

    /**
     * @name Identification
     * Methods to identify the element and identifier manipulation.
     */ 
    //@{
    
    bool isPixel() const;
    bool isSCT() const;
    bool isDBM() const;
    bool isBarrel() const;
    bool isEndcap() const; // inline
    bool isBlayer() const;
    bool isInnermostPixelLayer() const;
    bool isNextToInnermostPixelLayer() const;

    /**
     * Identifier <-> SiCellId (ie strip number or pixel eta_index,phi_index)
     * Identifier from SiCellId (ie strip number or pixel eta_index,phi_index)
     */
    Identifier identifierFromCellId(const SiCellId &cellId) const;
    /**
     * SiCellId from Identifier
     */
    SiCellId cellIdFromIdentifier(const Identifier& identifier) const;
      
    //@}

    /**
     * @name Surface
     */
    //@{

    /**
     * Returns the full list of surfaces associated to this detector element
     */
    const std::vector<const Trk::Surface*>& surfaces() const;
    //@{

    /**
     * @name Module Frame
     *
     * Methods to help work with the module frame.
     * This is mainly of of use in the SCT where the module frame can
     * in general be different from the element frame. It is actully
     * defined as the frame of one of the sides (currently the axial
     * side), but using these methods one does not need to make any
     * assumptions about what the actual frame is.  In the following
     * methods the local element frame is the local reconstruction
     * frame of this element.
     */
    //@{
    /**
     * Module to global frame transform.
     * Includes misalignment. The module frame is defined to be the
     * local reconstruction frame of the axial layer in the SCT. For
     * the pixel it is the same as the element.
     */
    const Amg::Transform3D& moduleTransform() const;

    /**
     * Default module to global frame transform, ie with no misalignment.
     * The module frame is defined to be the
     * local reconstruction frame of the axial layer in the SCT. For
     * the pixel it is the same as the element.
     */
    Amg::Transform3D defModuleTransform() const;

    /**
     * Take a transform of the local element frame and return its equivalent in the module frame.
     */
    Amg::Transform3D localToModuleFrame(const Amg::Transform3D& localTransform) const;

    /**
     * Transformation from local element to module frame.  This can be
     * used to take a local position in the element frame and transform
     * it to a position in the module frame. If one is already in the
     * module frame it will return the Identity transform.
     */
    Amg::Transform3D localToModuleTransform() const;

    /**
     * Check if the element and module frame are the same.
     */
    bool isModuleFrame() const;
    //@}

    /**
     * @name Element Extent
     *  Methods to get extent of element in r,phi and z.  
     */
    double get_rz() const;
    //@}

    /**
     * @name Angle
     */
    //@{
    /**
     * Compute sin(tilt angle) at a given position:
     * at center
     */
    double sinTilt() const;
    /**
     * at given local position
     */
    double sinTilt(const Amg::Vector2D& localPos) const;
    /**
     * at given global position
     */
    double sinTilt(const HepGeom::Point3D<double>& globalPosition) const;
    
    /**
     * Compute sin(stereo angle) at a given position: at center
     */
    double sinStereo() const;
    /**
     * at given local position
     */
    double sinStereo(const Amg::Vector2D& localPos) const;
    /**
     * at given global position
     */
    double sinStereo(const HepGeom::Point3D<double>& globalPosition) const;
    
    /**
     * Angle of strip in local frame with respect to the etaAxis. 
     * Zero for all elements except trapezoidal detectors (ie SCT forward modules).
     */
    double sinStereoLocal(const Amg::Vector2D& localPos) const;
    /**
     * See previous method
     */
    double sinStereoLocal(const HepGeom::Point3D<double>& globalPos) const;
    
    /**
     * Check if it is the stereo side (useful for SCT)
     */
    bool isStereo() const;
    
    //@}
    
    
    /**
     * @name Design methods
     */
    //@{
    
    /**
     * access to the local description (inline):
     */
    const SiDetectorDesign& design() const;

    /**
     * Pitch (inline methods)
     *
     * NOTE: phiPitch is ambiguous for the Forward SCT where it varies along the strip.\n
     *       etaPitch is ambiguous for the pixel which has long pixels between FE chips.\n
     *
     * For these cases:
     *
     * phiPitch:  For SCT Forward returns pitch at center.\n
     * etaPitch:  For pixel returns average pitch. (Active_length/number_of_cells)\n
     *
     * All return pitch in distance units. 
     */

    double phiPitch() const;
    /**
     * Useful for SCT Forward.
     */
    double phiPitch(const Amg::Vector2D& localPosition) const;

    /**
     * carrier type for readout. ie holes for SCT and electrons for pixels. inline
     */
    InDetDD::CarrierType carrierType() const;

    /**
     * Determine if readout direction between online and offline needs swapping.
     * Returns true if online and offline numbers run in opposite directions.
     * For strip/phi_index (inline)
     */
    bool swapPhiReadoutDirection() const;
    /**
     * For eta_index (only relevant for pixel) (inline)
     */
    bool swapEtaReadoutDirection() const;

    //@}
    
    /**
     * @name Intersection Tests
     */
    //@{
    /**
     * Test if near bond gap within tolerances
     */
    bool nearBondGap(const Amg::Vector2D& localPosition, double etaTol) const;
    bool nearBondGap(const HepGeom::Point3D<double>& globalPosition, double etaTol) const; 
    
    /**
     * If cell is ganged return the id of the other cell which shares the readout
     * for this cell, otherwise return an invalid id.
     * This is a more convenient (and slightly faster) alternative than 
     * using the above two methods.
     * Only relevant for pixel. For SCT always returns an invalid ID. (inline)
     */
    SiCellId gangedCell(const SiCellId& cellId) const;
    
    //@}
    
    /**
     * @name Miscellaneous
     */
    //@{
    /**
     * Special method for SCT to retrieve the two ends of a "strip"
     * Returned coordinates are in global frame.
     */
    std::pair<Amg::Vector3D,Amg::Vector3D> endsOfStrip(const Amg::Vector2D& position) const;
    //@}
    
  private:
    /**
     * @name Private Methods
     *
     * Do not use locks.
     */
    //@{
    /**
     * Common code for constructors.
     */
    void commonConstructor();

    /**
     * Recalculate cached values.
     */
    void updateCache() const;
   
    /**
     * Determine m_isStereo variable and m_stereoCacheValid variable.
     */
    void determineStereo() const;
    
    /**
     * Private implementation method with no lock at center
     */
    double sinStereoImpl() const;
    /**
     * Private implementation method with no lock at given global position
     */
    double sinStereoImpl(const HepGeom::Point3D<double>& globalPosition) const;
    
    /**
     * Protected data:
     */
  protected:

    /**
     * @name Variables for cache validities
     */
    //@{
 
    /**
     * Since m_isStereo depends on m_otherSide->sinStereo(), a dedicated validity variable is needed.
     */
    mutable std::atomic_bool m_stereoCacheValid{false};
    /**
     * Since m_surfaces depends on m_otherSide->surface(), a dedicated validity variable is needed.
     */
    mutable std::atomic_bool m_surfacesValid{false};
    //@}

    /**
     * @name Variable set by surfaces ith m_surfacesValid of false
     * Happens only once
     */
    //@{
    mutable std::vector<const Trk::Surface*> m_surfaces ATLAS_THREAD_SAFE {};
    //@}

    /**
     * @name Mutex guard to update mutable variables in const methods
     */
    //@{
    mutable std::mutex m_mutex{};
    //@}

    /**
     * @name Variables set by constructor
     */
    //@{
    const SiDetectorDesign* m_design;

    /**
     * @name Variables set by commonConstructor
     */
    //@{

    bool m_isPixel{false};
    bool m_isDBM{false};
    bool m_isBarrel{false};
    //@}

    /**
     * @name Variables set by individual setter methods
     */
    //@{
    const SiDetectorElement* m_nextInEta{nullptr}; // set by setNextInEta
    const SiDetectorElement* m_prevInEta{nullptr}; // set by setPrevInEta
    const SiDetectorElement* m_nextInPhi{nullptr}; // set by setNextInPhi
    const SiDetectorElement* m_prevInPhi{nullptr}; // set by setPrevInPhi
    const SiDetectorElement* m_otherSide{nullptr}; // set by setOtherSide
    //@}

    /**
     * @name Variables set by updateCache with m_firstTime of true
     * Happens only once
     *
     * Directions of axes. These are true if the hit/simulation and reconstruction local
     * frames are in the same direction and false if they are opposite.
     */
    //@{

    /**
     * @name Variable set by determineStereo with m_stereoCacheValid of false
     * Happens only once
     */
    //@{
    mutable bool m_isStereo ATLAS_THREAD_SAFE {false};
    //@}

     /**
     * Declaring the Message method for further use (inline)
     */
    MsgStream& msg(MSG::Level lvl) const;
    
    /**
     * Declaring the Method providing Verbosity Level (inline)
     */
    bool msgLvl(MSG::Level lvl) const;
    //@}
    
  };
    
} // namespace InDetDD

/**
 * Include implementation of inline methods
 */
#include "InDetReadoutGeometry/SiDetectorElement.icc"

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
