/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSTATIONINTESECTCOND_MDTINTERSECTGEOMETRY_H
#define MUONSTATIONINTESECTCOND_MDTINTERSECTGEOMETRY_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonStationIntersectCond/MuonIntersectGeometry.h"
class MsgStream;
namespace MuonGM {
    class MuonDetectorManager;
    class MdtReadoutElement;
}  // namespace MuonGM

class MdtCondDbData;

namespace TrkDriftCircleMath {
    class MdtChamberGeometry;
}
namespace TrkDriftCircleMath {
    class MdtChamberGeometry;
}

namespace Muon {

    class MdtIntersectGeometry : public MuonIntersectGeometry {
    public:
        MdtIntersectGeometry(MsgStream& msg, const Identifier& chid, const IMuonIdHelperSvc* idHelperSvc,
                             const MuonGM::MuonDetectorManager* detMgr, const MdtCondDbData* dbData);

        MdtIntersectGeometry(const MdtIntersectGeometry& right) = delete;
        MdtIntersectGeometry& operator=(const MdtIntersectGeometry& right) = delete;

        virtual ~MdtIntersectGeometry();

        MuonStationIntersect intersection(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const override;

        const Amg::Transform3D& transform() const { return m_transform; }

        const TrkDriftCircleMath::MdtChamberGeometry* mdtChamberGeometry() const;
        const Identifier& chamberId() const { return m_chid; }

    private:
        double tubeLength(const int ml, const int layer, const int tube) const;
        void init(MsgStream& msg);
        void fillDeadTubes(const MuonGM::MdtReadoutElement* mydetEl, MsgStream& msg);

        Identifier m_chid{};
        Amg::Transform3D m_transform;
        std::unique_ptr<TrkDriftCircleMath::MdtChamberGeometry> m_mdtGeometry{};
        const MuonGM::MdtReadoutElement* m_detElMl0{nullptr};
        const MuonGM::MdtReadoutElement* m_detElMl1{nullptr};
        const MuonGM::MuonDetectorManager* m_detMgr{nullptr};  // cannot use ReadCondHandleKey since no athena component
        const MdtCondDbData* m_dbData{nullptr};
        const IMuonIdHelperSvc* m_idHelperSvc{nullptr};
        std::set<Identifier> m_deadTubesML{};
        std::vector<Identifier> m_deadTubes{};
    };

}  // namespace Muon

#endif
