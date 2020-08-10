/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolidStateDetectorElementBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H
#define READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H

// Base class.
#include "TrkDetElementBase/TrkDetElementBase.h"

#include "ReadoutGeometryBase/DetectorDesign.h"
#include "ReadoutGeometryBase/SiIntersect.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "ReadoutGeometryBase/SiCellId.h"
#include "ReadoutGeometryBase/SiReadoutCellId.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

namespace Trk{
    class Surface;
}

namespace InDetDD {

class SolidStateDetectorElementBase : public Trk::TrkDetElementBase {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    // Constructor
    SolidStateDetectorElementBase(const Identifier &id,
                                  const DetectorDesign *design,
                                  const GeoVFullPhysVol *geophysvol,
                                  SiCommonItems *commonItems);

    // Destructor
    ~SolidStateDetectorElementBase();

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Identification
    /// Methods to identify the element and identifier manipulation.
    //
    ///////////////////////////////////////////////////////////////////

    //@{

    /// identifier of this detector element:
    virtual Identifier identify() const;

    /// identifier hash
    virtual IdentifierHash identifyHash() const;

    /// Returns the id helper
    const AtlasDetectorID* getIdHelper() const;

    // Identifier <-> SiCellId (ie strip number or pixel eta_index,phi_index)
    // pure virtual so that the correct ID helper can be used in the derived classes

    /// Identifier from SiCellId
    virtual Identifier identifierFromCellId(const SiCellId & cellId) const = 0;

    /// SiCellId from Identifier
    virtual SiCellId   cellIdFromIdentifier(const Identifier & identifier) const = 0;

    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Navigation
    /// Methods to access neighbours.
    //
    ///////////////////////////////////////////////////////////////////

    //@{
    virtual std::vector<const SolidStateDetectorElementBase *> neighbours() const;
    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Transformation/Orientation
    //
    ///////////////////////////////////////////////////////////////////

    //@{
    // Position 
    /// Local (simulation/hit frame) to global transform
    virtual const HepGeom::Transform3D & transformHit() const; 
    /// Local (reconstruction frame) to global transform
    const Amg::Transform3D & transform() const; 
    /// Default Local (reconstruction frame) to global transform
    /// ie with no misalignment. 
    const HepGeom::Transform3D defTransformCLHEP() const; 
    const Amg::Transform3D defTransform() const; 
    /// Center of the sensor in global coordinates
    const Amg::Vector3D & center() const; 
    /// Center of the local reference system in global coordinates
    const Amg::Vector3D & origin() const; 
    
    const HepGeom::Transform3D & transformCLHEP() const;

    /// Transform to go from local reconstruction frame to local hit frame.
    const HepGeom::Transform3D recoToHitTransform() const;

    /// Simulation/Hit local frame to reconstruction local frame. 2D.
    //  TODO: Will change order of parameters at some point.
    Amg::Vector2D hitLocalToLocal(double xEta, double xPhi) const;
    /// Same as previuos method but 3D.
    HepGeom::Point3D<double> hitLocalToLocal3D(const HepGeom::Point3D<double> & hitPosition) const; 

    /// Directions of hit depth,phi,eta axes relative to reconstruction local position
    /// axes (LocalPosition). Returns +/-1.
    double hitDepthDirection() const;
    /// See previous method.
    double hitPhiDirection() const;
    /// See previous method.
    double hitEtaDirection() const;
    
    
    // Orientation. 
    // Directions.
    //  phiAxis - in same direction as increasing phi and identifier phi_index/strip. 
    //            NB. This requires some flipping of axes with repsect to the hits.  
    //  etaAxis - in direction of increasing z in the barrel and increasing r in the endcap. 
    //  normal  - choosen to give right-handed coordinate frame (x=normal,y=phiAxis,z=etaAxis)
    //            NB. This requires some flipping of axes with repsect to the hits.  
      
