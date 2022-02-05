/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 this corresponds to a Set of RPC modules (same station and same doubletR)
// it can be accessed via hash-id;
// it holds pointers to RpcReadoutElements belonging to him
 -------------------------------------------------------------------------
 ***************************************************************************/

#include "MuonReadoutGeometry/RpcDetectorElement.h"

#include <TString.h>
#include <stdlib.h>

#include <stdexcept>
#include <string>

#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/Surface.h"

class GeoVFullPhysVol;

namespace MuonGM {

    RpcDetectorElement::RpcDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idHash) :
        MuonDetectorElement(pv, mgr, id, idHash), m_ndbz(0) {
        for (unsigned int i = 0; i < NDoubletZ; i++) m_rpcVector[i] = nullptr;
        m_nREinDetectorElement = 0;
        m_helper = manager()->rpcIdHelper();
    }

    void RpcDetectorElement::addRpcReadoutElement(const RpcReadoutElement* rpc, int index) {
        if (index < NDoubletZ) {
            if (!m_rpcVector[index]) {
                m_rpcVector[index] = rpc;
                // everything ok
                m_nREinDetectorElement++;
            } else {
                throw std::runtime_error(
                    Form("File: %s, Line: %d\nRpcDetectorElement::addRpcReadoutElement() - problems for idhash %d: index %d is taken.",
                         __FILE__, __LINE__, (unsigned int)identifyHash(), index));
            }
        } else {
            throw std::runtime_error(
                Form("File: %s, Line: %d\nRpcDetectorElement::addRpcReadoutElement() - problems for idhash %d: index %d is >= %d.",
                     __FILE__, __LINE__, (unsigned int)identifyHash(), index, NDoubletZ));
        }
    }

    const RpcReadoutElement* RpcDetectorElement::getRpcReadoutElement(Identifier id) const {
        int dbz = m_helper->doubletZ(id);
        int dbp = m_helper->doubletPhi(id);
        return getRpcReadoutElement(dbz, dbp);
    }

    const RpcReadoutElement* RpcDetectorElement::getRpcReadoutElement(int dbz, int dbp) const {
        int dbz_index = dbz - 1;
        std::string stName = m_rpcVector[0]->getStationName();

        if (stName == "BMS") {
            int stEta = m_rpcVector[0]->getStationEta();
            int dbr = m_rpcVector[0]->getDoubletR();
            if (std::abs(stEta) == 2 && dbz == 3) {
                if (dbp == 2) dbz_index++;

            } else if (std::abs(stEta) == 4 && dbr == 2 && dbz == 3) {
                if (dbp == 2) dbz_index++;

            } else if (std::abs(stEta) == 4 && dbr == 1 && dbz == 2) {
                if (dbp == 2) dbz_index++;
            }
        }

        if (dbz_index < (int)nReadoutElements() && dbz_index >= 0)
            return m_rpcVector[dbz_index];
        else
            return nullptr;
    }

    const Amg::Transform3D& RpcDetectorElement::transform() const { return m_rpcVector[0]->transform(); }

    const Trk::Surface& RpcDetectorElement::surface() const { return m_rpcVector[0]->surface(); }

    const Trk::SurfaceBounds& RpcDetectorElement::bounds() const { return m_rpcVector[0]->bounds(); }

    const Amg::Vector3D& RpcDetectorElement::center() const { return m_rpcVector[0]->center(); }

    const Amg::Vector3D& RpcDetectorElement::normal() const { return m_rpcVector[0]->normal(); }

    const Trk::Surface& RpcDetectorElement::surface(const Identifier& id) const { return getRpcReadoutElement(id)->surface(id); }

    const Trk::SurfaceBounds& RpcDetectorElement::bounds(const Identifier& id) const { return getRpcReadoutElement(id)->bounds(id); }

    const Amg::Transform3D& RpcDetectorElement::transform(const Identifier& id) const { return getRpcReadoutElement(id)->transform(id); }

    const Amg::Vector3D& RpcDetectorElement::center(const Identifier& id) const { return surface(id).center(); }

    const Amg::Vector3D& RpcDetectorElement::normal(const Identifier& id) const { return m_rpcVector[0]->normal(id); }

    std::vector<const Trk::Surface*> RpcDetectorElement::surfaces() const {
        std::vector<const Trk::Surface*> detectorSurfaces;
        for (unsigned int i = 0; i < NDoubletZ; i++) {
            std::vector<const Trk::Surface*> surfs = m_rpcVector[i]->surfaces();
            detectorSurfaces.insert(detectorSurfaces.end(), surfs.begin(), surfs.end());
        }

        return detectorSurfaces;
    }

}  // namespace MuonGM
