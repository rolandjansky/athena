/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVVERTEXRECOTOOL_H
#define MSVVERTEXRECOTOOL_H

#include "MSVertexToolInterfaces/IMSVertexRecoTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "MSVertexUtils/Tracklet.h"
#include "MSVertexUtils/MSVertex.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "StoreGate/WriteDecorHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "xAODTracking/VertexContainer.h"

#include <utility>
#include <string>
#include <vector>

namespace Muon {

  class MSVertexRecoTool : virtual public IMSVertexRecoTool, public AthAlgTool {
  typedef xAOD::VertexContainer decortype;
  public :
    MSVertexRecoTool (const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MSVertexRecoTool()=default;

    virtual StatusCode initialize(void) override;  
    StatusCode findMSvertices(std::vector<Tracklet>& traklets, std::vector<MSVertex*>& vertices, const EventContext &ctx) const override;

    struct TrkCluster {
      float eta;
      float phi;
      int ntrks;
      int trks[100];
      bool isSystematic;
      std::vector<Tracklet> tracks;
      TrkCluster() {
	eta = 0;
	phi = 0;
	ntrks = 0;
        for(unsigned int i=0; i<100; ++i) trks[i]=0;
        isSystematic = 0;
      }
    }; 
    

  private:
    ToolHandle<Trk::IExtrapolator> m_extrapolator {this, "MyExtrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
    float m_BarrelTrackletUncert;
    float m_EndcapTrackletUncert;
    float m_TrackPhiAngle;
    float m_VxChi2ProbCUT;
    float m_VxPlaneDist;
    float m_VertexMaxRadialPlane;
    float m_VertexMinRadialPlane;
    int m_minHighOccupancyChambers;
    float m_ChamberOccupancyMin;
    int m_useOldMSVxEndcapMethod;
    unsigned int m_maxGlobalTracklets;
    unsigned int m_maxClusterTracklets;
    float m_MaxTollDist;
    bool m_doSystematics;

    CLHEP::HepRandomEngine* m_rndmEngine;
    std::string             m_rndmEngineName;
    IAtRndmGenSvc*          m_rndmSvc;

    //barrel vertex reco algorithm
    void MSVxFinder(const std::vector<Tracklet>& tracklets, std::unique_ptr<MSVertex>& vtx, const EventContext &ctx) const;
    //endcap vertex reco algorithm
    void MSStraightLineVx(const std::vector<Tracklet> &trks, std::unique_ptr<MSVertex>& vtx, const EventContext &ctx) const;
    void MSStraightLineVx_oldMethod(const std::vector<Tracklet> &trks, std::unique_ptr<MSVertex>& vtx, const EventContext &ctx )const;
    void MakeDummyVertex(MSVertex*&) const;
    float vxPhiFinder(const float theta, const float phi, const EventContext &ctx) const;//vertex phi location reco algorithm
    void HitCounter(MSVertex* MSRecoVx, const EventContext &ctx) const;//counts MDT, RPC & TGC around a reco'd vertex
    std::vector<TrkCluster> findTrackClusters(const std::vector<Tracklet>& tracklets) const;//group tracklets into clusters -- vertex reco runs on each cluster of tracklets
    TrkCluster ClusterizeTracks(std::vector<Tracklet>& tracks) const;//core algorithm for creating the clusters
    StatusCode FillOutputContainer(std::vector<MSVertex*>&, SG::WriteHandle<xAOD::VertexContainer> &xAODVxContainer, SG::WriteDecorHandle<decortype, int> &, SG::WriteDecorHandle<decortype, int> &, SG::WriteDecorHandle<decortype, int> &) const;
    Amg::Vector3D VxMinQuad(const std::vector<Tracklet> &tracks) const;//endcap vertex reco core
    std::vector<Tracklet> RemoveBadTrk(const std::vector<Tracklet> &tracklets, const Amg::Vector3D &Vx) const;//endcap vertex algo track selector
    bool EndcapHasBadTrack(const std::vector<Tracklet> &tracklets, const Amg::Vector3D &Vx) const;
    std::vector<Tracklet> getTracklets(const std::vector<Tracklet> &trks, const std::set<int> &tracklet_subset) const;

    SG::WriteHandleKey<xAOD::VertexContainer> m_xAODContainerKey{ this, "xAODVertexContainer", "MSDisplacedVertex"};

    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_rpcTESKey{ this, "TESKey", "RPC_Measurements"};
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_tgcTESKey{ this, "TGCKey", "TGC_Measurements"};
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtTESKey{ this, "MDTKey", "MDT_DriftCircles"};

    SG::WriteDecorHandleKey<decortype> m_decor_nMDT{ this, "Decor_MDTK", "nMDT"};
    SG::WriteDecorHandleKey<decortype> m_decor_nRPC{ this, "Decor_nRPC", "nRPC"};
    SG::WriteDecorHandleKey<decortype> m_decor_nTGC{ this, "Decor_nTGC", "nTGC"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  };
  
  
}
#endif //MSVERTEXRECOTOOL_H