    /// Get reconstruction local phi axes in global frame. 
    /// In same direction as increasing phi and identifier phi_index/strip. 
    const Amg::Vector3D & phiAxis() const;
    const HepGeom::Vector3D<double> & phiAxisCLHEP() const;
    /// Get reconstruction local eta axes in global frame. 
    /// In direction of increasing z in the barrel and increasing r in the endcap. 
    const Amg::Vector3D & etaAxis() const;
    const HepGeom::Vector3D<double> & etaAxisCLHEP() const;
    /// Get reconstruction local normal axes in global frame. Choosen to give right-handed coordinate frame.
    const Amg::Vector3D & normal() const;

    /// transform a hit local position into a global position:
    HepGeom::Point3D<double> globalPositionHit(const HepGeom::Point3D<double> &simulationLocalPos) const;
    Amg::Vector3D globalPositionHit(const Amg::Vector3D &simulationLocalPos) const;
      
    /// transform a reconstruction local position into a global position:
    HepGeom::Point3D<double> globalPosition(const HepGeom::Point3D<double> &localPos) const;
    Amg::Vector3D globalPosition(const Amg::Vector3D &localPos) const;

    /// as in previous method but for 2D local position
    HepGeom::Point3D<double> globalPositionCLHEP(const Amg::Vector2D &localPos) const;
      
    Amg::Vector3D globalPosition(const Amg::Vector2D &localPos) const;
    
    /// transform a global position into a 2D local position (reconstruction frame)
    Amg::Vector2D localPosition(const HepGeom::Point3D<double> & globalPosition) const;

    Amg::Vector2D localPosition(const Amg::Vector3D& globalPosition) const;

    //@}

    /// Element Surface
    virtual const Trk::Surface & surface() const;

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Element Extent
    /// Methods to get extent of element in r,phi and z.  
    ///////////////////////////////////////////////////////////////////

    //@{
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

    virtual double get_rz() const = 0;
    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Design methods
    //
    ///////////////////////////////////////////////////////////////////
    //@{

    /// access to the local description:
    virtual const DetectorDesign &design() const = 0;

    // Methods from design
    double width() const; // Width in phi direction. For the SCT endcap it returns the average width.
    double minWidth() const; // Min, max width. Needed for the SCT endcap.
    double maxWidth() const;
    double length() const; // Length in eta direction (z - barrel, r - endcap)
    double thickness() const;

    virtual const Trk::SurfaceBounds & bounds() const;

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
    double etaPitch() const;
    double phiPitch() const;
    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Intersection Tests
    //
    ///////////////////////////////////////////////////////////////////

    //@{
    // Test that it is in the active region
    // Intersect has 3 states
    // bool SiIntersect::in() const // definitely in
    // bool SiIntersect::out() const // definitely out
    // bool SiIntersect::nearBoundary() const // near a boundary within the tolerances
    // bool SiIntersect::mayIntersect() const // in() OR nearBoundary()
    SiIntersect inDetector(const Amg::Vector2D & localPosition, double phiTol, double etaTol) const;
    SiIntersect inDetector(const HepGeom::Point3D<double> & globalPosition, double phiTol, double etaTol) const;
    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Lorentz Correction
    //
    ///////////////////////////////////////////////////////////////////
    //@{

    /// tan(Lorentz angle). Component in phi direction (hit frame)
    double getTanLorentzAnglePhi() const; 
    
    /// Same as getTanLorentzAnglePhi()
    double getTanLorentzAngle() const; 
    
    /// tan(Lorentz angle). Component in eta direction (hit frame)
    double getTanLorentzAngleEta() const; 
    
    /// Lorentz shift in distPhi (reconstruction local frame)
    double getLorentzCorrection() const; 
    
    /// Correct a local position (reconstruction frame) for the Lorentz angle:
    /// Users generally shouldn't need to call this as localPositionOfCell() returns the Lorentz corrected position.
    Amg::Vector2D correctLocalPosition(const Amg::Vector2D &position) const;

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

