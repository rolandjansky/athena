/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_LINEARSEGMENTMAKERTOOL_H
#define TGC_LINEARSEGMENTMAKERTOOL_H

#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "CLHEP/Vector/TwoVector.h"

class TgcIdHelper;

namespace MuonGM
{
    class MuonDetectorManager;
}
namespace Trk
{
    class IIntersector;
}

class TGC_LinearSegmentMakerTool : virtual public Muon::IMuonSegmentMaker, public AthAlgTool
{
public:
    TGC_LinearSegmentMakerTool(const std::string& type,
                               const std::string& name,
                               const IInterface* pIID);

    StatusCode initialize();
    StatusCode Finalize();
    virtual std::vector<const Muon::MuonSegment*>*
    find(const std::vector<const Trk::RIO_OnTrack*>&)
    {
        return NULL;
    }

    virtual std::vector<const Muon::MuonSegment*>*
    find(const std::vector<const Trk::RIO_OnTrack*>&,
         const std::vector<const Trk::RIO_OnTrack*>&)
    {
        return NULL;
    }

    virtual std::vector<const Muon::MuonSegment*>*
    find(const std::vector<const Muon::MdtDriftCircleOnTrack*>&,
         const std::vector<const Muon::MuonClusterOnTrack*>&)
    {
        return NULL;
    }

    virtual std::vector<const Muon::MuonSegment*>*
    find(const Amg::Vector3D&,
         const Amg::Vector3D&,
         const std::vector<const Muon::MdtDriftCircleOnTrack*>&,
         const std::vector<const Muon::MuonClusterOnTrack*>&,
         bool,double)
    {
        return NULL;
    }

    virtual std::vector<const Muon::MuonSegment*>*
    find(const Trk::TrackRoad& road,
         const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >& mdts,
         const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >& clusters,
         bool hasPhiMeasurements = false,double momentum=1e9);

private:
    const MuonGM::MuonDetectorManager* m_pMuonMgr;
    const TgcIdHelper*                 m_pIdHelper;
    DoubleProperty                     m_fExclChi2;
    ToolHandle<Trk::IIntersector>      m_pIntersector;
};

#endif //TGC_LINEARSEGMENTMAKERTOOL_H

