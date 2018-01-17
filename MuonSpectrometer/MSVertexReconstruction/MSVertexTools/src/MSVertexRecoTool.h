/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVVERTEXRECOTOOL_H
#define MSVVERTEXRECOTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "Identifier/Identifier.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "MSVertexToolInterfaces/IMSVertexRecoTool.h"
#include "MSVertexUtils/Tracklet.h"
#include "MSVertexUtils/MSVertex.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include <utility>
#include <vector>

class StoreGate;
class MsgStream;
class Identifier;

namespace Muon {

  class MSVertexRecoTool : virtual public IMSVertexRecoTool, public AthAlgTool
  {
	
  public :
    /** default constructor */
    MSVertexRecoTool (const std::string& type, const std::string& name, const IInterface* parent);
    /** destructor */
    virtual ~MSVertexRecoTool();

    virtual StatusCode initialize(void);  
    virtual StatusCode finalize(void);  

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
    //add tool handles, private variables, etc here
    ToolHandle <Trk::IExtrapolator> m_extrapolator;
    const MdtIdHelper* m_mdtIdHelper;
    const RpcIdHelper* m_rpcIdHelper;
    const TgcIdHelper* m_tgcIdHelper;
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


  public:

    StatusCode findMSvertices(std::vector<Tracklet>& traklets, std::vector<MSVertex*>& vertices);
    
  private:
    //barrel vertex reco algorithm
    void MSVxFinder(std::vector<Tracklet>& tracklets, MSVertex*& vtx);
    //endcap vertex reco algorithm
    void MSStraightLineVx(std::vector<Tracklet> trks, MSVertex*& vtx);
    void MSStraightLineVx_oldMethod(std::vector<Tracklet> trks, MSVertex*& vtx);
    void MakeDummyVertex(MSVertex*&);
    float vxPhiFinder(float theta,float phi);//vertex phi location reco algorithm
    void HitCounter(MSVertex* MSRecoVx);//counts MDT, RPC & TGC around a reco'd vertex
    std::vector<TrkCluster> findTrackClusters(std::vector<Tracklet>& tracklets);//group tracklets into clusters -- vertex reco runs on each cluster of tracklets
    TrkCluster ClusterizeTracks(std::vector<Tracklet>& tracks);//core algorithm for creating the clusters
    StatusCode FillOutputContainer(std::vector<MSVertex*>&, SG::WriteHandle<xAOD::VertexContainer> &xAODVxContainer);
    Amg::Vector3D VxMinQuad(std::vector<Tracklet> tracks);//endcap vertex reco core
    std::vector<Tracklet> RemoveBadTrk(std::vector<Tracklet> tracklets, Amg::Vector3D Vx);//endcap vertex algo track selector
    bool EndcapHasBadTrack(std::vector<Tracklet> tracklets, Amg::Vector3D Vx);
    std::vector<Tracklet> getTracklets(std::vector<Tracklet> trks, std::set<int> tracklet_subset);
    float sq(float x) { return (x)*(x); }
    SG::WriteHandleKey<xAOD::VertexContainer> m_xAODContainerKey;
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_rpcTESKey;//"RPC_Measurements"
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_tgcTESKey;//"TGC_Measurements"
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtTESKey; //"MDT_DriftCircles" 
  };
  
  
}
#endif //MSVERTEXRECOTOOL_H
