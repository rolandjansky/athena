/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"

#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

// #include "CLHEP/Geometry/Point3D.h"
// #include "CLHEP/Geometry/Vector3D.h"
// #include "CLHEP/Vector/ThreeVector.h"
// #include "CLHEP/Units/PhysicalConstants.h" // for M_PI
// #include "CLHEP/Units/SystemOfUnits.h"

// #include "TrkSurfaces/PlaneSurface.h"
// #include "TrkSurfaces/SurfaceBounds.h"

// #include "InDetCondServices/ISiLorentzAngleSvc.h"
// #include <cmath>
// #include <cassert>
// #include <limits>

namespace HGTDGeo {

// Constructor with parameters:
HGTD_DetectorElement::HGTD_DetectorElement(const Identifier &id,
                                           const HGTD_DetectorDesign *design,
                                           const GeoVFullPhysVol *geophysvol,
                                           SiCommonItems * commonItems) :
  SolidStateDetectorElementBase(id, design, geophysvol, commonItems),
  m_cacheValid(false),
{
    const HGTD_ID* hgtdId = dynamic_cast<const HGTD_ID *>(getIdHelper());
    m_idHash = hgtdId->wafer_hash(m_id); // TODO: implement method in HGTD_ID for returning hash

    if (!m_idHash.is_valid()) throw std::runtime_error("HGTD_DetectorElement: Unable to set IdentifierHash");
}

// Destructor:
HGTD_DetectorElement::~HGTD_DetectorElement()
{
    delete m_surface;
}

void 
HGTD_DetectorElement::updateCache() const
{
    SolidStateDetectorElementBase::updateCache();
    m_cacheValid = true;
}

Identifier 
HGTD_DetectorElement::identifierFromCellId(const SiCellId & cellId) const
{
    Identifier id; // Will be initialized in an invalid state.
    // If something fails it returns the id in an invalid state.
    if (cellId.isValid()) {
        const HGTD_ID * hgtdIdHelper = static_cast<const HGTD_ID *>(getIdHelper());
        if (hgtdIdHelper) {
            id = hgtdIdHelper->pixel_id(m_id, cellId.phiIndex(), cellId.etaIndex()); // TODO: implement method in HGTD_ID for returning Id
        }
    }

    return id;
}

SiCellId
HGTD_DetectorElement::cellIdFromIdentifier(const Identifier & identifier) const
{
    SiCellId cellId; // Initialized in invalid state.
    // If something fails it returns the cellId in an invalid state.
    if (identifier.is_valid()) {
        const HGTD_ID * hgtdIdHelper = static_cast<const HGTD_ID *>(getIdHelper());
        if (hgtdIdHelper) {
            cellId = SiCellId(hgtdIdHelper->phi_index(identifier), hgtdIdHelper->eta_index(identifier)); // TODO: implement method in HGTD_ID for returning indices (x/y instead of phi/eta?)
        }
    }
  
    return cellId;
}

double HGTD_DetectorElement::get_rz() const
{
    // Calculate z (endcap)
    // Use center of sensor ((0,0,0) in local coordinates) for determining this.
    //  HepGeom::Point3D<double> globalCenter = globalPosition(HepGeom::Point3D<double>(0,0,0));
    return center().z();
}

} // namespace HGTDGeo
