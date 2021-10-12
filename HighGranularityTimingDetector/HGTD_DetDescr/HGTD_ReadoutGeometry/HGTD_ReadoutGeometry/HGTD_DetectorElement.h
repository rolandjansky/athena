/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "HGTD_ReadoutGeometry/HGTD_ModuleDesign.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "ReadoutGeometryBase/SiCellId.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

class AtlasDetectorID;
class GeoVFullPhysVol;

namespace InDetDD {

/**

    @class HGTD_DetectorElement

    Class to hold geometrical description of an HGTD detector element.
    A detector element in this case is a single module in the HGTD.

*/

class HGTD_DetectorElement final : public SolidStateDetectorElementBase {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    /// Constructor:
    HGTD_DetectorElement(const Identifier &id, 
                         const HGTD_ModuleDesign *design,
                         const GeoVFullPhysVol *geophysvol,
                         const SiCommonItems * commonItems,
                         const GeoAlignmentStore* geoAlignStore=nullptr);

    /// Destructor:
    virtual ~HGTD_DetectorElement();

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Identification
    /// Methods to identify the element and identifier manipulation.
    //
    ///////////////////////////////////////////////////////////////////

    /// Identifier from SiCellId (ie pixel eta_index,phi_index)
    Identifier identifierFromCellId(const SiCellId & cellId) const override final;
    
    /// SiCellId from Identifier
    SiCellId   cellIdFromIdentifier(const Identifier & identifier) const override final;

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Transformation/Orientation
    //
    ///////////////////////////////////////////////////////////////////

    /** Returns the full list of surfaces associated to this detector element */
    virtual const std::vector<const Trk::Surface*>& surfaces() const;

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Element Extent
    /// Methods to get extent of element in r,phi and z.  
    ///////////////////////////////////////////////////////////////////
    
    //@{
    double get_rz() const override final;
    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Design methods
    //
    ///////////////////////////////////////////////////////////////////
    //@{
    
    /// access to the local description:
    const HGTD_ModuleDesign &design() const override final;

    // carrier type for readout. ie electrons for pixels.
    InDetDD::CarrierType carrierType() const;

    //@}

    ///////////////////////////////////////////////////////////////////
    // Protected data:
    ///////////////////////////////////////////////////////////////////
protected:

    /**
     * @name Cache vector of surfaces.
     */
    //@{
    CxxUtils::CachedValue<std::vector<const Trk::Surface*> > m_surfaces;
    //@}

    /**
     * @name Variables set by constructor
     */
    //@{
    const HGTD_ModuleDesign* m_design;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline const HGTD_ModuleDesign &HGTD_DetectorElement::design() const
{
  return *m_design;
}

inline InDetDD::CarrierType HGTD_DetectorElement::carrierType() const
{
  return m_design->carrierType();
}

} // namespace InDetDD

#endif // HGTD_READOUTGEOMETRY_HGTD_DETECTORELEMENT_H
