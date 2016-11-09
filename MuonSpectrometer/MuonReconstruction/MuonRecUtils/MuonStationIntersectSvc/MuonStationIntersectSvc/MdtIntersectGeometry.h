/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTINTERSECTGEOMETRY_H
#define MDTINTERSECTGEOMETRY_H

#include "MuonStationIntersectSvc/MuonIntersectGeometry.h"
#include "Identifier/Identifier.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonGM {
  class MuonDetectorManager;
  class MdtReadoutElement;
}

class MdtIdHelper;
class IMDTConditionsSvc;

namespace TrkDriftCircleMath {
  class MdtChamberGeometry;
}

namespace Muon {

  class MdtIntersectGeometry : public MuonIntersectGeometry {
  public:
    MdtIntersectGeometry( const Identifier& chid, const MuonGM::MuonDetectorManager* detMgr, IMDTConditionsSvc* mdtSummarySvc );
    MdtIntersectGeometry(const MdtIntersectGeometry &right);
    MdtIntersectGeometry & operator=(const MdtIntersectGeometry &right);
    ~MdtIntersectGeometry();

    const MuonStationIntersect& intersection( const Amg::Vector3D& pos, const Amg::Vector3D& dir) const;
    
    const Amg::Transform3D& transform() const { return m_transform; }

    const TrkDriftCircleMath::MdtChamberGeometry* mdtChamberGeometry() const { return m_mdtGeometry; }

    const Identifier& chamberId() const { return m_chid; }
  private:
    double tubeLength( int ml, int layer, int tube ) const;
    void init();
    void fillDeadTubes(const MuonGM::MdtReadoutElement* mydetEl);

    Identifier                              m_chid;
    Amg::Transform3D                        m_transform;
    mutable MuonStationIntersect            m_intersect;
    TrkDriftCircleMath::MdtChamberGeometry* m_mdtGeometry;
    const MuonGM::MdtReadoutElement*        m_detElMl0;
    const MuonGM::MdtReadoutElement*        m_detElMl1;
    const MuonGM::MuonDetectorManager*      m_detMgr;
    IMDTConditionsSvc*                      m_mdtSummarySvc;
    const MdtIdHelper*                      m_mdtIdHelper;
    std::set<Identifier>                    m_deadTubesML;
    std::vector<Identifier>                 m_deadTubes;
  };

}


#endif
