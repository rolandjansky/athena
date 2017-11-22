/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONROADFINDERTOOL_H
#define MUONROADFINDERTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinderTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include <utility>
#include <vector>

class StoreGate;
class MsgStream;
class Identifier;

namespace Muon {

  class IMuonTrackCleaner;

  class MuonClusterSegmentFinderTool : virtual public IMuonClusterSegmentFinderTool, public AthAlgTool
  {
	
  public :
    /** default constructor */
    MuonClusterSegmentFinderTool (const std::string& type, const std::string& name, const IInterface* parent);
    /** destructor */
    virtual ~MuonClusterSegmentFinderTool();
        
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize(void);  
    virtual StatusCode finalize(void);  

  private:
    ToolHandle<Trk::ITrackFitter>                 m_slTrackFitter;  //<! fitter, always use straightline
    mutable ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiTool; //!< Tool for ambiguity solving
    ToolHandle<IMuonTrackToSegmentTool>           m_trackToSegmentTool; //<! track to segment converter
    ToolHandle<MuonIdHelperTool>                  m_idHelperTool;   //<! Id helper tool
    ToolHandle<MuonEDMPrinterTool>                m_printer;   //<! Id helper tool
    ToolHandle<MuonEDMHelperTool>                 m_helper;   //<! Id helper tool
    ToolHandle<IMuonTrackCleaner>                 m_trackCleaner;
    bool                                          m_ipConstraint; // use a ip perigee(0,0) constraint in the segment fit
    double                                        m_maxClustDist;

  public:
    //find segments given a list of MuonCluster
    std::vector<const Muon::MuonSegment*>* find(std::vector< const Muon::MuonClusterOnTrack* >& clusters);
    
  private:
    //reconstruct the segments in the precision (eta) plane
    std::vector<const Muon::MuonSegment*>* findPrecisionSegments(std::vector< const Muon::MuonClusterOnTrack* >& MuonClusters) const;
    //recontruct 3D segments
    std::vector<const Muon::MuonSegment*>* find3DSegments(std::vector< const Muon::MuonClusterOnTrack* >& MuonClusters, std::vector<const Muon::MuonSegment*>* etaSegs) const;
    //clean the clusters
    std::vector< const Muon::MuonClusterOnTrack* > cleanClusters(std::vector< const Muon::MuonClusterOnTrack* >& MuonClusters, bool selectPhiClusters=false) const ;
    //order the clusters
    std::vector< std::vector<const Muon::MuonClusterOnTrack*> > orderByLayer(std::vector< const Muon::MuonClusterOnTrack* >& clusters, bool usePads=false) const;
    //find segment seeds
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > segmentSeed(std::vector< std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, bool usePhi) const;
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > segmentSeedFromPads(std::vector< std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, 
											  const Muon::MuonSegment* etaSeg) const;
    std::vector<std::pair<double,double> > getPadPhiOverlap(std::vector< std::vector<const Muon::MuonClusterOnTrack*> >& pads) const;
    //associate clusters to the segment seeds
    std::vector< const Muon::MuonClusterOnTrack* > getClustersOnSegment(std::vector< std::vector<const Muon::MuonClusterOnTrack*> >& clusters, 
									std::pair<Amg::Vector3D,Amg::Vector3D>& seed) const;
    //distance of cluster to segment seed
    double clusterDistanceToSeed(const Muon::MuonClusterOnTrack* clust, std::pair<Amg::Vector3D,Amg::Vector3D>& seed) const;
    Amg::Vector3D intersectPlane( const Trk::PlaneSurface& surf, const Amg::Vector3D& pos, const Amg::Vector3D& dir ) const;
    Trk::Track* fit( const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar ) const;
  };
  
  
}
#endif //MUONROADFINDERTOOL_H
