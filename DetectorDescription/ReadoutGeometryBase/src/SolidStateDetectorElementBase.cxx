/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"

namespace InDetDD {

    SolidStateDetectorElementBase::SolidStateDetectorElementBase(const Identifier &id,
                                                                 const GeoVFullPhysVol *geophysvol,
                                                                 SiCommonItems *commonItems):
        TrkDetElementBase(geophysvol),
        m_id(id),
        m_commonItems(commonItems)
    {
        if (!m_id.is_valid()) throw std::runtime_error("SolidStateDetectorElementBase: Invalid identifier");

        // Increase the reference count of the SiCommonItems objects.
        m_commonItems->ref();

        // Should we reference count the geophysvol as well?

    }

    SolidStateDetectorElementBase::~SolidStateDetectorElementBase()
    {

        // The SiCommonItems is reference counted so that it will not be deleted until the last element is deleted.
        m_commonItems->unref();

    }

} // namespace InDetDD
