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

    }

    SolidStateDetectorElementBase::~SolidStateDetectorElementBase()
    {

    }

} // namespace InDetDD
