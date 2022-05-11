/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_LINEARSEGMENTMAKERTOOL_H
#define TGC_LINEARSEGMENTMAKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "TrkExInterfaces/IIntersector.h"

class TgcIdHelper;

class TGC_LinearSegmentMakerTool : virtual public Muon::IMuonSegmentMaker, public AthAlgTool {
  public:
    TGC_LinearSegmentMakerTool(const std::string& type, const std::string& name, const IInterface* pIID);

    StatusCode   initialize();
    virtual void find(const std::vector<const Trk::RIO_OnTrack*>&, Trk::SegmentCollection*) const {}

    virtual void find(const std::vector<const Trk::RIO_OnTrack*>&, const std::vector<const Trk::RIO_OnTrack*>&) const {}

    virtual void find(const std::vector<const Muon::MdtDriftCircleOnTrack*>&,
                      const std::vector<const Muon::MuonClusterOnTrack*>&, Trk::SegmentCollection*) const
    {
    }

    virtual void find(const Amg::Vector3D&, const Amg::Vector3D&,
                      const std::vector<const Muon::MdtDriftCircleOnTrack*>&,
                      const std::vector<const Muon::MuonClusterOnTrack*>&, bool, Trk::SegmentCollection*, double,
                      double) const
    {
    }

    virtual void find(const Trk::TrackRoad&                                                road,
                      const std::vector<std::vector<const Muon::MdtDriftCircleOnTrack*> >& mdts,
                      const std::vector<std::vector<const Muon::MuonClusterOnTrack*> >&    clusters,
                      Trk::SegmentCollection* segColl = 0, bool hasPhiMeasurements = false,
                      double momentum = 1e9) const;

  private:
    const TgcIdHelper*                 m_pIdHelper;
    DoubleProperty                     m_fExclChi2;

    ToolHandle<Trk::IIntersector> m_pIntersector{
        this,
        "Intersector",
        "Trk::RungeKuttaIntersector/Trk::RungeKuttaIntersector",
    };
};

#endif  // TGC_LINEARSEGMENTMAKERTOOL_H
