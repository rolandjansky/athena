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
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "ReadoutGeometryBase/SiCellId.h"

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
    /// @name Transformation/Orientation
    //
    ///////////////////////////////////////////////////////////////////

    //@{

    /// Element Surface
    virtual const Trk::Surface & surface() const;

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

    /// Recalculate all cached values.
    virtual void updateCache() const;

    virtual void updateAllCaches() const = 0;

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Methods to satisfy TrkDetElementBase interface
    //
    ///////////////////////////////////////////////////////////////////
    //{@
    //virtual const Amg::Transform3D & transform(const Identifier&) const {return transform();}
    virtual const Trk::Surface& surface (const Identifier&) const {return surface();}
    //virtual const Amg::Vector3D& center (const Identifier&) const {return center();}
    //virtual const Amg::Vector3D& normal (const Identifier&) const {return normal();}
    //virtual const Trk::SurfaceBounds & bounds(const Identifier&) const {return bounds();}
    //@}

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

    mutable bool m_cacheValid; // Alignment associated quatities.

    mutable Trk::Surface * m_surface;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

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
    m_cacheValid = false;
}

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