    /// Full identifier of the cell for a given position:
    /// assumes a raw local position (no Lorentz shift)
    Identifier identifierOfPosition(const Amg::Vector2D &localPos) const;
    /// As in previous method but returns SiCellId 
    SiCellId cellIdOfPosition(const Amg::Vector2D &localPos) const;

    /// Returns position (center) of cell. These are corrected for the Lorentz shift
    Amg::Vector2D correctedLocalPositionOfCell(const SiCellId & cellId) const;
    /// As above
    Amg::Vector2D correctedLocalPositionOfCell(const Identifier & id) const;

    /// Returns position (center) of cell. These are the raw positions *NOT* corrected for the Lorentz shift
    Amg::Vector2D localPositionOfCell(const SiCellId & cellId) const;
    /// As above
    Amg::Vector2D localPositionOfCell(const Identifier & id) const;

    /// Test if readout cell has more than one diode associated with it.
    /// Number of cells sharing the same readout as this cell.
    /// ie generally 1 except for ganged pixels which will be 2.
    int numberOfConnectedCells(const SiCellId cellId) const;
    /// Get the cell ids sharing the readout for this cell.
    /// number = 0 will return the primary readout cell id.
    SiCellId connectedCell(const SiCellId cellId, int number) const;

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
    /// Invalidate general cache
    void invalidate() const;

    /// Signal that cached values are no longer valid.
    /// invalidate conditions cache
    void invalidateConditions() const;

    /// Recalculate all cached values.
    virtual void updateCache() const;

    /// Recalculate subset of cached values.
    void updateConditionsCache() const;

    /// Keep the actual implementation of this method in derived classes
    /// to ensure that the updateCache implementation of the derived class gets called
    /// instead of just the one from this class
    virtual void updateAllCaches() const = 0;

    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Methods to satisfy TrkDetElementBase interface
    //
    ///////////////////////////////////////////////////////////////////
    //{@
    virtual const Amg::Transform3D & transform(const Identifier&) const {return transform();}
    virtual const Trk::Surface& surface (const Identifier&) const {return surface();}
    virtual const Amg::Vector3D& center (const Identifier&) const {return center();}
    virtual const Amg::Vector3D& normal (const Identifier&) const {return normal();}
    virtual const Trk::SurfaceBounds & bounds(const Identifier&) const {return bounds();}
    //@}

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////

private:

    // Calculate extent in r,z and phi. The values are cached and there
    // are rMin(), rMax etc methods.
    void getExtent(double &rMin, double &rMax,
                   double &zMin, double &zMax,
                   double &phiMin, double &phiMax) const;

    // Return the four corners of an element in local coordinates.
    // Pass it an array of length 4.
    // This function is used by getEtaPhiRegion()
    void getCorners(HepGeom::Point3D<double> *corners) const;

    // Get eta and phi coresponding to a point in local coordinates. 
    // Requires as input the vertex spread. Returns etaMin, etaMax, and phi.
    // This function is used by getEtaPhiRegion()
    void getEtaPhiPoint(const HepGeom::Point3D<double> & point, double deltaZ, 
                        double &etaMin, double &etaMax, double &phi) const;
    

protected:
    //Declaring the Message method for further use
    MsgStream& msg (MSG::Level lvl) const { return m_commonItems->msg(lvl);}
    
    //Declaring the Method providing Verbosity Level
    bool msgLvl (MSG::Level lvl) const { return m_commonItems->msgLvl(lvl);}

public:
    
    
    ///////////////////////////////////////////////////////////////////
    /// @name Non-const methods:
    /// These are not needed by most clients.
    ///////////////////////////////////////////////////////////////////
    //@{
    // Set neighbours. 
    
    void setNeighbours(std::vector<const SolidStateDetectorElementBase *>);
    void addNeighbour(const SolidStateDetectorElementBase *);
    
