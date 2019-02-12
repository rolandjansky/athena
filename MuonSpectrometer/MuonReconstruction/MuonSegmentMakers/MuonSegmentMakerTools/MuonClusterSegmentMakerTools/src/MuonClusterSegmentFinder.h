/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
//// MuonClusterSegmentFinder.h (c) ATLAS Detector software
//// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
/////////////////////////////////////////////////////////////////////////
#ifndef MUON_MUONCLUSTERSEGMENTFINDER_H
#define MUON_MUONCLUSTERSEGMENTFINDER_H

// STL includes
#include <string>
#include <vector>

// EDM
#include "MuonSegment/MuonSegment.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

// helpers
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"

// FrameWork includes
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "TrkParameters/TrackParameters.h"

#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"

// fwd declares
class TTree;
class TFile;
class PRD_MultiTruthCollection;

namespace Trk {
  class Track;
  class MeasurementBase;
  class ITrackFitter;
  class ITrackAmbiguityProcessorTool;
}

namespace Muon {

  struct candEvent {

    candEvent() : m_segTrkColl(new TrackCollection),m_resolvedTracks(new TrackCollection) {}
    ~candEvent(){
      delete m_segTrkColl;
      delete m_resolvedTracks;
    }

    TrackCollection* segTrkColl(){return m_segTrkColl;}
    std::vector<const MuonClusterOnTrack*>& clusters(){return m_clusters;}
    std::vector<ClusterSeg::Cluster*>& Clust(){return m_Clust;}
    std::vector<std::vector<const MuonClusterOnTrack*>>& hits(){return m_hits;}
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D>>& trackSeeds(){return m_trackSeeds;}
    std::vector<MuonSegmentKey>& keyVector(){return m_keyVector;}
    std::vector<std::vector<ClusterSeg::SpacePoint>>& SPoints(){return m_SPoints;}
    MuonSectorMapping& sectorMapping(){return m_sectorMapping;}
    TrackCollection* resolvedTracks(){return m_resolvedTracks;}
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D>>& resolvedTrackSeeds(){return m_resolvedTrackSeeds;}
    std::vector<std::vector<const MuonClusterOnTrack*>>& resolvedhits(){return m_resolvedhits;}

    TrackCollection* m_segTrkColl;
    std::vector<const MuonClusterOnTrack*> m_clusters;
    std::vector<ClusterSeg::Cluster*> m_Clust;
    std::vector<std::vector<const MuonClusterOnTrack*>> m_hits;
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D>> m_trackSeeds;
    std::vector<MuonSegmentKey> m_keyVector;
    std::vector<std::vector<ClusterSeg::SpacePoint>> m_SPoints;
    MuonSectorMapping m_sectorMapping;
    TrackCollection* m_resolvedTracks;
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D>> m_resolvedTrackSeeds;
    std::vector<std::vector<const MuonClusterOnTrack*>> m_resolvedhits;
  };

  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class MuonLayerHashProviderTool;
  class IMuonPRDSelectionTool;
  class IMuonSegmentMaker;
  class IMuonClusterOnTrackCreator;
  class IMuonTrackToSegmentTool;
  class MuonEDMHelperTool;
  class IMuonTrackCleaner;
  class IMuonSegmentOverlapRemovalTool;
  class IMuonClusterizationTool;

