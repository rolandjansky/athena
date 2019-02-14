/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_LINEARSEGMENTMAKERTOOL_H
#define RPC_LINEARSEGMENTMAKERTOOL_H

#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

class RpcIdHelper;

namespace MuonGM
{
    class MuonDetectorManager;
}

class RPC_LinearSegmentMakerTool : virtual public Muon::IMuonSegmentMaker, public AthAlgTool
{
public:
    RPC_LinearSegmentMakerTool(const std::string& type,
                               const std::string& name,
                               const IInterface* pIID);

    StatusCode initialize();
    StatusCode Finalize();
    virtual void
      find(const std::vector<const Trk::RIO_OnTrack*>&,
	   Trk::SegmentCollection*) const
    {
    }

    virtual void
    find(const std::vector<const Trk::RIO_OnTrack*>&,
         const std::vector<const Trk::RIO_OnTrack*>&) const
    {
    }

    virtual void
    find(const std::vector<const Muon::MdtDriftCircleOnTrack*>&,
         const std::vector<const Muon::MuonClusterOnTrack*>&,
	 Trk::SegmentCollection*) const
    {
    }

    virtual void
    find(const Amg::Vector3D&,
         const Amg::Vector3D&,
         const std::vector<const Muon::MdtDriftCircleOnTrack*>&,
         const std::vector<const Muon::MuonClusterOnTrack*>&,
         bool,Trk::SegmentCollection*,double,double) const
    {
    }

    virtual void
    find(const Trk::TrackRoad& road,
         const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >& mdts,
         const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >& clusters,
	 Trk::SegmentCollection* segCol=0,
         bool hasPhiMeasurements = false,double momentum=1e9) const;

private:
    void superPoint(const std::vector<const Muon::MuonClusterOnTrack*>& rios,
                    double& baseR,
                    double& z, int& nZ,
                    double& phi, int& nPhi);

    const MuonGM::MuonDetectorManager* m_pMuonMgr;
    const RpcIdHelper*                 m_pIdHelper;
    DoubleProperty                     m_fExclChi2;
};

#endif //RPC_LINEARSEGMENTMAKERTOOL_H
