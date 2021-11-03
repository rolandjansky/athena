/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "HGTD_Identifier/HGTD_ID.h"

#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TrkSurfaces/PlaneSurface.h"

namespace InDetDD {

// Constructor with parameters:
HGTD_DetectorElement::HGTD_DetectorElement(const Identifier &id,
                                           const HGTD_ModuleDesign *design,
                                           const GeoVFullPhysVol *geophysvol,
                                           const SiCommonItems * commonItems,
                                           const GeoAlignmentStore* geoAlignStore) :
  SolidStateDetectorElementBase(id, design, geophysvol, commonItems, geoAlignStore),
  m_design(design)
{
    const HGTD_ID* hgtdId = dynamic_cast<const HGTD_ID *>(getIdHelper());
    m_idHash = hgtdId->wafer_hash(m_id);

    if (!m_idHash.is_valid()) throw std::runtime_error("HGTD_DetectorElement: Unable to set IdentifierHash");
}

// Destructor:
HGTD_DetectorElement::~HGTD_DetectorElement()
{
}

Identifier 
HGTD_DetectorElement::identifierFromCellId(const SiCellId & cellId) const
{
    Identifier id; // Will be initialized in an invalid state.
    // If something fails it returns the id in an invalid state.
    if (cellId.isValid()) {
        const HGTD_ID * hgtdIdHelper = static_cast<const HGTD_ID *>(getIdHelper());
        if (hgtdIdHelper) {
            id = hgtdIdHelper->pixel_id(m_id, cellId.phiIndex(), cellId.etaIndex());
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
            cellId = SiCellId(hgtdIdHelper->phi_index(identifier), hgtdIdHelper->eta_index(identifier));
        }
    }
  
    return cellId;
}

const std::vector<const Trk::Surface*>& HGTD_DetectorElement::surfaces() const 
{
    // This method is needed to satisfy inheritance from TrkDetElementBase
    // so just return the one surface
    if (!m_surfaces.isValid()) {
      std::vector<const Trk::Surface*> s;
      // get this surface
      s.push_back(&surface());
      m_surfaces.set (std::move (s));
    }

    // return the surfaces
    return *m_surfaces.ptr();
}

double HGTD_DetectorElement::get_rz() const
{
    // Calculate z (endcap)
    // Use center of sensor ((0,0,0) in local coordinates) for determining this.
    return center().z();
}

} // namespace InDetDD
