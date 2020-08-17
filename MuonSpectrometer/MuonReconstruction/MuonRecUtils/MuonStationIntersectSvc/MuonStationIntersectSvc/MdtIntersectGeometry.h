/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTINTERSECTGEOMETRY_H
#define MDTINTERSECTGEOMETRY_H

#include "MuonStationIntersectSvc/MuonIntersectGeometry.h"
#include "Identifier/Identifier.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;
namespace MuonGM {
  class MuonDetectorManager;
  class MdtReadoutElement;
}

class MdtCondDbData;

namespace TrkDriftCircleMath {
  class MdtChamberGeometry;
}

namespace Muon {
  class IMuonIdHelperSvc;
  class MdtIntersectGeometry : public MuonIntersectGeometry {
  public:
    MdtIntersectGeometry(const Identifier& chid, const MuonGM::MuonDetectorManager* detMgr, const MdtCondDbData* dbData, MsgStream* msg, const Muon::IMuonIdHelperSvc* idHelp);
    MdtIntersectGeometry(const MdtIntersectGeometry &right);
    MdtIntersectGeometry & operator=(const MdtIntersectGeometry &right);
    ~MdtIntersectGeometry();

    const MuonStationIntersect intersection( const Amg::Vector3D& pos, const Amg::Vector3D& dir) const;
    
    const Amg::Transform3D& transform() const { return m_transform; }

    const TrkDriftCircleMath::MdtChamberGeometry* mdtChamberGeometry() const { return m_mdtGeometry; }

    const Identifier& chamberId() const { return m_chid; }
  private:
    double tubeLength( int ml, int layer, int tube ) const;
    void init(MsgStream* msg);
    void fillDeadTubes(const MuonGM::MdtReadoutElement* mydetEl, MsgStream* msg);

    Identifier                              m_chid;
    Amg::Transform3D                        m_transform;
    TrkDriftCircleMath::MdtChamberGeometry* m_mdtGeometry;
    const MuonGM::MdtReadoutElement*        m_detElMl0;
    const MuonGM::MdtReadoutElement*        m_detElMl1;
    const MuonGM::MuonDetectorManager*      m_detMgr;
    const MdtCondDbData*                    m_dbData;
    const Muon::IMuonIdHelperSvc*           m_idHelperSvc;
    std::set<Identifier>                    m_deadTubesML;
    std::vector<Identifier>                 m_deadTubes;
  };

}


#endif