    //@}

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    // Don't allow copying.
    SolidStateDetectorElementBase();
    SolidStateDetectorElementBase(const SolidStateDetectorElementBase&);
    SolidStateDetectorElementBase &operator=(const SolidStateDetectorElementBase&);

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Protected data:
    ///////////////////////////////////////////////////////////////////
protected:
    Identifier m_id; // identifier of this detector element
    IdentifierHash m_idHash; // hash id
    const DetectorDesign *m_design; // local description of this detector element
    SiCommonItems * m_commonItems;

    std::vector<const SolidStateDetectorElementBase *> m_neighbours;

    //
    // Cached values.
    //
    // Axes
    DetectorDesign::Axis m_hitEta;
    DetectorDesign::Axis m_hitPhi;
    DetectorDesign::Axis m_hitDepth;

    mutable bool m_baseCacheValid; // Alignment associated quatities.
    mutable bool m_conditionsCacheValid; // Lorentz angle related values.
    mutable bool m_firstTimeBase;
    mutable bool m_barrelLike;

    // Directions of axes. These are true if the hit/simulation and reconstruction local frames are
    // in the same direction and false if they are opposite.
    mutable bool m_depthDirection; // Direction of depth axis.
                                   // Also direction of readout implant (n+ for pixel, p+ for SCT).
    mutable bool m_phiDirection;   //
    mutable bool m_etaDirection;   //

    mutable Amg::Transform3D m_transform;
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
    mutable Amg::Vector3D m_origin;
    mutable HepGeom::Vector3D<double> m_originCLHEP;

    mutable double m_minZ;
    mutable double m_maxZ;
    mutable double m_minR;
    mutable double m_maxR;
    mutable double m_minPhi;
    mutable double m_maxPhi;

    mutable double m_tanLorentzAnglePhi;
    mutable double m_tanLorentzAngleEta;
    mutable double m_lorentzCorrection;

