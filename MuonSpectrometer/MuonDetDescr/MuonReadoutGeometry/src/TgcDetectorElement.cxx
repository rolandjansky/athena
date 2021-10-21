/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 this corresponds to a Set of RPC modules (same station and same doubletR)
// it can be accessed via hash-id;
// it holds pointers to TgcReadoutElements belonging to him
 -------------------------------------------------------------------------
 ***************************************************************************/

#include "MuonReadoutGeometry/TgcDetectorElement.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/Surface.h"

class GeoVFullPhysVol;

namespace MuonGM {

    class MuonDetectorManager;

    TgcDetectorElement::TgcDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idHash) :
        MuonDetectorElement(pv, mgr, id, idHash) {
        m_nREinDetectorElement = 1;
    }

    const Amg::Transform3D& TgcDetectorElement::transform() const { return m_tgcre->transform(); }

    const Trk::Surface& TgcDetectorElement::surface() const { return m_tgcre->surface(); }

    const Trk::SurfaceBounds& TgcDetectorElement::bounds() const { return m_tgcre->bounds(); }

    const Amg::Vector3D& TgcDetectorElement::center() const { return m_tgcre->center(); }

    const Amg::Vector3D& TgcDetectorElement::normal() const { return m_tgcre->normal(); }

    const Trk::Surface& TgcDetectorElement::surface(const Identifier& id) const { return m_tgcre->surface(id); }

    const Trk::SurfaceBounds& TgcDetectorElement::bounds(const Identifier& id) const { return m_tgcre->bounds(id); }

    const Amg::Transform3D& TgcDetectorElement::transform(const Identifier& id) const { return m_tgcre->transform(id); }

    const Amg::Vector3D& TgcDetectorElement::center(const Identifier& id) const { return surface(id).center(); }

    const Amg::Vector3D& TgcDetectorElement::normal(const Identifier& id) const { return m_tgcre->normal(id); }

    std::vector<const Trk::Surface*> TgcDetectorElement::surfaces() const { return m_tgcre->surfaces(); }

}  // namespace MuonGM