  class MuonClusterSegmentFinder : virtual public IMuonClusterSegmentFinder, public AthAlgTool{
  public:
    /** Default AlgTool functions */
    MuonClusterSegmentFinder(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonClusterSegmentFinder();
    StatusCode initialize();
    StatusCode finalize();

    void getClusterSegments(const Muon::MdtPrepDataContainer* mdtPrdCont,
			    const Muon::RpcPrepDataContainer* rpcPrdCont, const Muon::TgcPrepDataContainer* tgcPrdCont,
			    const PRD_MultiTruthCollection* tgcTruthColl, const PRD_MultiTruthCollection* rpcTruthColl,
			    Trk::SegmentCollection* segColl) const;

    void getClusterSegments(const Muon::MdtPrepDataContainer* mdtPrdCont,
			    std::vector<const Muon::TgcPrepDataCollection*>* tgcCols, std::vector<const Muon::RpcPrepDataCollection*>* rpcCols,
			    const PRD_MultiTruthCollection* tgcTruthColl, const PRD_MultiTruthCollection* rpcTruthColl,
			    Trk::SegmentCollection* segColl) const;

    /** tgc segment finding */
    void findSegments(std::vector<const TgcPrepDataCollection*>& tgcCols, const Muon::MdtPrepDataContainer* mdtPrdCont, Trk::SegmentCollection* segColl,
		      const PRD_MultiTruthCollection* tgcTruthColl) const;
    /** rpc segment finding */
    void findSegments(std::vector<const RpcPrepDataCollection*>& rpcCols, const Muon::MdtPrepDataContainer* mdtPrdCont, Trk::SegmentCollection* segColl,
		      const PRD_MultiTruthCollection* tgcTruthColl) const;

  private:
    ToolHandle<MuonIdHelperTool>                      m_idHelper; 
    ToolHandle<MuonEDMPrinterTool>                    m_printer; 
    ToolHandle<MuonLayerHashProviderTool>             m_layerHashProvider;
    ToolHandle<IMuonPRDSelectionTool>                 m_muonPRDSelectionTool;
    ToolHandle<IMuonSegmentMaker>                     m_segmentMaker;
    ToolHandle<Muon::IMuonClusterizationTool>         m_clusterTool;     //<! clustering tool
    ToolHandle<IMuonClusterOnTrackCreator>            m_clusterCreator;
    ToolHandle<IMuonTrackToSegmentTool>               m_trackToSegmentTool; //<! track to segment converter
    ToolHandle<Trk::ITrackFitter>                     m_slTrackFitter;  //<! fitter, always use straightline
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>     m_ambiguityProcessor; //!< Tool for ambiguity solving
    ToolHandle<MuonEDMHelperTool>                     m_helper;   //<! Id helper tool
    ToolHandle<IMuonTrackCleaner>                     m_trackCleaner;
    ToolHandle<IMuonSegmentOverlapRemovalTool>        m_segmentOverlapRemovalTool;

    bool m_doNtuple;
    TFile* m_file;
    TTree* m_tree;
    ClusterSeg::ClusterNtuple* m_ntuple;

    bool matchTruth(const PRD_MultiTruthCollection& truthCol, const Identifier& id, int& barcode) const;
    Trk::Track* fit( const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar ) const;
    void makeClusterVecs(const std::vector<const Muon::MuonClusterOnTrack*>& clustCol, candEvent* theEvent) const;
    void makeClusterVecs(const PRD_MultiTruthCollection* truthCollectionTGC, const std::vector<const TgcPrepDataCollection*>& tgcCols, candEvent* theEvent) const;
    void makeClusterVecs(const PRD_MultiTruthCollection* truthCollectionRPC, const std::vector<const RpcPrepDataCollection*>& rpcCols, candEvent* theEvent) const;
    void findOverlap(std::map<int,bool>& themap,candEvent* theEvent) const;
    void processSpacePoints(candEvent* theEvent,std::vector<std::vector<ClusterSeg::SpacePoint>>& sPoints) const;
    void resolveCollections(std::map<int,bool> themap,candEvent* theEvent) const;
    void getSegments(candEvent* theEvent, const Muon::MdtPrepDataContainer* mdtPrdCont, Trk::SegmentCollection* segColl) const;
    bool getLayerData( int sector, MuonStationIndex::DetectorRegionIndex regionIndex, MuonStationIndex::LayerIndex layerIndex, 
		       const Muon::MdtPrepDataContainer* input, std::vector<const MdtPrepDataCollection*>& output ) const;
  };

}

#endif
