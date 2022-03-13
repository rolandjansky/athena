/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONSTATIONINTERSECTCOND_MuonIntersectGeoData_H
#define MUONSTATIONINTERSECTCOND_MuonIntersectGeoData_H
#include <MuonReadoutGeometry/MuonDetectorManager.h>
#include <MuonStationIntersectCond/MdtIntersectGeometry.h>

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

/**
 * @brief Conditions data to cache the MDT chamber geometry used for the intersections and calculations
 *        of holes in a Mdt Segement
 */

class MsgStream;
class MdtCondDbData;

namespace Muon {
    class IMuonIdHelperSvc;
    class MuonIntersectGeoData {
    public:
        ~MuonIntersectGeoData();
        MuonIntersectGeoData();
        MuonIntersectGeoData(MsgStream& msg, const MuonGM::MuonDetectorManager* detMgr, const IMuonIdHelperSvc* idHelperSvc,
                             const MdtCondDbData* cond_data);

        Muon::MuonStationIntersect tubesCrossedByTrack(const Identifier& id, const Amg::Vector3D& pos, const Amg::Vector3D& dir) const;

        /** @brief get geometry description of the given chamber + neighbouring chambers */
        std::vector<const MdtIntersectGeometry*> getStationGeometry(const Identifier& id) const;

        const MuonGM::MuonDetectorManager* detMgr() const { return m_detMgr; }

    private:
        std::vector<Identifier> binPlusneighbours(const Identifier& id) const;

        std::array<std::unique_ptr<MdtIntersectGeometry>, MuonGM::MuonDetectorManager::MdtRElMaxHash> m_geometry{};
        const IMuonIdHelperSvc* m_idHelperSvc{nullptr};
        const MuonGM::MuonDetectorManager* m_detMgr{nullptr};
        const MdtCondDbData* m_dbData{nullptr};
    };
}  // namespace Muon

CLASS_DEF(Muon::MuonIntersectGeoData, 211728888, 1)
CONDCONT_DEF(Muon::MuonIntersectGeoData, 85983392);
#endif
