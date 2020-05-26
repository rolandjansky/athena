/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// HGTD_DetectorElement.h, (c) ATLAS Detector software
//////////////////////////////////////////////////////////

#ifndef HGTD_READOUTGEOMETRY_HGTD_DETECTORELEMENT_H
#define HGTD_READOUTGEOMETRY_HGTD_DETECTORELEMENT_H

// Base class.
#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"

// Data member classes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorDesign.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "ReadoutGeometryBase/SiCellId.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

class AtlasDetectorID;
class GeoVFullPhysVol;

namespace HGTDGeo {

/**

    @class HGTD_DetectorElement

    TODO: fill in class description

*/

class HGTD_DetectorElement : public SolidStateDetectorElementBase {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    /// Constructor:
    HGTD_DetectorElement(const Identifier &id, 
                         const HGTD_DetectorDesign *design,
                         const GeoVFullPhysVol *geophysvol,
                         SiCommonItems * commonItems);

    /// Destructor:
    virtual ~HGTD_DetectorElement();

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Identification
    /// Methods to identify the element and identifier manipulation.
    //
    ///////////////////////////////////////////////////////////////////

    /// Identifier from SiCellId (ie strip number or pixel eta_index,phi_index)
    Identifier identifierFromCellId(const SiCellId & cellId) const;
    
    /// SiCellId from Identifier
    SiCellId   cellIdFromIdentifier(const Identifier & identifier) const;

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
    const HGTD_DetectorDesign &design() const;

    // carrier type for readout. ie electrons for pixels.
    InDetDD::CarrierType carrierType() const;

    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Cache handling.
    //
    ///////////////////////////////////////////////////////////////////
    //@{.

    /// Recalculate all cached values. 
    void updateCache() const;

    /// Update all caches including surfaces.
    void updateAllCaches() const;

    //@}

    ///////////////////////////////////////////////////////////////////
    // Protected data:
    ///////////////////////////////////////////////////////////////////
protected:

    //
    // Cached values.
    //
    mutable bool m_cacheValid; // used only for HGTD_DetectorElement specific cache values

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline const HGTD_DetectorDesign &HGTD_DetectorElement::design() const
{
  return *dynamic_cast<const HGTD_DetectorDesign *>(m_design);
}

inline InDetDD::CarrierType HGTD_DetectorElement::carrierType() const
{
  return dynamic_cast<const HGTD_DetectorDesign *>(m_design)->carrierType();
}

inline void HGTD_DetectorElement::updateAllCaches() const
{
    if (!m_cacheValid) updateCache();
    if (!m_conditionsCacheValid) updateConditionsCache();
    if (!m_surface) surface();
}

} // namespace HGTDGeo

#endif // HGTD_READOUTGEOMETRY_HGTD_DETECTORELEMENT_H
