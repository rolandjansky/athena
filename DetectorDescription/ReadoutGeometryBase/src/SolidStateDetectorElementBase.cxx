/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"

namespace InDetDD {

    SolidStateDetectorElementBase::SolidStateDetectorElementBase(const Identifier &id,
                                                                 const DetectorDesign *design,
                                                                 const GeoVFullPhysVol *geophysvol,
                                                                 SiCommonItems *commonItems):
        TrkDetElementBase(geophysvol),
        m_id(id),
        m_design(design),
        m_commonItems(commonItems)
    {
        if (!m_id.is_valid()) throw std::runtime_error("SolidStateDetectorElementBase: Invalid identifier");

        // Increase the reference count of the DetectorDesign and SiCommonItems objects.
        m_design->ref();
        m_commonItems->ref();

        // Should we reference count the geophysvol as well?

    }

    SolidStateDetectorElementBase::~SolidStateDetectorElementBase()
    {

        // The design and SiCommonItems are reference counted so that it will not be deleted until the last element is deleted.
        m_design->unref();
        m_commonItems->unref();

    }

} // namespace InDetDD