    mutable Trk::Surface * m_surface;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline std::vector<const SolidStateDetectorElementBase *> SolidStateDetectorElementBase::neighbours() const
{
    return m_neighbours;
}

inline void SolidStateDetectorElementBase::setNeighbours(std::vector<const SolidStateDetectorElementBase *> neighbours)
{
    m_neighbours = neighbours;
}

inline void SolidStateDetectorElementBase::addNeighbour(const SolidStateDetectorElementBase * neighbour)
{
    m_neighbours.push_back(neighbour);
}

inline HepGeom::Point3D<double> SolidStateDetectorElementBase::globalPositionHit(const HepGeom::Point3D<double> &localPos) const
{
    return transformHit()*localPos;
}

inline Amg::Vector3D SolidStateDetectorElementBase::globalPosition(const Amg::Vector2D &localPos) const
{
    if (!m_baseCacheValid) updateCache();
    return m_origin + localPos[Trk::distEta] * m_etaAxis + localPos[Trk::distPhi] * m_phiAxis;
}

inline Amg::Vector3D SolidStateDetectorElementBase::globalPositionHit(const Amg::Vector3D &localPos) const
{
    return Amg::CLHEPTransformToEigen(transformHit()) * localPos;
}

inline HepGeom::Point3D<double> SolidStateDetectorElementBase::globalPositionCLHEP(const Amg::Vector2D &localPos) const
{
    if (!m_baseCacheValid) updateCache();
    return m_originCLHEP + localPos[Trk::distEta] * m_etaAxisCLHEP + localPos[Trk::distPhi] * m_phiAxisCLHEP;
}

inline Amg::Vector3D SolidStateDetectorElementBase::globalPosition(const Amg::Vector3D &localPos) const
{
    return transform() * localPos;
}

inline HepGeom::Point3D<double> SolidStateDetectorElementBase::globalPosition(const HepGeom::Point3D<double> &localPos) const
{
    return transformCLHEP() * localPos;
}

inline Amg::Vector2D SolidStateDetectorElementBase::localPosition(const HepGeom::Point3D<double> & globalPosition) const
{
    if (!m_baseCacheValid) updateCache();
    HepGeom::Vector3D<double> relativePos = globalPosition - m_originCLHEP;
    return Amg::Vector2D(relativePos.dot(m_phiAxisCLHEP), relativePos.dot(m_etaAxisCLHEP));
}

inline Amg::Vector2D SolidStateDetectorElementBase::localPosition(const Amg::Vector3D & globalPosition) const{
    if (!m_baseCacheValid) updateCache();
    Amg::Vector3D relativePos = globalPosition - m_origin;
    return Amg::Vector2D(relativePos.dot(m_phiAxis), relativePos.dot(m_etaAxis));
}




inline double SolidStateDetectorElementBase::hitDepthDirection() const
{
    if (!m_baseCacheValid) updateCache();
    return (m_depthDirection) ? 1. : -1.;
}

inline double SolidStateDetectorElementBase::hitPhiDirection() const
{
    if (!m_baseCacheValid) updateCache();
    return (m_phiDirection) ? 1. : -1.;
}

inline double SolidStateDetectorElementBase::hitEtaDirection() const
{
    if (!m_baseCacheValid) updateCache();
    return (m_etaDirection) ? 1. : -1.;
}

inline Identifier SolidStateDetectorElementBase::identify() const
{
    return m_id;
}

inline IdentifierHash SolidStateDetectorElementBase::identifyHash() const
{
    return m_idHash;
}

inline const AtlasDetectorID* SolidStateDetectorElementBase::getIdHelper() const
{
    return m_commonItems->getIdHelper();
}

inline void SolidStateDetectorElementBase::invalidate() const
{
    m_baseCacheValid = false;
}

inline void SolidStateDetectorElementBase::invalidateConditions() const
{
    m_conditionsCacheValid = false;
}

inline double SolidStateDetectorElementBase::rMin() const 
{
    if (!m_baseCacheValid) updateCache();
    return m_minR;
}

inline double SolidStateDetectorElementBase::rMax() const 
{
    if (!m_baseCacheValid) updateCache();
    return m_maxR;
}

inline double SolidStateDetectorElementBase::zMin() const 
{
    if (!m_baseCacheValid) updateCache();
    return m_minZ;
}

inline double SolidStateDetectorElementBase::zMax() const 
{
    if (!m_baseCacheValid) updateCache();
    return m_maxZ;
}

inline double SolidStateDetectorElementBase::phiMin() const 
{
    if (!m_baseCacheValid) updateCache();
    return m_minPhi;
}

inline double SolidStateDetectorElementBase::phiMax() const 
{
    if (!m_baseCacheValid) updateCache();
    return m_maxPhi;
}

inline double SolidStateDetectorElementBase::width() const
{
    return m_design->width();
}

inline double SolidStateDetectorElementBase::minWidth() const
{
    return m_design->minWidth();
}

inline double SolidStateDetectorElementBase::maxWidth() const
{
    return m_design->maxWidth();
}

inline double SolidStateDetectorElementBase::length() const
{
    return m_design->length();
}

inline double SolidStateDetectorElementBase::thickness() const
{
    return m_design->thickness();
}

inline double SolidStateDetectorElementBase::etaPitch() const
{
    return m_design->etaPitch();
}

inline double SolidStateDetectorElementBase::phiPitch() const
{
    return m_design->phiPitch();
}

inline double SolidStateDetectorElementBase::getTanLorentzAnglePhi() const
{
    if (!m_conditionsCacheValid) updateConditionsCache();
    return m_tanLorentzAnglePhi;
}

// Same as getTanLorentzAnglePhi()
inline double SolidStateDetectorElementBase::getTanLorentzAngle() const
{
    if (!m_conditionsCacheValid) updateConditionsCache();
    return m_tanLorentzAnglePhi;
}

inline double SolidStateDetectorElementBase::getTanLorentzAngleEta() const
{
    if (!m_conditionsCacheValid) updateConditionsCache();
    return m_tanLorentzAngleEta;
}

inline double SolidStateDetectorElementBase::getLorentzCorrection() const
{
    if (!m_conditionsCacheValid) updateConditionsCache();
    return m_lorentzCorrection;
}

} // namespace InDetDD

#endif // READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H
